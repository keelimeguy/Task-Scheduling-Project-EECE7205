#ifndef TASK_SCHEDULE_H
#define TASK_SCHEDULE_H

#include <iostream>
#include <vector>
#include <stack>

#include "task_graph.h"

class TaskSchedule {
    struct TaskAssignment {
        Task* task;
        int assigned_core, sequence;
        double FT_ws, FT_wr, FT_c, FT_l, RT_ws, RT_c, RT_l;

        void print(std::ostream &os)const {
            os << "\n\tcore=" << assigned_core << ",  sequence=" << sequence;
            os << "\n\tFT_ws=" << FT_ws << ",  FT_wr=" << FT_wr << ",  FT_c=" << FT_c << ",  FT_l=" << FT_l;
            os << "\n\tRT_ws=" << RT_ws << ",  RT_c=" << RT_c << ",  RT_l=" << RT_l << "\n";
        }
    };

    int num_cores, num_vert;
    bool offline;

    std::vector<Task*> *cores;
    TaskAssignment* assignments;

    double t_total, e_total;
    TaskGraph *graph;

    double ws_free;
    double *l_free;

    void set_assignment_on_core(Task *task, int core);
    void copy_helper(TaskSchedule *other);

public:
    TaskSchedule(TaskSchedule *other) :
            num_cores(other->num_cores), num_vert(other->num_vert), offline(other->offline),
            t_total(other->t_total), e_total(other->e_total), graph(other->graph), ws_free(other->ws_free)
    { copy_helper(other); }

    TaskSchedule(TaskGraph *graph, int num_vert, int num_cores, bool offline) :
            num_cores(num_cores), num_vert(num_vert), offline(offline),
            t_total(0), e_total(0), graph(graph), ws_free(0)
    {
        cores = new std::vector<Task*>[num_cores+1];
        assignments = new TaskAssignment[num_vert];
        for (int i = 0; i < num_vert; i++) {
            assignments[i].task = NULL;
            assignments[i].assigned_core = -1;
            assignments[i].sequence = 0;
            assignments[i].FT_ws = 0;
            assignments[i].FT_wr = 0;
            assignments[i].FT_c = 0;
            assignments[i].FT_l = 0;
            assignments[i].RT_ws = 0;
            assignments[i].RT_c = 0;
            assignments[i].RT_l = 0;
        }

        l_free = new double[num_cores];
        for (int i = 0; i < num_cores; i++) l_free[i] = 0;
    }

    TaskSchedule(TaskGraph *graph, int num_vert, int num_cores) :
        TaskSchedule(graph, num_vert, num_cores, false)
    {}

    ~TaskSchedule() {
        delete[] cores;
        delete[] assignments;
        delete[] l_free;
    }

    double get_t_total() { return t_total; }
    double get_e_total() { return e_total; }
    int get_assigned_core(Task *task) { return assignments[task->key].assigned_core; }

    std::vector<Task*> get_core(int core) {
        std::vector<Task*> ret(cores[core]);
        return ret;
    }

    void assign_core(Task* task, int core);
    void reassign(Task* task, int core);

    void copy(TaskSchedule* other);
    void print();
};

#endif
