#pragma once
#include "../container/String.hpp"
#include "Ticket.hpp"
#include "../memoryriver/memoryriver.hpp"
using namespace std;
namespace sjtu {
class User {
   private:
    static constexpr int MAX_ORDERS = 30;  // Maximum orders per user

    int privilege;
    String UserName, PassWord, MailAdr;
    String name;
    order bought_tickets[MAX_ORDERS];  // Fixed-size array
    int bought_cnt = 0;  // Current number of orders
    bool logged_in = false;
    friend class UserSystem;
    friend class System;
    friend class Ticket;

   public:
    User() : privilege(-1), bought_cnt(0), logged_in(false) {
    }
    User(int privilege, String UserName, String PassWord, String name,String MailAdr)
        : privilege(privilege),
          PassWord(PassWord),
          UserName(UserName),
          MailAdr(MailAdr),
          name(name){
    }
    User(const User& other)
        : privilege(other.privilege),
          UserName(other.UserName),
          PassWord(other.PassWord),
          name(other.name),
          MailAdr(other.MailAdr),
          bought_cnt(other.bought_cnt),
          logged_in(other.logged_in){
        // Copy orders
        for (int i = 0; i < bought_cnt; i++) {
            bought_tickets[i] = other.bought_tickets[i];
        }
    }
    ~User() {
    }
    User& operator=(const User& other) {
        if (this == &other) return *this;
        privilege = other.privilege;
        UserName = other.UserName, PassWord = other.PassWord;
        MailAdr = other.MailAdr;
        bought_cnt = other.bought_cnt;
        logged_in = other.logged_in;
        name = other.name;
        // Copy orders
        for (int i = 0; i < bought_cnt; i++) {
            bought_tickets[i] = other.bought_tickets[i];
        }
        return *this;
    }
    bool operator==(const User& other) const{
        return UserName == other.UserName;
    }
    bool operator!=(const User& other) const{
        return UserName != other.UserName;
    }
    bool operator<(const User& other) const{
        return UserName < other.UserName;
    }
    bool operator<=(const User& other) const{
        return UserName <= other.UserName;
    }
    bool operator>(const User& other) const{
        return !(*this <= other);
    }
    bool operator>=(const User& other) const{
        return !(*this < other);
    }

    // serialization helpers
    void serialize(std::ostream &os) const {
        os.write(reinterpret_cast<const char*>(&privilege), sizeof(privilege));
        Serializer<String>::write(os, UserName);
        Serializer<String>::write(os, PassWord);
        Serializer<String>::write(os, MailAdr);
        Serializer<String>::write(os, name);
        os.write(reinterpret_cast<const char*>(&bought_cnt), sizeof(bought_cnt));
        // Write fixed-size array
        for (int i = 0; i < MAX_ORDERS; i++) {
            Serializer<order>::write(os, bought_tickets[i]);
        }
        os.write(reinterpret_cast<const char*>(&logged_in), sizeof(logged_in));
    }
    void deserialize(std::istream &is) {
        is.read(reinterpret_cast<char*>(&privilege), sizeof(privilege));
        Serializer<String>::read(is, UserName);
        Serializer<String>::read(is, PassWord);
        Serializer<String>::read(is, MailAdr);
        Serializer<String>::read(is, name);
        is.read(reinterpret_cast<char*>(&bought_cnt), sizeof(bought_cnt));

        // Validate bought_cnt to prevent buffer overflow
        if (!is.good() || bought_cnt < 0 || bought_cnt > MAX_ORDERS) {
            bought_cnt = 0;  // Reset to safe value
            is.clear();  // Clear error state
        }

        // Read fixed-size array
        for (int i = 0; i < MAX_ORDERS; i++) {
            Serializer<order>::read(is, bought_tickets[i]);
        }
        is.read(reinterpret_cast<char*>(&logged_in), sizeof(logged_in));
    }
};

}  // namespace sjtu