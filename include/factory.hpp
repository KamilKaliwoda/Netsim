#ifndef NET_SIMULATION_FACTORY_HPP
#define NET_SIMULATION_FACTORY_HPP
#include "nodes.hpp"
#include <list>
#include <sstream>
#include <iostream>
#include <vector>


struct ParsedLineData{
    enum class ElementType{
        LOADING_RAMP,
        WORKER,
        STOREHOUSE,
        LINK
    } element_type;
    std::map<std::string, std::string> parameters;
};


template<typename Node>
class NodeCollection{
public:
    using container = typename std::list<Node>;
    using iterator = typename container::iterator;
    using const_iterator = typename container::const_iterator;

    [[nodiscard]] const_iterator cbegin() const {return list_.cbegin();}
    [[nodiscard]] const_iterator begin() const {return list_.cbegin();}
    [[nodiscard]] const_iterator cend() const {return list_.cend();}
    [[nodiscard]] const_iterator end() const {return list_.cend();}
    iterator begin() {return list_.begin();}
    iterator end() {return list_.end();}

    void add(Node&& node){list_.push_front(std::move(node));}
    iterator find_by_id(ElementID id){return std::find_if(list_.begin(), list_.end(), [id](const auto& element){return element.get_id() == id;});}
    [[nodiscard]] const_iterator find_by_id(ElementID id) const {return std::find_if(list_.cbegin(), list_.cend(), [id](const auto& elem){return elem.get_id() == id;});}

    void remove_by_id(ElementID id){
        auto i = find_by_id(id);
        if(i != list_.end()){
            list_.erase(i);
        }
    }

private:
    container list_ = {};
};



class Factory{
public:
    void add_ramp(Ramp&& ramp){list_ramp_.add(std::move(ramp));}
    void remove_ramp(ElementID id){list_ramp_.remove_by_id(id);}
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id){return list_ramp_.find_by_id(id);}
    [[nodiscard]] NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const {return list_ramp_.find_by_id(id);}
    [[nodiscard]] NodeCollection<Ramp>::const_iterator ramp_cbegin() const {return list_ramp_.cbegin();}
    [[nodiscard]] NodeCollection<Ramp>::const_iterator ramp_cend() const {return list_ramp_.cend();}

    void add_worker(Worker&& worker){list_worker_.add(std::move(worker));}
    void remove_worker(ElementID id);
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id){return list_worker_.find_by_id(id);}
    [[nodiscard]] NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const {return list_worker_.find_by_id(id);}
    [[nodiscard]] NodeCollection<Worker>::const_iterator worker_cbegin() const {return list_worker_.cbegin();}
    [[nodiscard]] NodeCollection<Worker>::const_iterator worker_cend() const {return list_worker_.cend();}

    void add_storehouse(Storehouse&& storehouse){list_storehouse_.add(std::move(storehouse));}
    void remove_storehouse(ElementID id);
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id){return list_storehouse_.find_by_id(id);}
    [[nodiscard]] NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const {return list_storehouse_.find_by_id(id);}
    [[nodiscard]] NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const {return list_storehouse_.cbegin();}
    [[nodiscard]] NodeCollection<Storehouse>::const_iterator storehouse_cend() const {return list_storehouse_.cend();}

    bool is_consistent();
    void do_deliveries(Time t);
    void do_package_passing();
    void do_work(Time t);
private:
    NodeCollection<Ramp> list_ramp_ = {};
    NodeCollection<Worker> list_worker_ = {};
    NodeCollection<Storehouse> list_storehouse_ = {};

    template<typename Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id){
        for(auto& i: collection){
            auto pref = i.receiver_preferences_.get_preferences();
            for(auto k: pref){
                if((*k.first).get_id() == id){
                    i.receiver_preferences_.remove_receiver(k.first);
                }
            }
        }
    }
};

std::string convert_to_string(PackageQueueType type);

ParsedLineData parse_line(std::string line);

PackageQueueType convert(std::string str);

Factory load_factory_structure(std::istream& is);

void save_factory_structure(Factory& factory, std::ostream& os);

#endif //NET_SIMULATION_FACTORY_HPP
