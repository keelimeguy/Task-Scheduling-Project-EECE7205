#ifndef PARAMS_H
#define PARAMS_H

#ifndef NUM_TASKS
#define NUM_TASKS           5
#endif
#ifndef NUM_CORES
#define NUM_CORES           3
#endif
#ifndef CORE_CONSUMPTION
#define CORE_CONSUMPTION   {1, 2, 4}
#endif
#ifndef P_S
#define P_S                 0.5
#endif
#ifndef T_AVG_L
#define T_AVG_L             120
#endif
#ifndef T_AVG_S
#define T_AVG_S             3
#endif
#ifndef T_AVG_R
#define T_AVG_R             6
#endif
#ifndef T_AVG_C
#define T_AVG_C             9
#endif
#ifndef CORE_TIME_FACTOR
#define CORE_TIME_FACTOR    1.1
#endif
#ifndef GRAPH_DENSITY
#define GRAPH_DENSITY       0.4
#endif
// #ifndef T_MAX
// #define T_MAX               (T_AVG_L*NUM_TASKS/NUM_CORES)    // changed to 1.5 * initial scheduling time
// #endif
void print_params(double T_MAX);

#endif
