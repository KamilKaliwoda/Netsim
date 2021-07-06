#ifndef NET_SIMULATION_NODES_HPP
#define NET_SIMULATION_NODES_HPP
#include "types.hpp"
#include "storage_types.hpp"
#include <map>
#include <utility>
#include "helpers.hpp"
#include <memory>
#include <optional>
#include <iostream>
#include "config.hpp"

enum class ReceiverType {WORKER, STOREHOUSE};

class IPackageReceiver{
public:
    using const_iterator = std::list<Package>::const_iterator;

    [[nodiscard]] virtual IPackageStockpile::const_iterator cbegin() const = 0;
    [[nodiscard]] virtual IPackageStockpile::const_iterator cend() const = 0;
    [[nodiscard]] virtual IPackageStockpile::const_iterator begin() const = 0;
    [[nodiscard]] virtual IPackageStockpile::const_iterator end() const = 0;

    virtual void receive_package(Package&& p) = 0;
    [[nodiscard]] virtual ElementID get_id() const = 0;
    [[nodiscard]] virtual ReceiverType get_receiver_type() const = 0;
};

class ReceiverPreferences{
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    [[nodiscard]] const_iterator cbegin() const {return preferences_.cbegin();}
    [[nodiscard]] const_iterator cend() const {return preferences_.cend();}
    const_iterator begin() {return preferences_.begin();}
    const_iterator end() {return preferences_.end();}

    explicit ReceiverPreferences(ProbabilityGenerator &pg = probability_generator):pg_(pg){}

    void add_receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);
    IPackageReceiver* choose_receiver();
    [[nodiscard]] const preferences_t& get_preferences() const {return preferences_;};

private:
    ProbabilityGenerator &pg_;
    preferences_t preferences_;
};

class PackageSender{
public:
    PackageSender(PackageSender&&) = default;
    PackageSender() = default;
    std::optional<Package>& get_sending_buffer() {return sending_buffer_;}
    const std::optional<Package>& get_sending_buffer() const {return sending_buffer_;}
    void send_package();

    ReceiverPreferences receiver_preferences_ = ReceiverPreferences();

protected:
    void push_package(Package&& p){sending_buffer_.emplace(std::move(p));}
private:
    std::optional<Package> sending_buffer_ = std::nullopt;
};

class Ramp: public PackageSender{
public:
    Ramp(ElementID id, TimeOffset di): id_(id), di_(di){}
    void deliver_goods(Time t);
    [[nodiscard]] TimeOffset get_delivery_interval() const {return di_;}
    [[nodiscard]] ElementID get_id() const {return id_;}

private:
    ElementID id_;
    TimeOffset di_;
};


class Worker: public PackageSender, public IPackageReceiver{
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> pointer = std::make_unique<PackageQueue>(PackageQueueType::FIFO)): id_(id), pd_(pd), queue_pointer_(std::move(pointer)){}
    void do_work(Time t);
    [[nodiscard]] TimeOffset get_processing_duration() const {return pd_;}
    [[nodiscard]] Time get_package_processing_start_time() const {return work_started_time_;}
    void receive_package(Package && pack) override {queue_pointer_->push(std::move(pack));}
    [[nodiscard]] ElementID get_id() const override {return id_;}
    [[nodiscard]] IPackageStockpile::const_iterator cbegin() const override {return queue_pointer_->cbegin();}
    [[nodiscard]] IPackageStockpile::const_iterator begin() const override {return queue_pointer_->cbegin();}
    [[nodiscard]] IPackageStockpile::const_iterator cend() const override {return queue_pointer_->cend();}
    [[nodiscard]] IPackageStockpile::const_iterator end( ) const override {return queue_pointer_->cend ();}
    [[nodiscard]] ReceiverType get_receiver_type() const override {return ReceiverType::WORKER;}
    std::optional<Package>& get_processing_buffer() {return buffer_work_;};
    const std::optional<Package>& get_processing_buffer() const {return buffer_work_;};
    IPackageQueue* get_queue() const {return &*queue_pointer_;};

private:
    ElementID id_;
    TimeOffset pd_;
    std::unique_ptr<IPackageQueue> queue_pointer_;
    std::optional<Package> buffer_work_ = std::nullopt;
    Time work_started_time_;
};

class Storehouse: public IPackageReceiver{
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO)): id_(id), d_(std::move(d)){};
    [[nodiscard]] ElementID get_id() const override {return id_;}
    void receive_package(Package&& p) override;
    [[nodiscard]] IPackageStockpile::const_iterator cbegin() const override {return d_->cbegin();}
    [[nodiscard]] IPackageStockpile::const_iterator cend() const override {return d_->cend();}
    [[nodiscard]] IPackageStockpile::const_iterator begin() const override {return d_->begin();}
    [[nodiscard]] IPackageStockpile::const_iterator end() const override {return d_->end();}
    [[nodiscard]] ReceiverType get_receiver_type() const override {return ReceiverType::STOREHOUSE;}
private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};



#endif //NET_SIMULATION_NODES_HPP
