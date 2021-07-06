#include "storage_types.hpp"

Package PackageQueue::pop(){
    if (package_list_.empty()){
        throw std::logic_error("You can't use this method, because list is empty.");
    }
    Package it;
    switch (type_){
        case PackageQueueType::LIFO:{
            it = std::move(package_list_.back());
            package_list_.pop_back();
            break;
        }
        case PackageQueueType::FIFO:{
            it = std::move(package_list_.front());
            package_list_.pop_front();
            break;
        }
    }
    return it;
}