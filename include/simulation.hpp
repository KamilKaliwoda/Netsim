#ifndef NET_SIMULATION_SIMULATION_HPP
#define NET_SIMULATION_SIMULATION_HPP
#include "factory.hpp"
#include "reports.hpp"

void simulate(Factory& factory, TimeOffset d, std::function<void (Factory&, Time)> rf);

#endif //NET_SIMULATION_SIMULATION_HPP
