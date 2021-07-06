#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "nodes.hpp"
#include "simulation.hpp"
#include "helpers.hpp"
#include "reports.hpp"

using ::testing::Return;
using ::testing::_;

TEST(SimulationTest, Simulate) {

    Factory factory;
    factory.add_ramp(Ramp(1, 10));
    factory.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(Storehouse(1));

    ReceiverPreferences prefs;

    Ramp& r = *(factory.find_ramp_by_id(1));
    r.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));

    Worker& w = *(factory.find_worker_by_id(1));
    w.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));

    simulate(factory, 3, [](Factory&, TimeOffset) {});

    // Robotnik ma pustą kolejkę i bufor.
    EXPECT_EQ(w.cbegin(), w.cend());
    EXPECT_FALSE(w.get_sending_buffer().has_value());

    // Magazyn zawiera półprodukt.
    auto storehouse_it = factory.storehouse_cbegin();
    ASSERT_NE(storehouse_it->cbegin(), storehouse_it->cend());
    EXPECT_EQ(storehouse_it->cbegin()->get_id(), 1);
}

TEST(Ramp_Worker_Storehouse_Test, can_a_package_make_it_all_the_way_through){
    Ramp ramp(1, 3);
    Worker worker(1, 2);
    Storehouse storehouse(1);
    ramp.receiver_preferences_.add_receiver(&worker);
    worker.receiver_preferences_.add_receiver(&storehouse);

    ramp.deliver_goods(1);
    ASSERT_EQ(ramp.get_sending_buffer()->get_id(), 1);
    ramp.send_package();
    EXPECT_FALSE(ramp.get_sending_buffer());
    ramp.deliver_goods(4);
    ASSERT_EQ(ramp.get_sending_buffer()->get_id(), 2);

    EXPECT_FALSE(worker.get_sending_buffer());
    EXPECT_FALSE(worker.get_processing_buffer());
    worker.do_work(1);
    ASSERT_EQ(worker.get_processing_buffer()->get_id(), 1);
    EXPECT_FALSE(worker.get_sending_buffer());
    worker.do_work(2);
    EXPECT_FALSE(worker.get_processing_buffer());
    ASSERT_EQ(worker.get_sending_buffer()->get_id(), 1);
    worker.send_package();
    EXPECT_FALSE(worker.get_processing_buffer());
    EXPECT_FALSE(worker.get_processing_buffer());
    ramp.send_package();
    worker.do_work(3);
    ASSERT_EQ(worker.get_processing_buffer()->get_id(), 2);
    EXPECT_FALSE(worker.get_sending_buffer());
    worker.do_work(4);
    EXPECT_FALSE(worker.get_processing_buffer());
    ASSERT_EQ(worker.get_sending_buffer()->get_id(), 2);
    worker.send_package();
    EXPECT_FALSE(worker.get_processing_buffer());
    EXPECT_FALSE(worker.get_processing_buffer());
}
