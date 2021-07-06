#include "factory.hpp"
#include "nodes.hpp"
#include "types.hpp"
#include <map>


enum class NodeColor {UNVISITED, VISITED, VERIFIED};

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors){
    if(node_colors[sender] == NodeColor::VERIFIED){
        return true;
    }
    node_colors[sender] = NodeColor::VISITED;
    if(sender->receiver_preferences_.get_preferences().empty()){
        throw std::logic_error("Receiver was not found.");
    }
    bool does_sender_not_receiver = false;
    for(const auto& it : sender->receiver_preferences_.get_preferences()){
        if(it.first->get_receiver_type() == ReceiverType::STOREHOUSE){
            does_sender_not_receiver = true;
        }else if(it.first->get_receiver_type() == ReceiverType::WORKER){
            IPackageReceiver* receiver_ptr = it.first;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto send_rec_ptr = dynamic_cast<PackageSender*>(worker_ptr);

            if(send_rec_ptr == sender){
                break;
            }else {
                does_sender_not_receiver = true;
                if (node_colors[send_rec_ptr] == NodeColor::UNVISITED) {
                    try {
                        has_reachable_storehouse(send_rec_ptr, node_colors);
                    }
                    catch (const std::logic_error&) {
                        throw std::logic_error("Receiver was not found.");
                    }
                }
            }
        }
    }
    node_colors[sender] = NodeColor::VERIFIED;
    if (does_sender_not_receiver){
        return true;
    }else{
        throw std::logic_error("Receiver was not found.");
    }
}


bool Factory::is_consistent(){
    std::map<const PackageSender*, NodeColor> node_colors;
    for(auto& ramp : list_ramp_){
        Ramp* n_ramp = &ramp;
        const PackageSender* sender = dynamic_cast<Ramp*>(n_ramp);
        node_colors[sender] = NodeColor::UNVISITED;
    }
    for(auto & worker : list_worker_){
        Worker* n_worker = &worker;
        const PackageSender* sender = dynamic_cast<Worker*>(n_worker);
        node_colors[sender] = NodeColor::UNVISITED;
    }
    try{
        for(auto & ramp : list_ramp_){
            Ramp *n_ramp = &ramp;
            const PackageSender* sender = dynamic_cast<Ramp*>(n_ramp);
            has_reachable_storehouse(sender, node_colors);
        }
    }
    catch(const std::logic_error&){
        return false;
    }
    return true;
}

void Factory::do_deliveries(Time t){
    for(auto& it : list_ramp_){
        it.deliver_goods(t);
    }
}

void Factory::do_work(Time t){
    for(auto& it : list_worker_){
        it.do_work(t);
    }
}

void Factory::do_package_passing(){
    for(auto& it : list_worker_){
        it.send_package();
    }
    for(auto& it : list_ramp_) {
        it.send_package();
    }
}

void Factory::remove_worker(ElementID id){
    remove_receiver<Ramp>(list_ramp_, id);
    remove_receiver<Worker>(list_worker_, id);
    list_worker_.remove_by_id(id);
}

void Factory::remove_storehouse(ElementID id){
    remove_receiver<Ramp>(list_ramp_, id);
    remove_receiver<Worker>(list_worker_, id);
    list_storehouse_.remove_by_id(id);
}

ParsedLineData parse_line(std::string line){
    ParsedLineData new_line;
    std::vector<std::string> tokens;
    std::string token;

    std::istringstream token_stream(line);
    char delimiter = ' ';

    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(token);
    }

    if(*tokens.begin() == "LOADING_RAMP"){
        new_line.element_type = ParsedLineData::ElementType::LOADING_RAMP;
    }else if(*tokens.begin() == "WORKER"){
        new_line.element_type = ParsedLineData::ElementType::WORKER;
    }else if(*tokens.begin() == "STOREHOUSE"){
        new_line.element_type = ParsedLineData::ElementType::STOREHOUSE;
    }else if(*tokens.begin() == "LINK") {
        new_line.element_type = ParsedLineData::ElementType::LINK;
    }else{
        throw std::logic_error("Wrong element type!");
    }

    for(auto it = tokens.begin() + 1; it != tokens.end(); ++it){
        std::string k_v;
        std::istringstream token_stream_2(*it);
        std::vector<std::string> pairs;
        while(std::getline(token_stream_2, k_v, '=')){
            pairs.push_back(k_v);
        }
        std::string key = pairs[0];
        std::string value = pairs[1];
        new_line.parameters.emplace(key, value);
    }

    return new_line;
}

