#ifndef TASK_H
#define TASK_H

#include <iostream>
#include <vector>

struct Task {
    int key;
    std::vector<double> T_l, E_l;
    double T_s, T_r, T_c, E_c;
    bool cloud;
    double priority, w;

    void print(std::ostream &os)const {
        os << "task_" << key << ":\n\tT_s=" << T_s << ",  T_r=" << T_r << ",  T_c=" << T_c << "\n\tT_l=";
        for (auto const& it: T_l)
            os << it << ",";
        os << "\n\tE_c=" << E_c << ",  E_l=";
        for (auto const& it: E_l)
            os << it << ",";
        os << "\n\tcloud=" << cloud;
        os << "\n\tpriority=" << priority << ",  w=" << w;
    }
};

std::ostream &operator<<(std::ostream &os, Task const &m);

#endif
