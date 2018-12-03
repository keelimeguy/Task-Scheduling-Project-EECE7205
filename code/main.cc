#include "params.h"

#include "mcc_task_scheduling.h"
#include "baseline_algorithm_1.h"
#include "baseline_algorithm_2.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <set>
#include <chrono>

using Clock=std::chrono::high_resolution_clock;

// return random double within: [0, max]
inline double rand_double(double max) { return (double) rand() / RAND_MAX * max; }
// (non-zero version): [max/(RAND_MAX+1), max]
inline double rand_double_nz(double max) { return ((double)rand()+1.0) / ((double)RAND_MAX+1.0) * max; }

void gen_rand_tasks(Task *tasks, int N, int K, double T_avg_l, double T_avg_s, double T_avg_r, double T_avg_c, double B, double *P, double P_s) {
    for (int i = 0; i < N; i++) {
        tasks[i].key = i;

        tasks[i].cloud = false;
        tasks[i].priority = 0;
        tasks[i].w = 0;

        tasks[i].T_s = (T_avg_s + rand_double(2*T_avg_s))/2;
        tasks[i].T_r = (T_avg_r + rand_double(2*T_avg_r))/2;
        tasks[i].T_c = (T_avg_c + rand_double(2*T_avg_c))/2;

        tasks[i].T_l.reserve(K);
        tasks[i].T_l.push_back((T_avg_l + rand_double(2*T_avg_l))/2);
        for (int j = 1; j < K; j++)
            tasks[i].T_l.push_back(rand_double_nz(tasks[i].T_l[j-1]/B));

        tasks[i].E_c = P_s * tasks[i].T_s;
        tasks[i].E_l.reserve(K);
        for (int j = 0; j < K; j++)
            tasks[i].E_l.push_back(P[j] * tasks[i].T_l[j]);
    }
}

void gen_rand_task_graph(TaskGraph *graph, Task *tasks, int N, double a) {
    graph->set_entry(&tasks[0]);
    graph->set_exit(&tasks[N-1]);

    std::set<int> open;
    open.insert(0);

    int nodes = 1;
    if (N>2) {
        int ranks = rand()%(N-2)+1;
        for (int i = 0; i < ranks; i++) {
            int new_nodes = i<ranks-1 ? rand()%(N-nodes-ranks+i)+1 : N-nodes-1;
            std::set<int> open_copy(open);
            for (int k = 0; k < new_nodes; k++) {
                bool found = false;
                open.insert(k+nodes);
                for (auto const& it: open_copy) {
                    if (rand_double(1) < a || nodes == 1) {
                        graph->add_edge(&tasks[it], &tasks[k+nodes]);
                        open.erase(it);
                        found = true;
                    }
                }
                if (!found)
                    graph->add_edge(&tasks[0], &tasks[k+nodes]);
            }
            nodes += new_nodes;
        }
        for (auto const& it: open) {
            graph->add_edge(&tasks[it], &tasks[N-1]);
        }

    } else if (N==2) {
        graph->add_edge(&tasks[0], &tasks[1]);
    }
}

int main() {
    srand(time(0));
    Clock::now();

    double T_MAX = 0.0;

    double P[NUM_CORES] = CORE_CONSUMPTION;
    Task tasks[NUM_TASKS];
    gen_rand_tasks(tasks, NUM_TASKS, NUM_CORES, T_AVG_L, T_AVG_S, T_AVG_R, T_AVG_C, CORE_TIME_FACTOR, P, P_S);

    TaskGraph graph(NUM_TASKS);
    gen_rand_task_graph(&graph, tasks, NUM_TASKS, GRAPH_DENSITY);

    std::cout << "Parameters:\n";
    print_params(T_MAX);
    std::cout << "\nTask Graph:\n";
    graph.print();

    std::cout << "\nMCC Task Scheduling Algorithm:\n";
    TaskSchedule init_schedule(&graph, NUM_TASKS, NUM_CORES);

    Clock::time_point start = Clock::now();
    TaskSchedule schedule = MCC_TASK_SCHEDULING_ALGORITHM(&graph, tasks, NUM_TASKS, NUM_CORES, &T_MAX, &init_schedule);
    Clock::time_point end = Clock::now();
    double seconds = (std::chrono::duration_cast<std::chrono::duration<double>>(end - start)).count();
    double time_mcc = seconds*1000.0;

    schedule.print();

    std::cout << "\nBaseline 1:\n";

    start = Clock::now();
    TaskSchedule schedule_b1 = BASELINE_ALGORITHM_1(&graph, tasks, NUM_TASKS, NUM_CORES, T_MAX);
    end = Clock::now();
    seconds = (std::chrono::duration_cast<std::chrono::duration<double>>(end - start)).count();
    double time_b1 = seconds*1000.0;

    schedule_b1.print();

    std::cout << "\nBaseline 2:\n";

    start = Clock::now();
    TaskSchedule schedule_b2 = BASELINE_ALGORITHM_2(&graph, tasks, NUM_TASKS, NUM_CORES, T_MAX);
    end = Clock::now();
    seconds = (std::chrono::duration_cast<std::chrono::duration<double>>(end - start)).count();
    double time_b2 = seconds*1000.0;

    schedule_b2.print();

    std::cout << "\nComparisons:\tINITIAL\t\tMCC\t\tB1\t\tB2\n";
    std::cout << "t_total =   \t" << init_schedule.get_t_total() << "\t\t" << schedule.get_t_total() << "\t\t" << schedule_b1.get_t_total() << "\t\t" << schedule_b2.get_t_total() << std::endl;
    std::cout << "e_total =   \t" << init_schedule.get_e_total() << "\t\t" << schedule.get_e_total() << "\t\t" << schedule_b1.get_e_total() << "\t\t" << schedule_b2.get_e_total() << std::endl;
    std::cout << "time(ms) =   \t\t\t" << time_mcc << "\t\t" << time_b1 << "\t\t" << time_b2 << std::endl;

    return 0;
}
