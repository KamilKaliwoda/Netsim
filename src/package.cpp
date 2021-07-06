#include "package.hpp"

std::set<ElementID> Package:: assigned_IDs = {};
std::set<ElementID> Package:: freed_IDs = {};
ElementID Package:: counter_ = 1;

Package::Package(){
    if(freed_IDs.size()){
        id_ = *freed_IDs.begin();
        freed_IDs.erase(freed_IDs.begin());
    }else{
        while(assigned_IDs.count(counter_)){
            counter_++;
        }
        id_ = counter_;
        assigned_IDs.insert(counter_);
        counter_++;
    }
}

Package::~Package() {
    if(id_ != 0) {
        assigned_IDs.erase(id_);
        freed_IDs.insert(id_);
    }
}

Package::Package(ElementID id) {
    if(assigned_IDs.count(id)){
        throw std::logic_error("This ID of element is already taken.");
    }else{
        id_ = id;
        assigned_IDs.insert(id);
    }
}