#include <simulation.hpp>

void simulate(Factory& factory, TimeOffset d, std::function<void (Factory&, Time)> rf){

    if(!factory.is_consistent()){throw std::logic_error("Inconsistent network");}

    for(TimeOffset timeOffset = 1; timeOffset <= d; timeOffset++){
        factory.do_deliveries(timeOffset);
        factory.do_package_passing();
        factory.do_work(timeOffset);
        rf(factory, timeOffset);
    }
}