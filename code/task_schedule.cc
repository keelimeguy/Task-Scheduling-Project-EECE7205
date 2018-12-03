#include "task_schedule.h"

#include <iostream>
#include <vector>
#include <stack>

void TaskSchedule::copy_helper(TaskSchedule *other) {
    cores = new std::vector<Task*>[num_cores+1];
    for (int i = 0; i < num_cores+1; i++) {
        for (auto const& it: other->cores[i])
            cores[i].push_back(it);
    }

    assignments = new TaskAssignment[num_vert];
    for (int i = 0; i < num_vert; i++) {
        assignments[i].task = other->assignments[i].task;
        assignments[i].assigned_core = other->assignments[i].assigned_core;
        assignments[i].sequence = other->assignments[i].sequence;
        assignments[i].FT_ws = other->assignments[i].FT_ws;
        assignments[i].FT_wr = other->assignments[i].FT_wr;
        assignments[i].FT_c = other->assignments[i].FT_c;
        assignments[i].FT_l = other->assignments[i].FT_l;
        assignments[i].RT_ws = other->assignments[i].RT_ws;
        assignments[i].RT_c = other->assignments[i].RT_c;
        assignments[i].RT_l = other->assignments[i].RT_l;
    }

    l_free = new double[num_cores];
    for (int i = 0; i < num_cores; i++) l_free[i] = other->l_free[i];
}

void TaskSchedule::assign_core(Task* task, int core) {
    set_assignment_on_core(task, core);
    core = assignments[task->key].assigned_core;
    assignments[task->key].task = task;
    assignments[task->key].sequence = cores[core].size();
    cores[core].push_back(task);
    if (core == 0) e_total += task->E_c;
    else e_total += task->E_l[core-1];
}

void TaskSchedule::copy(TaskSchedule* other) {
    delete[] cores;
    delete[] assignments;
    delete[] l_free;

    num_cores = other->num_cores;
    num_vert = other->num_vert;
    offline = other->offline;
    t_total = other->t_total;
    e_total = other->e_total;
    graph = other->graph;
    ws_free = other->ws_free;

    copy_helper(other);
}

void TaskSchedule::print() {
    std::cout << "-----------------------------------------\n";
    std::cout << "------------ BEGIN SCHEDULE -------------\n";
    std::cout << "-----------------------------------------\n";
    std::cout << "t_total = " << t_total << "\n";
    std::cout << "e_total = " << e_total << "\n";
    for (int i = 0; i <= num_cores; i++) {
        std::cout << "-------------------------\n";
        std::cout << "    BEGIN CORE " << i << "\n";
        std::cout << "-------------------------\n";
        for (auto const& it: cores[i]) {
            it->print(std::cout);
            assignments[it->key].print(std::cout);
        }
    }
    std::cout << "-----------------------------------------\n";
    std::cout << "------------- END SCHEDULE --------------\n";
    std::cout << "-----------------------------------------\n";
}

