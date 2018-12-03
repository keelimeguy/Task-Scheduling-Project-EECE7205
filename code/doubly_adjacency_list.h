#ifndef DOUBLY_ADJACENCY_LIST_H
#define DOUBLY_ADJACENCY_LIST_H

#include <vector>
#include <iostream>

template <typename N>
class doubly_adjacency_list {
    struct List_Node {
        std::vector<N*> adj;
        std::vector<N*> pre;
    };

    int num_vert;
    std::vector<List_Node> list;

    virtual int key(N* node) = 0;

public:
    doubly_adjacency_list(int num_vert) : num_vert(num_vert), list(num_vert) {}

    int get_size() { return num_vert; }

    void add_edge(N *u, N *v) {
        list[key(u)].adj.push_back(v);
        list[key(v)].pre.push_back(u);
    }

    void add_dual_edge(N *u, N *v) {
        add_edge(u, v);
        add_edge(v, u);
    }

    std::vector<N*> get_suc(N *u) {
        std::vector<N*> ret(list[key(u)].adj);
        return ret;
    }

    std::vector<N*> get_pre(N *u) {
        std::vector<N*> ret(list[key(u)].pre);
        return ret;
    }

    void print() {
        for (int i = 0; i < num_vert; i++) {
            std::cout << i << ": ";
            for (int j = 0; j < list[i].adj.size(); j++)
                std::cout << *list[i].adj[j] << ", ";
            std::cout << std::endl;
        }
    }
};

#endif
