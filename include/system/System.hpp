#pragma once
#include "TicketSystem.hpp"
#include "TrainSystem.hpp"  
#include "UserSystem.hpp"
#include "../parser.hpp"
#include "../memoryriver/memoryriver.hpp"
using namespace std;
namespace sjtu {
    class System {
    private:
        TicketSystem ticket_system;
        TrainSystem train_system;
        UserSystem user_system;
        Input input;;
        int timestamp=0,user_cnt=0;
        MemoryRiver<int, 3> system_river;
        vector<String> logged_in_users;

    public:
        ~System() = default;
        explicit System(const std::string& name);
        void run();
        void add_user();
        void login();
        void logout();
        void query_profile();
        void modify_profile();
        void add_train();
        void delete_train();
        void release_train();
        void query_train();
        void query_ticket();
        void query_transfer_ticket();
        void buy_ticket();
        void refund_ticket();
        void query_order();
        void clean();
        int getPrice(Train& train,String from_station, String to_station)const;
        int getTime(Train& train,int date,String from_station, String to_station)const;
        void printTicket(Train& tr,int date, String from_station, String to_station);
    };

}