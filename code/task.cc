#include "task.h"

std::ostream &operator<<(std::ostream &os, Task const &m) {
    return os << "task_" << m.key;
}
