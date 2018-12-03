#include "mcc_task_scheduling.h"

#include <cstdlib>

double mcc_find_priority(TaskGraph *graph, Task *task);
void mcc_rand_quicksort(Task **arr, int left, int right);

TaskSchedule _Initial_Scheduling_Algorithm(TaskGraph *graph, Task *tasks, int N, int K, Task **p_tasks);
void __primary_assignment(Task *tasks, int N, int K);
void __task_prioritizing(TaskGraph *graph, Task *tasks, int N, int K, Task **p_tasks);
TaskSchedule __execution_unit_selection(TaskGraph *graph, Task **p_tasks, int N, int K);

TaskSchedule _Task_Migration_Algorithm(TaskGraph *graph, Task **p_tasks, int N, int K, double *t_max, TaskSchedule *orig_schedule);
TaskSchedule __kernel_algorithm(TaskGraph *graph, TaskSchedule *orig_schedule, Task *task, int N, int K, int k);

void __primary_assignment(Task *tasks, int N, int K) {
    for (int i = 0; i < N; i++) {
        double min = tasks[i].T_l[0];
        for (int j = 1; j < K; j++) {
            if (tasks[i].T_l[j] < min)
                min = tasks[i].T_l[j];
        }

        if (tasks[i].T_s + tasks[i].T_r + tasks[i].T_c < min)
            tasks[i].cloud = true;
    }
}

double mcc_find_priority(TaskGraph *graph, Task *task) {
    double max = 0;
    for (auto it: graph->get_suc(task)) {
        double p = it->priority;
        if (p == 0) p = mcc_find_priority(graph, it);
        if (p > max) max = p;
    }
    task->priority = task->w + max;
    return task->priority;
}

void mcc_rand_quicksort(Task **arr, int left, int right) {
    if (left >= right) return;

    int temp = rand()%(1 + right - left) + left;
    std::swap(arr[temp], arr[right]);

    int pivot = arr[right]->priority;

    int i = left, j = left-1;
    for(; i < right; i++) {
        if (arr[i]->priority > pivot) {
            j++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[j+1], arr[right]);

    mcc_rand_quicksort(arr, left, j);
    mcc_rand_quicksort(arr, j+2, right);
}

void __task_prioritizing(TaskGraph *graph, Task *tasks, int N, int K, Task **p_tasks) {
    for (int i = 0; i < N; i++) {
        if (tasks[i].cloud)
            tasks[i].w = tasks[i].T_s + tasks[i].T_r + tasks[i].T_c;
        else {
            for (int j = 0; j < K; j++) tasks[i].w += tasks[i].T_l[j];
            tasks[i].w /= K;
        }
        p_tasks[i] = &tasks[i];
    }
    mcc_find_priority(graph, &tasks[0]);
    mcc_rand_quicksort(p_tasks, 0, N-1);
}

TaskSchedule __execution_unit_selection(TaskGraph *graph, Task **p_tasks, int N, int K) {
    TaskSchedule schedule(graph, N, K);
    for (int i = 0; i < N; i++)
        schedule.assign_core(p_tasks[i], p_tasks[i]->cloud ? 0 : -1);
    return schedule;
}

TaskSchedule _Initial_Scheduling_Algorithm(TaskGraph *graph, Task *tasks, int N, int K, Task **p_tasks) {
    __primary_assignment(tasks, N, K);
    __task_prioritizing(graph, tasks, N, K, p_tasks);
    TaskSchedule schedule = __execution_unit_selection(graph, p_tasks, N, K);
    return schedule;
}

TaskSchedule __kernel_algorithm(TaskGraph *graph, TaskSchedule *orig_schedule, Task *task, int N, int K, int k) {
    TaskSchedule schedule(orig_schedule);
    schedule.reassign(task, k);
    return schedule;
}

TaskSchedule _Task_Migration_Algorithm(TaskGraph *graph, Task **p_tasks, int N, int K, double *t_max, TaskSchedule *orig_schedule) {
    double e_total = orig_schedule->get_e_total();
    double t_total = orig_schedule->get_t_total();
    double e_cur = e_total;
    double e_cur2 = e_total, t_cur2 = t_total;
    TaskSchedule cur_schedule(orig_schedule);

    *t_max = t_total * 1.5;

    bool found = false, change = false;
    int choose_i, choose_j;
    for (int i = 0; i < N; i++) {
        int orig_core = orig_schedule->get_assigned_core(p_tasks[i]);
        if (orig_core == 0) continue;

        for (int j = 0; j < K+1; j++) {
            if (orig_core == j) continue;
            TaskSchedule new_schedule = __kernel_algorithm(graph, orig_schedule, p_tasks[i], N, K, j);

            double e_new = new_schedule.get_e_total();
            double t_new = new_schedule.get_t_total();

            if (t_new <= t_total && e_new < e_cur) {
                cur_schedule.copy(&new_schedule);
                e_cur = e_new;
                found = true;
                change = true;
                choose_i = i;
                choose_j = j;
            } else if (!found && (e_total - e_cur2)/(t_cur2 - t_total) < (e_total - e_new)/(t_new - t_total) && t_new <= *t_max) {
                cur_schedule.copy(&new_schedule);
                e_cur2 = e_new;
                t_cur2 = t_new;
                change = true;
                choose_i = i;
                choose_j = j;
            }
        }
    }

    #ifdef VERBOSE_MIGRATION
        if (change) {
            std::cout << "\nbefore task migration..\n";
            orig_schedule->print();
            std::cout << "\nafter task migration: " << choose_i << "->" << choose_j << "..\n\n";
        } else std::cout << "\nno task migration..\n\n";
    #endif

    return cur_schedule;
}

TaskSchedule MCC_TASK_SCHEDULING_ALGORITHM(TaskGraph *graph, Task *tasks, int N, int K, double *t_max, TaskSchedule *init_schedule) {
    Task* p_tasks[N];
    TaskSchedule schedule = _Initial_Scheduling_Algorithm(graph, tasks, N, K, p_tasks);
    init_schedule->copy(&schedule);

    TaskSchedule new_schedule = _Task_Migration_Algorithm(graph, p_tasks, N, K, t_max, &schedule);
    return new_schedule;
}
