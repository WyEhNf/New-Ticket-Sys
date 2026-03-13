#include "../../include/system/System.hpp"

#include <iostream>

using namespace std;
namespace sjtu {
TrainSystem* Ticket::ptr = nullptr;
System::System(const std::string& name)
    : ticket_system(name + "_ticket_tree.data"),
      train_system(name + "_train_tree.data"),
      user_system(name + "_user_tree.data"),
      system_river(name + "_system.data") {
    system_river.initialise();
    system_river.get_info(user_cnt, 3);
    Ticket::ptr = &train_system;
}
void System::run() {
    while (!cin.eof()) {
        try {
            timestamp = input.GetTimestamp();
            string command = input.GetCommand();
            cout << "[" << timestamp << "] ";
            if (command == "add_user") {
                add_user();
            } else if (command == "login") {
                login();
            } else if (command == "logout") {
                logout();
            } else if (command == "query_profile") {
                query_profile();
            } else if (command == "modify_profile") {
                modify_profile();
            } else if (command == "add_train") {
                add_train();
            } else if (command == "delete_train") {
                delete_train();
            } else if (command == "release_train") {
                release_train();
            } else if (command == "query_train") {
                query_train();
            } else if (command == "query_ticket") {
                query_ticket();
            } else if (command == "query_transfer") {
                query_transfer_ticket();
            } else if (command == "buy_ticket") {
                buy_ticket();
            } else if (command == "refund_ticket") {
                refund_ticket();
            } else if (command == "query_order") {
                query_order();
            } else if (command == "clean") {
                clean();
            } else if (command == "exit") {
                for (auto& user_id : logged_in_users) {
                    user_system.logout(user_id);
                }
                logged_in_users.clear();
                std::cout << "bye" << std::endl;
                // Flush all cached data before exit
                ticket_system.flush_all();
                train_system.flush_all();
                user_system.flush_all();
                break;
            } else if (command == "test") {
                // for testing purpose
                for (auto& user_id : logged_in_users) {
                    user_system.logout(user_id);
                }
                logged_in_users.clear();
            }
        } catch (int) {
            cout << "-1" << endl;
        }
    }
}
void System::add_user() {
    auto key = input.GetKey();
    String cur_name;
    bool is_first = (user_cnt == 0);
    User new_user;
    while (key != '\n') {
        String str;
        if (key != 'g') str = input.GetString();
        if (key == 'u') {
            new_user.UserName = str;
        } else if (key == 'i') {
            new_user.UserName = str;
        } else if (key == 'p') {
            new_user.PassWord = str;
        } else if (key == 'n') {
            new_user.name = str;
        } else if (key == 'm') {
            new_user.MailAdr = str;
        } else if (key == 'g') {
            int pri = input.GetInteger();
            new_user.privilege = pri;
        } else if (key == 'c') {
            cur_name = str;
        }
        key = input.GetKey();
    }
    // cout<<new_user.UserName<<' '<<new_user.PassWord<<'
    // '<<new_user.privilege<<endl;
    if (is_first) {
        new_user.privilege = 10;
        user_system.add_user(new_user);
    } else {
        User cur_user = user_system.find_user(cur_name);
        if (cur_user.privilege <= new_user.privilege) throw -1;
        if (user_system.find_user(new_user.UserName) != User()) throw -1;
        user_system.add_user(new_user);
    }
    ++user_cnt;
    system_river.write_info(user_cnt, 3);
    cout << 0 << endl;
}

void System::login() {
    char key = input.GetKey();
    String user_id;
    String password;
    while (key != '\n') {
        if (key == 'u') {
            user_id = input.GetString();
        } else if (key == 'p') {
            password = input.GetString();
        }
        key = input.GetKey();
    }
    // cout<<"login "<<user_id<<' '<<password<<endl;
    if (!user_system.login(user_id, password)) throw -1;
    logged_in_users.push_back(user_id);
    cout << 0 << endl;
}

void System::logout() {
    char key = input.GetKey();
    String user_id = input.GetString();
    if (!user_system.logout(user_id)) throw -1;
    cout << 0 << endl;
}

void System::query_profile() {
    char key = input.GetKey();
    String cur_id, user_id;
    while (key != '\n') {
        if (key == 'u') {
            user_id = input.GetString();
        } else if (key == 'c') {
            cur_id = input.GetString();
        }
        key = input.GetKey();
    }
    User c = user_system.find_user(cur_id);
    if (c == User()) throw -1;
    if (!c.logged_in) throw -1;
    ;
    User u = user_system.find_user(user_id);
    if (u == User()) throw -1;
    if (c.privilege <= u.privilege && cur_id != user_id) throw -1;
    cout << u.UserName << ' ' << u.name << ' ' << u.MailAdr << ' '
         << u.privilege << endl;
}

void System::modify_profile() {
    char key = input.GetKey();
    String cur_username;
    String target_username;
    bool qualified = true;
    User tmp;
    bool isg = 0, isp = 0, isn = 0, ism = 0;
    while (key != '\n') {
        String str;
        if (key != 'g') str = input.GetString();
        if (key == 'p') {
            tmp.PassWord = str;
            isp = 1;
        } else if (key == 'n') {
            tmp.name = str;
            isn = 1;
        } else if (key == 'm') {
            tmp.MailAdr = str;
            ism = 1;
        } else if (key == 'g') {
            int pri = input.GetInteger();
            tmp.privilege = pri;
            isg = 1;
        } else if (key == 'u') {
            target_username = str;
        } else if (key == 'c') {
            cur_username = str;
        }
        key = input.GetKey();
    }
    User original_user = user_system.find_user(target_username);
    if (original_user == User()) throw -1;
    // std::cout<<"have origin"<<std::endl;
    User cur_user = user_system.find_user(cur_username);
    if (cur_user == User()) throw -1;
    // std::cout<<"have target"<<std::endl;
    if (!cur_user.logged_in) throw -1;
    // std::cout<<"cur logged in"<<std::endl;
    User target_user = original_user;
    if (isp) target_user.PassWord = tmp.PassWord;
    if (isn) target_user.name = tmp.name;
    if (ism) target_user.MailAdr = tmp.MailAdr;
    if (isg) target_user.privilege = tmp.privilege;
    if (original_user != cur_user &&
        original_user.privilege >= cur_user.privilege)
        throw -1;
    // std::cout<<"have privilege"<<std::endl;
    if (isg && target_user.privilege >= cur_user.privilege) throw -1;

    // cout<<"Checking privilege: "<<original_user.privilege<<' '
    //     <<target_user.privilege<<' '<<cur_user.privilege<<endl;
    user_system.modify_user(original_user.UserName, target_user);
    cout << target_user.UserName << ' ' << target_user.name << ' '
         << target_user.MailAdr << ' ' << target_user.privilege << endl;
}

void System::add_train() {
    auto key = input.GetKey();
    Train new_train;
    while (key != '\n') {
        String str;
        int num;
        if (key == 'i') {
            new_train.ID = input.GetString();
        } else if (key == 'n') {
            new_train.stationNum = input.GetInteger();
        } else if (key == 'm') {
            new_train.seatNum = input.GetInteger();
        } else if (key == 's') {
            new_train.stations = input.GetStringArray();
        } else if (key == 'p') {
            new_train.prices = input.GetIntegerArray();
        } else if (key == 'x') {
            new_train.startTime = input.GetTime();
        } else if (key == 't') {
            new_train.travelTimes = input.GetIntegerArray();
        } else if (key == 'o') {
            new_train.stopoverTimes = input.GetIntegerArray();
        } else if (key == 'd') {
            new_train.sale_begin = input.GetDate();
            // input.Skip();
            new_train.sale_end = input.GetDate();
        } else if (key == 'y') {
            new_train.type = input.GetChar();
        }
        key = input.GetKey();
    }
    new_train.initialise();
    if (!train_system.add_train(new_train)) throw -1;
    cout << 0 << endl;
}

void System::delete_train() {
    char key = input.GetKey();
    String train_id = input.GetString();
    if (!train_system.delete_train(train_id)) throw -1;
    cout << 0 << endl;
}

void System::release_train() {
    char key = input.GetKey();
    String train_id = input.GetString();
    Train t = train_system.find_train(train_id);
    if (t == Train()) throw -1;
    // std::cerr<<"releasing train "<<train_id<<endl;
    if (!train_system.release_train(train_id)) throw -1;
    // std::cerr << "released\n";
    // 优化：直接使用已查询的 t 对象，避免重复find
    ticket_system.add_ticket(t);
    cout << 0 << endl;
}

void System::query_train() {
    char key = input.GetKey();
    String train_id;
    int date;
    while (key != '\n') {
        if (key == 'i') {
            train_id = input.GetString();
        } else if (key == 'd') {
            date = input.GetDate();
        }
        key = input.GetKey();
    }
    Train train = train_system.find_train(train_id);
    // std::cerr<<"found"<<train.ID<<endl;
    if (train == Train()) throw -1;
    if (date < train.sale_begin || date > train.sale_end) throw -1;
    int time = train.startTime;
    int price = 0;
    // std::cerr<<date<<'\n';
    // std::cerr<<"station num:"<<train.stationNum<<'
    // '<<train.stopoverTimes.size()<<endl;
    cout << train.ID << ' ' << train.type << endl;
    for (int i = 0; i < train.stationNum; i++) {
        String arr_time, leave_time;
        if (i == 0)
            arr_time = "xx-xx xx:xx";
        else {
            arr_time = train.realTime(time + train.travelTimes[i - 1], date);
            time += train.travelTimes[i - 1];
        }
        // std::cerr<<"OK!"<<train.stations[i]<<endl;
        if (i == train.stationNum - 1)
            leave_time = "xx-xx xx:xx";
        else if (i == 0)
            leave_time = train.realTime(time, date);
        else {
            leave_time =
                train.realTime(time + train.stopoverTimes[i - 1], date);
            time += train.stopoverTimes[i - 1];
        }
        // std::cerr<<"OK!"<<train.stations[i]<<endl;
        int res_seat;
        if (i == train.stationNum - 1)
            res_seat = 0;
        else if (!train.is_released())
            res_seat = train.seatNum;
        else
            res_seat = train.get_seat_res(train.stations[i],
                                          train.stations[i + 1], date);
        // cerr<<"HERE!\n";
        cout << train.stations[i] << ' ' << arr_time << " -> " << leave_time
             << ' ' << price << ' ';
        if (i != train.stationNum - 1) {
            price += train.prices[i];
            cout << res_seat << endl;
        } else
            cout << 'x' << endl;
    }
}
void System::query_ticket() {
    char key = input.GetKey();
    String from_station, to_station;
    int date;
    CompareType cmp_type = PRICE;
    while (key != '\n') {
        if (key == 's') {
            from_station = input.GetString();
        } else if (key == 't') {
            to_station = input.GetString();
        } else if (key == 'd') {
            date = input.GetDate();
        } else if (key == 'p') {
            String get_type = input.GetString();
            if (get_type == (string) "time")
                cmp_type = TIME;
            else
                cmp_type = PRICE;
        }
        key = input.GetKey();
    }
    // std::cout<<from_station<<' '<<to_station<<' '<<date<<'
    // '<<(cmp_type==TIME?"time":"price")<<endl;
    if (!ticket_system.query_ticket(from_station, to_station, date, cmp_type))
        throw -1;
}

void System::query_transfer_ticket() {
    char key = input.GetKey();
    String from_station, to_station;
    int date;
    CompareType cmp_type = PRICE;
    while (key != '\n') {
        if (key == 's') {
            from_station = input.GetString();
        } else if (key == 't') {
            to_station = input.GetString();
        } else if (key == 'd') {
            date = input.GetDate();
        } else if (key == 'p') {
            String get_type = input.GetString();
            if (get_type == (string) "time")
                cmp_type = TIME;
            else
                cmp_type = PRICE;
        }
        key = input.GetKey();
    }
    if (!ticket_system.query_transfer_ticket(from_station, to_station, date,
                                             cmp_type))
        throw -1;
}

void System::buy_ticket() {
    char key = input.GetKey();
    Ticket ticket;
    int num;
    bool if_wait = false;
    String user_id;
    while (key != '\n') {
        if (key == 'u') {
            user_id = input.GetString();
        } else if (key == 'i') {
            ticket.trainID = input.GetString();
        } else if (key == 'd') {
            ticket.date = input.GetDate();
        } else if (key == 'f') {
            ticket.from_station = input.GetString();
        } else if (key == 't') {
            ticket.to_station = input.GetString();
        } else if (key == 'n') {
            num = input.GetInteger();
        } else if (key == 'q') {
            String c = input.GetString();
            if (c == (string)("true")) if_wait = true;
        }
        key = input.GetKey();
    }
    User find_user = user_system.find_user(user_id);
    if (find_user == User()) throw -1;
    if (!find_user.logged_in) throw -1;
    // cerr<<"find user"<<user_id<<endl;
    if (!train_system.find_train(ticket.trainID).is_released()) throw -1;
    ;
    // std::cerr<<"release found"<<endl;
    order result(ticket, num, user_id, "");
    // std::cerr<<"here\n";
    Train tr = train_system.find_train(ticket.trainID);
    int pos = -1;
    for (int i = 0; i < tr.stationNum - 1; i++) {
        if (tr.stations[i] == ticket.from_station) pos = i;
    }
    if (pos == -1) throw -1;
    // cerr<<"!!"<<pos<<'\n';
    ticket.date -= tr.date[pos];
    if (!ticket_system.buy_ticket(tr, ticket, num, if_wait, result, user_id))
        throw -1;
    order temp_order =
        user_system.add_ticket(user_id, ticket, num, result.status);
    if (temp_order == order()) throw -1;
    if (result.status[0] == 's') {
        train_system.train_tree.erase(ticket.trainID, tr);
        tr.update_seat_res(ticket.from_station, ticket.to_station, ticket.date,
                           num);
       
        train_system.train_tree.insert(ticket.trainID, tr);
    } else if (result.status[0] == 'p') {
        ticket_system.waiting_list.push_back(temp_order);
    }
    // cout << result.status << endl;
}

void System::refund_ticket() {
    char key = input.GetKey();
    String user_id;
    Ticket ticket;
    int num = 1;
    while (key != '\n') {
        if (key == 'u') {
            user_id = input.GetString();
        } else if (key == 'n') {
            num = input.GetInteger();
        }
        key = input.GetKey();
    }
    // --num;
    order refunded_order = user_system.refund_ticket(user_id, num);
    if (refunded_order == order()) throw -1;

    // std::cerr<<"here\n";
    user_system.modify_order(refunded_order, "refunded");
    Train refund_train = train_system.find_train(refunded_order.ticket.trainID);
    train_system.train_tree.erase(refunded_order.ticket.trainID, refund_train);
    refund_train.update_seat_res(
        refunded_order.ticket.from_station, refunded_order.ticket.to_station,
        refunded_order.ticket.date, -refunded_order.num);
    train_system.train_tree.insert(refunded_order.ticket.trainID, refund_train);
    // order result =
    //     ticket_system.refund_ticket(refunded_order.ticket,
    //     refunded_order.num);
    order result;
    // std::cerr<<"waiting_list size:
    // "<<ticket_system.waiting_list.size()<<endl;
    bool isfound = 0;
    while (ticket_system.waiting_list.size() > 0) {
        isfound = 0;
        Train tr_processed;  // 保存需要处理的Train对象
        for (auto it = ticket_system.waiting_list.begin();
             it != ticket_system.waiting_list.end();) {
            Train tr = train_system.find_train(it->ticket.trainID);
            int seat_res =
                tr.get_seat_res(it->ticket.from_station, it->ticket.to_station,
                                it->ticket.date);
            // DEBUG: 检查目标订单的站点索引
            int wait_from = -1, wait_to = -1;
            for (int i = 0; i < tr.stationNum; i++) {
                if (tr.stations[i] == it->ticket.from_station) wait_from = i;
                if (tr.stations[i] == it->ticket.to_station) wait_to = i;
            }
            std::cout << "[DEBUG] Waiting order: from=" << wait_from << " to=" << wait_to
                      << " date=" << it->ticket.date << " num=" << it->num << std::endl;
            if (seat_res >= it->num) {
                // std::cerr<<"found waiting ticket can be processed
                // "<<it->ticket.trainID<<' '<<it->ticket.from_station<<'
                // '<<it->ticket.to_station<<' '<<it->ticket.date<<endl;
                result = *it;
                std::strcpy(result.status, "success");
                ticket_system.waiting_list.erase(it);
                tr_processed = tr;  // 保存这个Train对象
                isfound = 1;
                break;
            } else {
                ++it;
            }
        }
        if (!isfound) break;  // 没有找到可以满足的订单，退出while循环
        // std::cerr<<"after processing waiting list\n";
        if (!(result == order())) {
            // std::cerr<<"processing waiting ticket "<<result.ticket.trainID<<'
            // '<<result.ticket.from_station<<' '<<result.ticket.to_station<<'
            // '<<result.ticket.date<<endl;
            // 关键修复：使用已经查询过的Train对象，而不是重新查询
            train_system.train_tree.erase(tr_processed.ID, tr_processed);
            tr_processed.update_seat_res(result.ticket.from_station,
                                     result.ticket.to_station, result.ticket.date,
                                     result.num);
            train_system.train_tree.insert(tr_processed.ID, tr_processed);
            user_system.modify_order(result, "success");
        }
        if (!isfound) break;
    }
    cout << 0 << endl;
}

void System::query_order() {
    char key = input.GetKey();
    // cerr<<key<<endl;
    String user_id = input.GetString();
    // cerr<<key<<' '<<user_id<<endl;
    if (!user_system.query_ordered_tickets(user_id)) throw -1;
}

void System::clean() {
    user_cnt = 0;
    system_river.write_info(user_cnt, 3);
    train_system.clean_up();
    user_system.clean_up();
    ticket_system.clean_up();
    cout << 0 << endl;
    /*
    TODO: clean all data files & cache
    */
}

}  // namespace sjtu