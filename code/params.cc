#include "params.h"

#include <iostream>
#include <vector>

void print_params(double T_MAX) {
    std::cout << "NUM_TASKS: " << NUM_TASKS << std::endl;
    std::cout << "NUM_CORES: " << NUM_CORES << std::endl;
    std::cout << "CORE_CONSUMPTION: ";
    std::vector<double> arr = CORE_CONSUMPTION;
    for (auto const& it: arr)
        std::cout << it << ", ";
    std::cout << std::endl;
    std::cout << "P_S: " << P_S << std::endl;
    std::cout << "T_AVG_L: " << T_AVG_L << std::endl;
    std::cout << "T_AVG_S: " << T_AVG_S << std::endl;
    std::cout << "T_AVG_R: " << T_AVG_R << std::endl;
    std::cout << "T_AVG_C: " << T_AVG_C << std::endl;
    std::cout << "CORE_TIME_FACTOR: " << CORE_TIME_FACTOR << std::endl;
    std::cout << "GRAPH_DENSITY: " << GRAPH_DENSITY << std::endl;
    std::cout << "T_MAX: " << T_MAX << std::endl;
}
