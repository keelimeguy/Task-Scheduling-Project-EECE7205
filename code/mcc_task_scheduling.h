#ifndef MCC_TASK_SCHEDULING_H
#define MCC_TASK_SCHEDULING_H

#include "task_schedule.h"

TaskSchedule MCC_TASK_SCHEDULING_ALGORITHM(TaskGraph *graph, Task *tasks, int N, int K, double *t_max, TaskSchedule *init_schedule);

#endif
