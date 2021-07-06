#ifndef NET_SIMULATION_PACKAGE_HPP
#define NET_SIMULATION_PACKAGE_HPP
#include "config.hpp"
#include "types.hpp"
#include <set>

class Package{
public:
    Package();
    Package(ElementID id);
    ~Package();
    ElementID get_id() const {return id_;}
    Package(Package&& other){
        assigned_IDs.erase(id_);
        freed_IDs.erase(id_);
        id_ = other.id_;
        other.id_ = 0;}

    Package& operator = (Package&& copy){
        assigned_IDs.erase(id_);
        freed_IDs.erase(id_);
        id_ = copy.id_;
        copy.id_ = 0;
        return *this;
    }
private:
    ElementID id_;
    static std::set<ElementID> assigned_IDs;
    static std::set<ElementID> freed_IDs;
    static ElementID counter_;
};
#endif //NET_SIMULATION_PACKAGE_HPP

