//
// Created by 翟霄 on 2021/7/30.
//
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include <boost/json.hpp>

int main(int argc, char* argv[]) {
    boost::interprocess::managed_shared_memory managed_shm(
            boost::interprocess::open_or_create,
            "shm",
            1024 * 1024 * 5);
    std::string *commandString = managed_shm.find_or_construct<std::string>("Command")("");
    boost::interprocess::named_mutex named_mtx(boost::interprocess::open_or_create, "mtx");
    boost::interprocess::named_condition named_cnd(boost::interprocess::open_or_create, "cnd");
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(named_mtx);

    *commandString = "";

    named_cnd.notify_all();
//    named_cnd.wait(lock);
//    named_mtx.

//    boost::json::object commandObj;
//    commandObj["name"] = "walk";
//    commandObj["begin_node_id"] = "f9e12aee12214301b757841df388be97";
//    commandObj["stepDefine"]
//                                          stepDefine,
//                                          auxiliaryEdge,
//                                          1.0,
//                                          100000,
//                                          EdgeChooseStrategy::RANDOM,
//                                          true);



    return 0;
}