void TaskSchedule::reassign(Task* task, int core) {
    int seq = assignments[task->key].sequence;
    int orig_core = assignments[task->key].assigned_core;
    for (int j = seq+1; j < cores[orig_core].size(); j++)
        --assignments[cores[orig_core][j]->key].sequence;

    cores[orig_core].erase(cores[orig_core].begin()+seq);

    double RT_tar = orig_core == 0 ? assignments[task->key].RT_ws : assignments[task->key].RT_l;
    for (seq = 0; seq < cores[core].size(); seq++) {
        if (core == 0 && assignments[cores[core][seq]->key].RT_ws >= RT_tar) break;
        else if (core != 0 && assignments[cores[core][seq]->key].RT_l >= RT_tar) break;
    }
    for (int j = seq; j < cores[core].size(); j++)
        ++assignments[cores[core][j]->key].sequence;

    cores[core].insert(cores[core].begin()+seq, &task, &task+1);
    assignments[task->key].assigned_core = core;
    assignments[task->key].sequence = seq;

    std::stack<Task*> stack;

    std::vector<std::pair<int,int>> ready;
    for (int i = 0; i < num_vert; i++) {
        Task *t = assignments[i].task;
        assignments[i].FT_ws = 0;
        assignments[i].FT_wr = 0;
        assignments[i].FT_c = 0;
        assignments[i].FT_l = 0;
        assignments[i].RT_ws = 0;
        assignments[i].RT_c = 0;
        assignments[i].RT_l = 0;
        ready.push_back(std::make_pair(graph->get_pre(t).size(), assignments[i].sequence));
        // ready.first = # predecessors not scheduled,  ready.second = # prev sequence not scheduled
        if (ready[i].first == 0 && ready[i].second == 0)
            stack.push(t);
    }

    t_total = 0;
    e_total = 0;
    ws_free = 0;
    for (int i = 0; i < num_cores; i++) l_free[i] = 0;

    while (!stack.empty()) {
        Task* v_i = stack.top();
        stack.pop();
        int s = assignments[v_i->key].sequence;
        int k = assignments[v_i->key].assigned_core;
        set_assignment_on_core(v_i, k);

        for (auto it: graph->get_suc(v_i)) {
            ready[it->key].first--;
            if (ready[it->key].first == 0 && ready[it->key].second == 0)
                stack.push(it);
        }

        for (int i = s+1; i < cores[k].size(); i++) {
            ready[cores[k][i]->key].second--;
            if (ready[cores[k][i]->key].first == 0 && ready[cores[k][i]->key].second == 0)
                stack.push(cores[k][i]);
        }

        if (k == 0) e_total += v_i->E_c;
        else e_total += v_i->E_l[k-1];
    }
}

void TaskSchedule::set_assignment_on_core(Task* task, int core) {
    TaskAssignment *assignment = &assignments[task->key];

    if (core <= 0 && !offline) {
        double max = ws_free;
        for (auto it: graph->get_pre(task)) {
            if (assignments[it->key].FT_l > max) max = assignments[it->key].FT_l;
            if (assignments[it->key].FT_ws > max) max = assignments[it->key].FT_ws;
        }
        assignment->RT_ws = max;
        assignment->FT_ws = max + task->T_s;

        max = assignment->FT_ws;
        for (auto it: graph->get_pre(task)) {
            if (assignments[it->key].FT_c > max) max = assignments[it->key].FT_c;
        }
        assignment->RT_c = max;
        assignment->FT_c = max + task->T_c;
        assignment->FT_wr = assignment->FT_c + task->T_r;
    }

    if (core == 0) {
        assignment->assigned_core = 0;
        ws_free = assignment->FT_ws;
        if (assignment->FT_wr > t_total) t_total = assignment->FT_wr;
        return;
    }

    if (core < 0) {
        double _RT_l = 0, min = assignment->FT_wr, pre_max = 0;
        int assigned_core = 0;
        for (auto it: graph->get_pre(task)) {
            if (assignments[it->key].FT_l > pre_max) pre_max = assignments[it->key].FT_l;
            if (assignments[it->key].FT_wr > pre_max) pre_max = assignments[it->key].FT_wr;
        }
        for (int j = 0; j < num_cores; j++) {
            double max = l_free[j] > pre_max ? l_free[j] : pre_max;
            double _FT_l = max + task->T_l[j];
            if (_FT_l < min || (offline && j == 0)) {
                _RT_l = max;
                min = _FT_l;
                assigned_core = j+1;
            }
        }
        assignment->RT_l = _RT_l;
        assignment->FT_l = min;
        assignment->assigned_core = assigned_core;

    } else {
        double max = l_free[core-1];
        for (auto it: graph->get_pre(task)) {
            if (assignments[it->key].FT_l > max) max = assignments[it->key].FT_l;
            if (assignments[it->key].FT_wr > max) max = assignments[it->key].FT_wr;
        }
        assignment->RT_l = max;
        assignment->FT_l = max + task->T_l[core-1];
        assignment->assigned_core = core;
    }

    if (assignment->assigned_core == 0) {
        ws_free = assignment->FT_ws;
        assignment->RT_l = 0;
        assignment->FT_l = 0;
        if (assignment->FT_wr > t_total) t_total = assignment->FT_wr;

    } else {
        l_free[assignment->assigned_core-1] = assignment->FT_l;
        assignment->FT_ws = 0;
        assignment->FT_wr = 0;
        assignment->FT_c = 0;
        assignment->RT_ws = 0;
        assignment->RT_c = 0;
        if (assignment->FT_l > t_total) t_total = assignment->FT_l;
    }
}
