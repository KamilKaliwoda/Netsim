#ifndef NET_SIMULATION_TYPES_HPP
#define NET_SIMULATION_TYPES_HPP
#include <functional>
using ElementID = unsigned int;
using Time = float;
using TimeOffset = float;
using ProbabilityGenerator = std::function<double()>;
#endif //NET_SIMULATION_TYPES_HPP
