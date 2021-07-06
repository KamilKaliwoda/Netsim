#include "nodes.hpp"
#include "types.hpp"

void ReceiverPreferences::add_receiver(IPackageReceiver* r){
    preferences_.insert({r, 1.0});
    size_t size = preferences_.size();
    for(auto &pref: preferences_){
        pref.second = pref.second / size;
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r){
    preferences_.erase(r);
    size_t size = preferences_.size();
    if(!preferences_.empty()) {
        for (auto &pref: preferences_) {
            pref.second = 1.0 / size;
        }
    }

}

IPackageReceiver* ReceiverPreferences::choose_receiver(){
    if(!preferences_.empty()) {
        double p = pg_();
        double sum = 0.0;
        for (auto pref: preferences_) {
            sum += pref.second;
            if (sum >= p) {
                return pref.first;
            }
        }
    }
    return std::nullptr_t();
}

void Storehouse::receive_package(Package&& p) {
    d_->push(std::move(p));
}

void PackageSender::send_package() {
    if(sending_buffer_ != std::nullopt){
        IPackageReceiver* choosed_receiver = receiver_preferences_.choose_receiver();
        choosed_receiver->receive_package(std::move(sending_buffer_.value()));
        sending_buffer_ = std::nullopt;
    }
}

void Ramp::deliver_goods(Time t){
    if(t == 1.0 or (fmodf((t - 1), di_) == 0.0 and (t - 1)/di_ > 0)){
        push_package(Package());
    }
}


void Worker::do_work(Time t){
    if(!buffer_work_.has_value()){
        work_started_time_ = t;
        if(!queue_pointer_->empty()){buffer_work_.emplace(queue_pointer_->pop());}
        }
    if(t - work_started_time_ == pd_ - 1) {
        if (buffer_work_) {
            push_package(std::move(buffer_work_.value()));
            buffer_work_.reset();
        }
    }
}
