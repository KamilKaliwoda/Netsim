#ifndef NET_SIMULATION_STORAGE_TYPES_HPP
#define NET_SIMULATION_STORAGE_TYPES_HPP

#include <list>
#include "package.hpp"
#include "types.hpp"

enum PackageQueueType{
    FIFO,
    LIFO
};

class IPackageStockpile{
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual IPackageStockpile::const_iterator begin()= 0;
    virtual IPackageStockpile::const_iterator end() = 0;
    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;

    virtual bool empty() = 0;
    virtual void push(Package&& pack) = 0;
    virtual int size() const = 0;
    virtual ~IPackageStockpile() = default;
};

class IPackageQueue: public IPackageStockpile{
public:
    virtual PackageQueueType get_queue_type() = 0;
    virtual Package pop() = 0;
    virtual ~IPackageQueue() = default;
};

class PackageQueue: public IPackageQueue{
private:
    PackageQueueType type_;
    std::list<Package> package_list_ = {};
public:
    explicit PackageQueue(PackageQueueType queue_type):type_(queue_type){};

    IPackageStockpile::const_iterator begin() override {return package_list_.begin();}
    IPackageStockpile::const_iterator end() override {return package_list_.end();}
    IPackageStockpile::const_iterator cbegin() const override {return package_list_.cbegin();}
    IPackageStockpile::const_iterator cend() const override {return package_list_.cend();}

    bool empty() override{return package_list_.empty();}
    int size() const override{return package_list_.size();}
    PackageQueueType get_queue_type() override{return type_;};
    Package pop() override;
    void push(Package&& pack) override{package_list_.emplace_back(std::move(pack));}
    ~PackageQueue() override= default;
};
#endif //NET_SIMULATION_STORAGE_TYPES_HPP