#pragma once
#include "User.hpp"
#include "../container/bpt.hpp"
using namespace std;
namespace sjtu{
    class UserSystem
    {
        private:
            BPlusTree<String,User> user_tree;
            friend class System;
        public:
            UserSystem(string filename="user_tree.data"): user_tree(filename) {}
            ~UserSystem(){}
        bool add_user(const User& new_user);
        bool delete_user(String user_id);
        User find_user(String user_id);
        order add_ticket(String user_id,const Ticket& ticket,int num,string status);
        order refund_ticket(String user_id,int pos);     
        bool query_ordered_tickets(const String& user_id);
        bool login(String user_id, String password);
        bool logout(String user_id);
        User modify_user(String user_id, const User& new_user);
        void modify_order(order &o,string new_sta);
        void clean_up();

    };
}