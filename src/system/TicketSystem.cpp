#include "../../include/system/TicketSystem.hpp"
#include <iostream>
#include <algorithm>
using namespace std;
namespace sjtu {

bool TicketSystem::add_ticket(const Train& train) {
    int cnt=0;

    for (int day = train.sale_begin; day <= train.sale_end; day++) {
        for (int i = 0; i < train.stationNum - 1; i++) {
            TicketKey key{day+train.date[i], train.stations[i]};
            for(int j=i+1;j<train.stationNum;j++)
            {
                 Ticket ticket(train, train.ID,
                          train.stations[i], train.stations[j], day);
                ticket_tree.insert(key, ticket);
               cnt++;
            }
        }
    }
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
    if (low_res.size() == 0) {cout<<0<<'\n'; return true;}
    if (cmp_type == PRICE)
        low_res.sort(Compare_with_cost);
    else if (cmp_type == TIME)
        low_res.sort(Compare_with_time);
    vector<BPlusTree<TicketKey, Ticket>::Key> final_res;
    for (int i = 0; i < low_res.size(); i++) {
        Ticket t = low_res[i].value;
        if (t.to_station == to_station) {
            final_res.push_back(low_res[i]);
        }
    }
    if (final_res.empty()) {cout<<0<<'\n'; return true;}
    cout << final_res.size() << endl;
    for (auto& item : final_res) {
        Ticket t = item.value;
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
    
    // 预计算第一班车信息，避免重复计算
    vector<pair<Ticket, int>> t1_info; // <ticket, travel_time>
    for (int i = 0; i < low_res.size(); i++) {
        Ticket t1 = low_res[i].value;
        int t1_travel_time = t1.getTime();
        t1_info.push_back({t1, t1_travel_time});
    }
    
    // 优化：只查询必要的天数范围
    int max_search_date = 92; // 8月31日对应的日期值
    for (int i = 0; i < t1_info.size(); i++) {
        Ticket t1 = t1_info[i].first;
        int t1_travel_time = t1_info[i].second;
        
        // 计算第一班车到达中转站的日期
        int t1_arrive_day = t1.date + t1_travel_time / 1440;
        
        // 只查询从第一班车到达日期开始到最大日期的列车
        int start_day = std::max(date, t1_arrive_day);
        for (int day_offset = 0; start_day + day_offset <= max_search_date; ++day_offset) {
            int search_date = start_day + day_offset;
            TicketKey mid_key{search_date, t1.to_station};
            auto mid_res = ticket_tree.find(mid_key);
            
            // 预过滤：只处理目的站匹配的列车
            for (int j = 0; j < mid_res.size(); j++) {
                Ticket t2 = mid_res[j].value;
                if (t2.to_station == to_station) {
                    // 时间约束检查（使用预计算的值）
                    int t1_depart_time = t1.date * 1440;
                    int t1_arrive_time = t1_depart_time + t1_travel_time;
                    int t2_depart_time = t2.date * 1440;
                    
                    if (t1_arrive_time <= t2_depart_time) {
                        final_res.push_back({low_res[i], mid_res[j]});
                    }
                }
            }
        }
    }
    
    if (final_res.empty()) {cout<<0<<'\n'; return true;}
    
    // 预计算所有候选方案的总时间和价格，避免重复计算
    vector<int> total_times;
    vector<int> total_prices;
    for (int i = 0; i < final_res.size(); i++) {
        Ticket t1_a = final_res[i].first.value;
        Ticket t2_a = final_res[i].second.value;
        
        int t1_travel_time = t1_a.getTime();
        int t2_travel_time = t2_a.getTime();
        
        int total_time = (t2_a.date * 1440 + t2_travel_time) - (t1_a.date * 1440);
        int total_price = t1_a.getPrice() + t2_a.getPrice();
        
        total_times.push_back(total_time);
        total_prices.push_back(total_price);
    }
    
    // 根据比较类型选择最优方案
    int best_index = 0;
    
    for (int i = 1; i < final_res.size(); i++) {
        bool update = false;
        if (cmp_type == PRICE) {
            if (total_prices[i] < total_prices[best_index]) {
                update = true;
            } else if (total_prices[i] == total_prices[best_index] && 
                       total_times[i] < total_times[best_index]) {
                update = true;
            }
        } else if (cmp_type == TIME) {
            if (total_times[i] < total_times[best_index]) {
                update = true;
            } else if (total_times[i] == total_times[best_index] && 
                       total_prices[i] < total_prices[best_index]) {
                update = true;
            }
        }
        
        // 如果价格和时间都相同，按车次号比较
        if (!update && total_prices[i] == total_prices[best_index] && 
            total_times[i] == total_times[best_index]) {
            Ticket t1_a = final_res[i].first.value;
            Ticket t1_b = final_res[best_index].first.value;
            Ticket t2_a = final_res[i].second.value;
            Ticket t2_b = final_res[best_index].second.value;
            
            if (t1_a.trainID < t1_b.trainID) {
                update = true;
            } else if (t1_a.trainID == t1_b.trainID && t2_a.trainID < t2_b.trainID) {
                update = true;
            }
        }
        
        if (update) {
            best_index = i;
        }
    }
    
    auto ans = final_res[best_index];
    
    ans.first.value.printTicket(ans.first.value.from_station, ans.first.value.to_station);
    ans.second.value.printTicket(ans.second.value.from_station, ans.second.value.to_station);
    return true;
}
bool TicketSystem::buy_ticket(Train &tr, Ticket& ticket, int num, bool if_wait,
                              order& result, const String& UserID) {
    
    if(ticket.date<tr.sale_begin||ticket.date>tr.sale_end) return false;
    int seat_res=tr.get_seat_res(ticket.from_station,ticket.to_station,
                                    ticket.date);
    if(seat_res<num)
     {
        if(!if_wait) return false;
        else
        {
            result.ticket=ticket;
            result.num=num;
            result.UserID=UserID;
            std::strcpy(result.status, "pending");
            cout<<"queue"<<std::endl;
            return true;
        }
     }
    else {
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
void TicketSystem::flush_all() {
    ticket_tree.flush_all();
}
}  // namespace sjtu