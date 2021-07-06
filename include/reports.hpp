#ifndef NET_SIMULATION_REPORTS_HPP
#define NET_SIMULATION_REPORTS_HPP
#include "factory.hpp"

void generate_structure_report(const Factory& factory, std::ostream& os);
void generate_simulation_turn_report(const Factory& factory, std::ostream& os, Time time);


class InterrvalReportNotifier{
public:
    InterrvalReportNotifier(TimeOffset to){Toffset = to;}
    TimeOffset Toffset;
    bool should_generate_report(Time t){
        return (fmodf(t, Toffset) == 1);
    }
};


class SpecificTurnsReportNotifier{
public:
    SpecificTurnsReportNotifier(std::set<Time> turns){Turns = turns;}
    std::set<Time> Turns;
    bool should_generate_report(Time t) {
        for (auto it = Turns.cbegin(); it != Turns.cend(); it++) {
            if(*it == t){return true;}
        }
        return false;
    }
};
#endif //NET_SIMULATION_REPORTS_HPP
