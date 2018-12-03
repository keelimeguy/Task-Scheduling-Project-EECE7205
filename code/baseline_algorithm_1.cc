#include "baseline_algorithm_1.h"

#include <cstdlib>

double find_priority(TaskGraph *graph, Task *task);
void rand_quicksort(Task **arr, int left, int right);

TaskSchedule _Modified_Initial_Scheduling_Algorithm(TaskGraph *graph, Task *tasks, int N, int K, Task **p_tasks);
void __modified_task_prioritizing(TaskGraph *graph, Task *tasks, int N, int K, Task **p_tasks, int *L);
TaskSchedule __modified_execution_unit_selection(TaskGraph *graph, Task **p_tasks, int N, int K);

double find_priority(TaskGraph *graph, Task *task) {
    double max = 0;
    for (auto it: graph->get_suc(task)) {
        double p = it->priority;
        if (p == 0) p = find_priority(graph, it);
        if (p > max) max = p;
    }
    task->priority = task->w + max;
}

void rand_quicksort(Task **arr, int left, int right) {
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

    rand_quicksort(arr, left, j);
    rand_quicksort(arr, j+2, right);
}

void __modified_task_prioritizing(TaskGraph *graph, Task *tasks, int N, int K, Task **p_tasks, int* L) {
    for (int i = 0; i < N; i++) {
        if (L[i] == 0)
            tasks[i].w = tasks[i].T_s + tasks[i].T_r + tasks[i].T_c;
        else
            tasks[i].w += tasks[i].T_l[L[i]];
        p_tasks[i] = &tasks[i];
    }
    find_priority(graph, &tasks[0]);
    rand_quicksort(p_tasks, 0, N-1);
}

TaskSchedule __modified_execution_unit_selection(TaskGraph *graph, Task **p_tasks, int N, int K, int* L) {
    TaskSchedule schedule(graph, N, K);
    for (int i = 0; i < N; i++)
        schedule.assign_core(p_tasks[i], L[p_tasks[i]->key]);
    return schedule;
}

TaskSchedule _Modified_Initial_Scheduling_Algorithm(TaskGraph *graph, Task *tasks, int N, int K, Task **p_tasks, int *L) {
    __modified_task_prioritizing(graph, tasks, N, K, p_tasks, L);
    TaskSchedule schedule = __modified_execution_unit_selection(graph, p_tasks, N, K, L);
    return schedule;
}

TaskSchedule BASELINE_ALGORITHM_1(TaskGraph *graph, Task *tasks, int N, int K, double t_max) {
    TaskSchedule cur_schedule(graph, N, K);
    double e_cur = 0;
    bool found = false;
    for (int i = 0; i < 10000; i++) {
        int L[N];
        for (int t = 0; t < N; t++) {
            L[t] = rand()%(K+1);
        }
        Task* p_tasks[N];
        TaskSchedule new_schedule = _Modified_Initial_Scheduling_Algorithm(graph, tasks, N, K, p_tasks, L);
        double t_new = new_schedule.get_t_total();
        double e_new = new_schedule.get_e_total();
        if (e_new < e_cur || !found) {
            cur_schedule.copy(&new_schedule);
            e_cur = e_new;
            found = true;
        }
    }
    return cur_schedule;
}