PackageQueueType convert(std::string str){
    if(str == "LIFO") return LIFO;
    else if(str == "FIFO") return FIFO;
    else throw "Invalid queue type!";
}

std::string convert_to_string(PackageQueueType type){
    if(type == FIFO) return "FIFO";
    else if(type == LIFO) return "LIFO";
    else throw "Invalid queue type";
}

Factory load_factory_structure(std::istream& is){

    Factory factory;
    std::string line;

    while(std::getline(is, line)) {

        if (!line.empty() and line.find(";") == std::string::npos) {
            ParsedLineData data;
            data = parse_line(line);

            switch (data.element_type) {

                case ParsedLineData::ElementType::LOADING_RAMP: {
                    ElementID id = std::stoi(data.parameters.find("id")->second);
                    TimeOffset time = std::stoi(data.parameters.find("delivery-interval")->second);
                    factory.add_ramp(Ramp(id, time));
                    break;
                }

                case ParsedLineData::ElementType::WORKER: {
                    ElementID id = std::stoi(data.parameters.find("id")->second);
                    TimeOffset time = std::stoi(data.parameters.find("processing-time")->second);
                    factory.add_worker(Worker(id, time, std::make_unique<PackageQueue>(convert(data.parameters.find("queue-type")->second))));
                    break;
                }

                case ParsedLineData::ElementType::STOREHOUSE: {
                    ElementID id = std::stoi(data.parameters.find("id")->second);
                    factory.add_storehouse(Storehouse(id));
                    break;
                }

                case ParsedLineData::ElementType::LINK: {
                    std::string k_v;
                    std::stringstream token_stream(data.parameters.begin()->second);
                    std::vector<std::string> f;
                    char delimiter = '-';

                    while (std::getline(token_stream, k_v, delimiter)) {
                        f.push_back(k_v);
                    }

                    std::string k_v_2;
                    std::stringstream token_stream_2(std::next(data.parameters.begin(), 1)->second);
                    std::vector<std::string> s;

                    while (std::getline(token_stream_2, k_v_2, delimiter)) {
                        s.push_back(k_v_2);
                    }

                    ElementID id = std::stoi(f[1]);
                    ElementID id_2 = std::stoi(s[1]);

                    if (data.parameters.begin()->first == "src") {

                        if (f[0] == "ramp") {
                            Ramp& ramp = *(factory.find_ramp_by_id(id));
                            ramp.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(id_2)));

                        } else if (f[0] == "worker" and s[0] == "worker") {
                            Worker& worker = *(factory.find_worker_by_id(id));
                            worker.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(id_2)));

                        } else if (f[0] == "worker" and s[0] == "store") {
                            Worker& worker = *(factory.find_worker_by_id(id));
                            worker.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(id_2)));
                        }

                    } else {

                        if (s[0] == "ramp") {
                            Ramp& ramp = *(factory.find_ramp_by_id(id_2));
                            ramp.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(id)));

                        } else if (s[0] == "worker" and f[0] == "worker") {
                            Worker& worker = *(factory.find_worker_by_id(id_2));
                            worker.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(id)));

                        } else if (s[0] == "worker" and f[0] == "store") {
                            Worker& worker = *(factory.find_worker_by_id(id_2));
                            worker.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(id)));

                        }
                    }
                    break;
                }
                default: {
                    throw std::logic_error("Invalid data");
                }
            }
        }
    }
    return factory;
}

