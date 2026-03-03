#include "../../include/system/TicketSystem.hpp"
#include <iostream>
#include <time.h>
using namespace std;
namespace sjtu {

bool TicketSystem::add_ticket(const Train& train) {
    // std::cerr<<"here!!\n";
    // clock_t S=clock();
    int cnt=0;

    // std:cerr<<train.date.size()<<' '<<train.stationNum<<'\n';

    for (int day = train.sale_begin; day <= train.sale_end; day++) {
        for (int i = 0; i < train.stationNum - 1; i++) {
            TicketKey key{day+train.date[i], train.stations[i]};
            for(int j=i+1;j<train.stationNum;j++)
            {
                // std::cerr<<"!!"<<train.seat_res.size()<<'\n';
                //  std::cerr<<train.stations.size()<<' '<<train.stations[0]<<'\n';
                // std::cerr<<"adding ticket "<<train.ID<<' '<<train.stations[i]<<' '<<train.stations[j]<<' '<<day<<endl;
                 Ticket ticket(train, train.ID,
                          train.stations[i], train.stations[j], day);
                // std::cerr<<"added ticket "<<train.ID<<' '<<train.stations[i]<<' '<<train.stations[j]<<' '<<day<<endl;
                // clock_t T=clock();
                ticket_tree.insert(key, ticket);
                // cerr<<(double)(T-S)/CLOCKS_PER_SEC<<'\n';
               cnt++;
            }
        }
    }
    // std::cerr<<"addded\n";
    // std::cerr<<"added tickets: "<<cnt<<'\n';
    return true;
}
bool TicketSystem::Compare_with_cost(
    BPlusTree<TicketKey, Ticket>::Key& A,
    BPlusTree<TicketKey, Ticket>::Key& B) {
    if (A.value.getPrice() == B.value.getPrice()) {
        return A.value.trainID < B.value.trainID;
    }
    return A.value.getPrice() < B.value.getPrice();
}
bool TicketSystem::Compare_with_time(
    BPlusTree<TicketKey, Ticket>::Key& A,
    BPlusTree<TicketKey, Ticket>::Key& B) {
    if (A.value.getTime() == B.value.getTime()) {
        return A.value.trainID < B.value.trainID;
    }
    return A.value.getTime() < B.value.getTime();
}


bool TicketSystem::query_ticket(const String& from_station,
                                const String& to_station, int date,
                                CompareType cmp_type) {
    TicketKey low_key{date, from_station};
    TicketKey high_key{date, to_station};
    auto low_res = ticket_tree.find(low_key);
    // std::cerr<<"query ticket from "<<from_station<<' '<<to_station<<' '<<date<<endl;
    // std::cerr<<"low res size "<<low_res.size()<<endl;
    // std::cerr<<low_res.size()<<endl;
    if (low_res.size() == 0) {cout<<0<<'\n'; return true;}
    if (cmp_type == PRICE)
        low_res.sort(Compare_with_cost);
    else if (cmp_type == TIME)
        low_res.sort(Compare_with_time);
    // std::cerr<<"after sort "<<low_res.size()<<endl;
    vector<BPlusTree<TicketKey, Ticket>::Key> final_res;
    for (int i = 0; i < low_res.size(); i++) {
        Ticket t = low_res[i].value;
        // std::cerr<<t.to_station<<'\n';
        if (t.to_station == to_station) {
            final_res.push_back(low_res[i]);
        }
    }
    if (final_res.empty()) {cout<<0<<'\n'; return true;}
    cout << final_res.size() << endl;
    for (auto& item : final_res) {
        Ticket t = item.value;
        // std::cerr<<"ticket "<<t.trainID<<' '<<t.from_station<<' '<<t.to_station<<' '<<t.date<<endl;
       t.printTicket(from_station, to_station);
    }
    return true;
}
bool TicketSystem::query_transfer_ticket(const String& from_station,
                                         const String& to_station, int date,
                                         CompareType cmp_type) {
    TicketKey low_key{date, from_station};
    auto low_res = ticket_tree.find(low_key);
    if (low_res.size() == 0) {cout<<0<<'\n'; return true;}
    vector<pair<BPlusTree<TicketKey, Ticket>::Key,
                BPlusTree<TicketKey, Ticket>::Key>>
        final_res;
    for (int i = 0; i < low_res.size(); i++) {
        Ticket t1 = low_res[i].value;
        TicketKey mid_key{date, t1.to_station};
        auto mid_res = ticket_tree.find(mid_key);
        for (int j = 0; j < mid_res.size(); j++) {
            Ticket t2 = mid_res[j].value;
            if (t2.to_station == to_station) {
                final_res.push_back({low_res[i], mid_res[j]});
            }
        }
    }
    if (final_res.empty()) {cout<<0<<'\n'; return true;}
    pair<BPlusTree<TicketKey, Ticket>::Key, BPlusTree<TicketKey, Ticket>::Key>
        ans = final_res[0];
    for (int i = 1; i < final_res.size(); i++) {
        Ticket t1_a = final_res[i].first.value;
        Ticket t2_a = final_res[i].second.value;
        Ticket t1_b = ans.first.value;
        Ticket t2_b = ans.second.value;
        if (cmp_type == PRICE) {
            if (t1_a.getPrice() + t2_a.getPrice() <
                t1_b.getPrice() + t2_b.getPrice()) {
                ans.first = final_res[i].first;
                ans.second = final_res[i].second;
            } else if (t1_a.getPrice() + t2_a.getPrice() ==
                           t1_b.getPrice() + t2_b.getPrice() &&
                       t1_a.getTime() + t2_a.getTime() <
                           t1_b.getTime() + t2_b.getTime()) {
                ans.first = final_res[i].first;
                ans.second = final_res[i].second;
            }
        } else if (cmp_type == TIME) {
            if (t1_a.getTime() + t2_a.getTime() <
                t1_b.getTime() + t2_b.getTime()) {
                ans.first = final_res[i].first;
                ans.second = final_res[i].second;
            } else if (t1_a.getTime() + t2_a.getTime() ==
                           t1_b.getTime() + t2_b.getTime() &&
                       t1_a.getPrice() + t2_a.getPrice() <
                           t1_b.getPrice() + t2_b.getPrice()) {
                ans.first = final_res[i].first;
                ans.second = final_res[i].second;
            }
        }
        if (t1_a.getTime() + t2_a.getTime() ==
                t1_b.getTime() + t2_b.getTime() &&
            t1_a.getPrice() + t2_a.getPrice() ==
                t1_b.getPrice() + t2_b.getPrice()) {
            if (t1_a.trainID < t1_b.trainID) {
                ans.first = final_res[i].first;
                ans.second = final_res[i].second;
            } else if (t1_a.trainID == t1_b.trainID &&
                       t2_a.trainID < t2_b.trainID) {
                ans.first = final_res[i].first;
                ans.second = final_res[i].second;
            }
        }
    }
    ans.first.value.printTicket(ans.first.value.from_station, ans.first.value.to_station);
    ans.second.value.printTicket(ans.second.value.from_station, ans.second.value.to_station);
    return true;
}
bool TicketSystem::buy_ticket(Train &tr, Ticket& ticket, int num, bool if_wait,
                              order& result, const String& UserID) {
    
    // std::cerr<<ticket.trainID<<' '<<num<<' '<<UserID<<'\n';
    
    // std::cerr<<tr.seat_res.size()<<endl;
    if(ticket.date<tr.sale_begin||ticket.date>tr.sale_end) return false;
    int seat_res=tr.get_seat_res(ticket.from_station,ticket.to_station,
                                    ticket.date);
    // std::cerr<<"seat res "<<seat_res<<endl;
    if(seat_res<num)
     {
        // std::cerr<<"wtf\n";
        if(!if_wait) return false;
        else
        {
            // std::cerr<<"still queue\n";
            // waiting_list.push_back(order{ticket,num,UserID,"queue"});
            result.ticket=ticket;
            result.num=num;
            result.UserID=UserID;
            std::strcpy(result.status, "pending");
            cout<<"queue"<<std::endl;
            return true;
        }
     }
    else {
        // std::cerr<<"success buy\n";
        result.ticket=ticket;
        result.num=num;
        result.UserID=UserID;
        std::strcpy(result.status, "success");
        cout<<ticket.getPrice()*num<<endl;
        return true;
    }
}

void TicketSystem::clean_up() {
    ticket_tree.clean_up();
}
}  // namespace sjtu