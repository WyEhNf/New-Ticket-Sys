#include "../../include/system/UserSystem.hpp"
#include "../../include/system/TicketSystem.hpp"
using namespace std;
namespace sjtu {

bool UserSystem::add_user(const User& new_user) {
    static int add_cnt = 0;
    add_cnt++;
    // std::cerr << "[DEBUG] add_user #" << add_cnt << ": " << new_user.UserName << std::endl;
    auto res = user_tree.find(new_user.UserName);
    if (res.size() != 0) return false;
    // std::cerr<<"adding user "<<new_user.UserName<<endl;
    // std::cerr << "[DEBUG] Before insert for " << new_user.UserName << std::endl;
    user_tree.insert(new_user.UserName, new_user);
    // std::cerr << "[DEBUG] After insert for " << new_user.UserName << std::endl;
    return true;
}
bool UserSystem::delete_user(String user_id) {
    auto res = user_tree.find(user_id);
    if (res.size() == 0) return false;
    user_tree.erase(res[0].index, res[0].value);
    return true;
}
bool UserSystem::login(String user_id, String password) {
    auto res = user_tree.find(user_id);
    // std::cerr<<"USR FIND "<<res.size()<<endl;
    if (res.size() == 0) return false;
    // std::cerr<<"found user "<<res[0].value.UserName<<' '<<res[0].value.PassWord<<endl;
    User u = res[0].value;
    if (u.PassWord != password) return false;
    if (u.logged_in) return false;
    u.logged_in = true;
    user_tree.update(u.UserName, u, u);
    return true;
}
bool UserSystem::logout(String user_id) {
    auto res = user_tree.find(user_id);
    if (res.size() == 0) return false;
    User u = res[0].value;
    if (!u.logged_in) return false;
    u.logged_in = false;
    user_tree.update(u.UserName, u, u);
    return true;
}
User UserSystem::find_user(String user_id) {
    auto res = user_tree.find(user_id);
    if (res.size() == 0) return User();
    User u = res[0].value;
    return u;
}
User UserSystem::modify_user(String user_id, const User& new_user) {
    auto res = user_tree.find(user_id);
    if (res.size() == 0) return User();
    user_tree.update(user_id, res[0].value, new_user);
    return new_user;
}
bool UserSystem::query_ordered_tickets(const String& user_id) {
    auto res = user_tree.find(user_id);
    if (res.size() == 0) return false;
    User u = res[0].value;
    if(!u.logged_in) return false;
    cout<<u.bought_cnt<<endl;
    for (int i = u.bought_cnt-1; i >= 0; i--) {
        order t = u.bought_tickets[i];
        cout<<"["<<t.status<<"] ";
        t.ticket.printTicket(t.ticket.from_station, t.ticket.to_station,t.num);
      /*
      TODO: print order information
      */
    }
    return true;
}
order UserSystem::add_ticket(String user_id, const Ticket& ticket,int num,string status) {
    auto res = user_tree.find(user_id);
    if (res.size() == 0) return order();
    User u = res[0].value;
    if (u.bought_cnt >= User::MAX_ORDERS) return order();  // Exceeds limit
    order new_order{ticket, num, user_id, status};
    u.bought_tickets[u.bought_cnt] = new_order;
    u.bought_tickets[u.bought_cnt].pos = u.bought_cnt;
    u.bought_cnt++;
    user_tree.update(u.UserName, res[0].value, u);
    return u.bought_tickets[u.bought_cnt-1];
}
order UserSystem::refund_ticket(String user_id, int pos) {
    auto res = user_tree.find(user_id);
    // std::cerr<<"refund find "<<res.size()<<endl;
    if (res.size() == 0) return order();
    // std::cout<<"reason1"<<endl;
    User u = res[0].value;
    if(!u.logged_in) return order();
    // std::cout<<"reason2"<<endl;
    if (pos < 1 || pos > u.bought_cnt) return order();
    // std::cout<<"reason3"<<endl;
    int idx = u.bought_cnt - pos;
    order target = u.bought_tickets[idx];
    std::strcpy(u.bought_tickets[idx].status, "refunded");
    user_tree.update(u.UserName, res[0].value, u);
    return target;
}
void UserSystem::modify_order(order &o,string new_sta){
    auto res = user_tree.find(o.UserID);
    if (res.size() == 0) return;
    // std::cerr<<o.pos<<endl;
    User u = res[0].value;
    std::strcpy(u.bought_tickets[o.pos].status, new_sta.c_str());
    user_tree.update(o.UserID, res[0].value, u);
}
void UserSystem::clean_up() {
    user_tree.clean_up();
}
void UserSystem::flush_all() {
    user_tree.flush_all();
}
} // namespace sjtu