void save_factory_structure(Factory& factory, std::ostream& os){

    std::string save = "; == LOADING_RAMPS ==\n\n";
    std::string link = "; == LINKS ==\n\n";
    std::vector<ElementID> id_vector;

    for(auto iter = factory.ramp_cbegin(); iter != factory.ramp_cend(); iter++){
        id_vector.push_back(iter->get_id());
        std::sort(id_vector.begin(), id_vector.end());
    }

    for(auto id: id_vector){
        std::string ramp = "LOADING_RAMP id=" + std::to_string(factory.find_ramp_by_id(id)->get_id());
        ramp += " delivery-interval=" + std::to_string(int(factory.find_ramp_by_id(id)->get_delivery_interval())) + "\n";
        save += ramp;
        std::map<IPackageReceiver*, double> prefs = factory.find_ramp_by_id(id)->receiver_preferences_.get_preferences();
        std::vector<ElementID> work_id_vector;
        std::vector<ElementID> store_id_vector;

        for(auto it = prefs.begin(); it != prefs.end(); it++){

            if(it->first->get_receiver_type() == ReceiverType::WORKER){work_id_vector.push_back(it->first->get_id());}
            else if(it->first->get_receiver_type() == ReceiverType::STOREHOUSE){store_id_vector.push_back(it->first->get_id());}
        }

        std::sort(work_id_vector.begin(), work_id_vector.end());
        std::sort(store_id_vector.begin(), store_id_vector.end());

        for(auto w_id: work_id_vector){
            link += "LINK src=ramp-" + std::to_string(id);
            link += " dest=worker-" + std::to_string(w_id) + "\n";
        }

        for(auto s_id: store_id_vector) {
            link += "LINK src=ramp-" + std::to_string(id);
            link += " dest=store-" + std::to_string(s_id) + "\n";
        }

        link += "\n";
    }

    save += "\n";
    save += "; == WORKERS ==\n\n";
    std::vector<ElementID> new_work_id_vector;

    for(auto it = factory.worker_cbegin(); it != factory.worker_cend(); it++){
        new_work_id_vector.push_back(it->get_id());
        std::sort(new_work_id_vector.begin(), new_work_id_vector.end());
    }

    for(auto id: new_work_id_vector){
        std::string work = "WORKER id=" + std::to_string(factory.find_worker_by_id(id)->get_id());
        work += " processing-time=" + std::to_string(int(factory.find_worker_by_id(id)->get_processing_duration()));
        work += " queue-type=" + convert_to_string(factory.find_worker_by_id(id)->get_queue()->get_queue_type()) + "\n";
        save += work;

        std::map<IPackageReceiver*, double> prefs = factory.find_worker_by_id(id)->receiver_preferences_.get_preferences();
        std::vector<ElementID> work_id_vector;
        std::vector<ElementID> store_id_vector;

        for(auto it = prefs.begin(); it != prefs.end(); it++){

            if(it->first->get_receiver_type() == ReceiverType::WORKER){work_id_vector.push_back(it->first->get_id());}
            else if(it->first->get_receiver_type() == ReceiverType::STOREHOUSE){store_id_vector.push_back(it->first->get_id());}
        }

        std::sort(work_id_vector.begin(), work_id_vector.end());
        std::sort(store_id_vector.begin(), store_id_vector.end());

        for(auto w_id: work_id_vector){
            link += "LINK src=worker-" + std::to_string(id);
            link += " dest=worker-" + std::to_string(w_id) + "\n";
        }

        for(auto s_id: store_id_vector) {
            link += "LINK src=worker-" + std::to_string(id);
            link += " dest=store-" + std::to_string(s_id) + "\n";
        }

        link += "\n";
    }

    save += "\n";
    save += "; == STOREHOUSES ==\n\n";
    std::vector<ElementID> new_store_id_vector;

    for(auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); it++){
        new_store_id_vector.push_back(it->get_id());
        std::sort(new_store_id_vector.begin(), new_store_id_vector.end());
    }

    for(auto id: new_store_id_vector){
        std::string storehouse = "STOREHOUSE id=" + std::to_string(factory.find_storehouse_by_id(id)->get_id()) + "\n";
        save += storehouse;
    }

    save += "\n" + link;
    os << save;
}
