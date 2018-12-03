#ifndef TASK_GRAPH_H
#define TASK_GRAPH_H

#include "doubly_adjacency_list.h"
#include "task.h"

class TaskGraph : public doubly_adjacency_list<Task> {
    inline int key(Task* node) { return node->key; }
    Task *entry, *exit;
public:
    TaskGraph(int num_vert) : doubly_adjacency_list<Task>(num_vert) {}
    void set_entry(Task* t) { entry = t; }
    void set_exit(Task* t) { exit = t; }
};

#endif
