#include "reports.hpp"
#include "factory.hpp"

void generate_structure_report(const Factory& factory, std::ostream& os){

    std::string report = "\n== LOADING RAMPS ==\n\n";
    std::vector<ElementID> index;

    for(auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); it++){
        index.push_back(it->get_id());
        std::sort(index.begin(), index.end());
    }

    for(auto id: index) {

        std::string ramp = "LOADING RAMP #" + std::to_string(factory.find_ramp_by_id(id)->get_id()) + "\n";
        ramp += "  Delivery interval: " + std::to_string(int(factory.find_ramp_by_id(id)->get_delivery_interval())) + "\n";
        ramp += "  Receivers:\n";
        std::map<IPackageReceiver*, double> prefs = factory.find_ramp_by_id(id)->receiver_preferences_.get_preferences();
        std::vector<ElementID> worker;
        std::vector<ElementID> storehouse;

        for (auto iter = prefs.begin(); iter != prefs.end(); iter++) {
            if (iter->first->get_receiver_type() == ReceiverType::WORKER) {worker.push_back(iter->first->get_id());}
            else if (iter->first->get_receiver_type() == ReceiverType::STOREHOUSE) {storehouse.push_back(iter->first->get_id());}
        }

        std::sort(worker.begin(), worker.end());
        std::sort(storehouse.begin(), storehouse.end());

        for (auto worker_id: worker) {
            ramp += "    worker #" + std::to_string(worker_id) + "\n";
        }

        for (auto storehouse_id: storehouse) {
            ramp += "    storehouse #" + std::to_string(storehouse_id) + "\n";
        }

        ramp += "\n";
        report += ramp;
    }

    report += "\n";
    report += "== WORKERS ==\n\n";
    std::vector<ElementID> new_worker_id;

    for(auto it = factory.worker_cbegin(); it != factory.worker_cend(); it++) {
        new_worker_id.push_back(it->get_id());
        std::sort(new_worker_id.begin(), new_worker_id.end());
    }

    for(auto id: new_worker_id){
        std::string work = "WORKER #" + std::to_string(factory.find_storehouse_by_id(id)->get_id()) + "\n";
        work += "  Processing time: " + std::to_string(int(factory.find_worker_by_id(id)->get_processing_duration())) + "\n";
        work += "  Queue type: " + convert_to_string(factory.find_worker_by_id(id)->get_queue()->get_queue_type()) + "\n";
        work += "  Receivers:\n";
        std::map<IPackageReceiver*, double> prefs = factory.find_worker_by_id(id)->receiver_preferences_.get_preferences();
        std::vector<ElementID> worker;
        std::vector<ElementID> storehouse;

        for(auto iter = prefs.begin(); iter != prefs.end(); iter++){

            if(iter->first->get_receiver_type() == ReceiverType::WORKER){worker.push_back(iter->first->get_id());}
            else if(iter->first->get_receiver_type() == ReceiverType::STOREHOUSE){storehouse.push_back(iter->first->get_id());}
        }

        std::sort(worker.begin(), worker.end());
        std::sort(storehouse.begin(), storehouse.end());

        for(auto storehouse_id: storehouse){
            work += "    storehouse #" + std::to_string(storehouse_id) + "\n";
        }

        for(auto worker_id: worker){
            work += "    worker #" + std::to_string(worker_id) + "\n";
        }

        work += "\n";
        report += work;
    }

    report += "\n";
    report += "== STOREHOUSES ==\n\n";
    std::vector<ElementID> new_storehouse_id;

    for(auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); it++){
        new_storehouse_id.push_back(it->get_id());
        std::sort(new_storehouse_id.begin(), new_storehouse_id.end());
    }

    for(auto id: new_storehouse_id){
        std::string storehouse = "STOREHOUSE #" + std::to_string(factory.find_storehouse_by_id(id)->get_id()) + "\n\n";
        report += storehouse;
    }

    os << report;
}


void generate_simulation_turn_report(const Factory& factory, std::ostream& os, Time time){

    std::string report = "=== [ Turn: " + std::to_string(time) + " ] ===\n";
    report += "\n";
    report += "== WORKERS ==\n\n";
    std::vector<ElementID> worker_id;

    for (auto iter = factory.worker_cbegin(); iter != factory.worker_cend(); iter++){
        worker_id.push_back(iter->get_id());
        std::sort(worker_id.begin(), worker_id.end());
    }

    for  (auto id: worker_id){
        std::string work = "WORKER #" + std::to_string(factory.find_worker_by_id(id)->get_id()) + "\n";

        if (factory.find_worker_by_id(id)->get_processing_buffer() != std::nullopt){
            work += "  PBuffer: #" + std::to_string(factory.find_worker_by_id(id)->get_processing_buffer()->get_id()) +
                   " (pt = " + std::to_string(int(time + 1 - factory.find_worker_by_id(id)->get_package_processing_start_time())) + ")\n";
        } else{work += "  PBuffer: (empty)\n";}

        work += "  Queue:";

        if (factory.find_worker_by_id(id)->get_queue()->empty()){work += " (empty)\n";}
        else {

            for (auto iter = factory.find_worker_by_id(id)->get_queue()->cbegin();
                 iter != factory.find_worker_by_id(id)->get_queue()->cend(); iter++) {
                 work += " #" + std::to_string(iter->get_id()) + '\n';
            }
        }

        if (factory.find_worker_by_id(id)->get_sending_buffer() != std::nullopt) {
            work += "  SBuffer: #" + std::to_string(factory.find_worker_by_id(id)->get_sending_buffer()->get_id()) + "\n";
        } else {work += "  SBuffer: (empty)\n";}

        work += "\n";
        report += work;
    }

    report += "\n";
    report += "== STOREHOUSES ==\n\n";
    std::vector<ElementID> storehouse_id;

    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); it++) {
        storehouse_id.push_back(it->get_id());
        std::sort(storehouse_id.begin(), storehouse_id.end());
    }

    for (auto id: storehouse_id) {
        std::string storehouse = "STOREHOUSE #" + std::to_string(factory.find_storehouse_by_id(id)->get_id()) + "\n";
        storehouse += "  Stock:";

        if (factory.find_storehouse_by_id(id)->cbegin() != factory.find_storehouse_by_id(id)->cend()) {

            for (auto iter = factory.find_storehouse_by_id(id)->cbegin(); iter != factory.find_storehouse_by_id(id)->cend(); iter++) {
                storehouse += " #" + std::to_string(iter->get_id());
            }
            
        } else {storehouse += " (empty)";}

        report += storehouse + "\n\n";
    }

    os << report;
}