#pragma once
#include "Train.hpp"
#include "../container/bpt.hpp"
using namespace std;
namespace sjtu {
class TrainSystem {
   private:
    BPlusTree<String, Train> train_tree;
    friend class Ticket;
    friend class TicketSystem;
    friend class System;
   public:
    TrainSystem(string filename="train_tree.data"): train_tree(filename) {}
    ~TrainSystem(){}
    bool add_train(const Train& new_train);
    bool delete_train(const String& train_id);
    Train find_train(const String& train_id);
    bool release_train(const String& train_id);
    void clean_up();
   

};
}  // namespace sjtu