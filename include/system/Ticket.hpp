// #ifdef _TICKET_HPP_
// #define _TICKET_HPP_
#pragma once
#include "../container/vector.hpp"
#include "Train.hpp"
#include "TrainSystem.hpp"
#include "../memoryriver/memoryriver.hpp"

using namespace std;
namespace sjtu {


class Ticket {
   private:
    String trainID;
    String from_station, to_station;
    int time=-1,price=-1;
    static TrainSystem* ptr;
    int date;
    friend class User;
    friend class UserSystem;
    friend class TicketSystem;
    friend class System;

   public:
    Ticket() = default;
    Ticket(Train train, String trainID, String from_station, String to_station,
           int date)
        : trainID(trainID),
          from_station(from_station),
          to_station(to_station),
          date(date) {
    }
    Ticket(const Ticket& other)
        : trainID(other.trainID),
          from_station(other.from_station),
          to_station(other.to_station),
          date(other.date) {
    }
    ~Ticket() = default;
    Ticket& operator=(const Ticket& other) {
        if (this == &other) return *this;
        trainID = other.trainID, from_station = other.from_station,
        to_station = other.to_station, date = other.date;
        return *this;
    }
    bool operator==(const Ticket& other) const {
        return trainID == other.trainID && from_station == other.from_station &&
               to_station == other.to_station && date == other.date;
    }
    bool operator!=(const Ticket& other) const {
        return !(*this == other);
    }
    bool operator<(const Ticket& other) const {
        return trainID < other.trainID;
    }
    bool operator<=(const Ticket& other) const {
        return trainID <= other.trainID;
    }
    bool operator>(const Ticket& other) const {
        return !(*this <= other);
    }
    bool operator>=(const Ticket& other) const {
        return !(*this < other);
    }

    int getPrice()  {
        if(price!=-1) return price;
        Train train=ptr->find_train(trainID);
        int from_index = -1, to_index = -1;
        for (int i = 0; i < train.stationNum; i++) {
            if (train.stations[i] == from_station) from_index = i;
            if (train.stations[i] == to_station) to_index = i;
        }
        int total_price = 0;
        for (int i = from_index; i < to_index; i++) {
            total_price += train.prices[i];
        }
        price=total_price;
        return total_price;
    }
    int getTime()  {
        if(time!=-1) return time;
        Train train=ptr->find_train(trainID);
        int from_index = -1, to_index = -1;
        for (int i = 0; i < train.stationNum; i++) {
            if (train.stations[i] == from_station) from_index = i;
            if (train.stations[i] == to_station) to_index = i;
        }
        int total_time = train.startTime;
        for (int i = 1; i < to_index; i++) {
            total_time += train.travelTimes[i - 1];
            if (i <= from_index) total_time += train.stopoverTimes[i - 1];
        }
        time=total_time;
        return total_time;
    }

    void printTicket(String from_station,
                     String to_station,int num=-1) {
        Train tr=ptr->find_train(trainID);
        // if(tr==Train()) 
        // {
        //     cerr<<"error in printTicket: train not found "<<trainID<<'\n';
        //     return;
        // }
        cout << tr.ID << ' ' << from_station << ' '
             << tr.getTime(from_station, date) << " -> "
             << to_station << ' ' << tr.getTime(to_station, date,0)
             << ' ' << getPrice() << ' '
             << (num == -1 ? tr.get_seat_res(from_station, to_station, date) : num) << endl;
    }

    String getID() const {
        return trainID;
    }

    // serialization for Ticket
    void serialize(std::ostream &os) const {
        Serializer<String>::write(os, trainID);
        Serializer<String>::write(os, from_station);
        Serializer<String>::write(os, to_station);
        os.write(reinterpret_cast<const char*>(&date), sizeof(date));
    }
    void deserialize(std::istream &is) {
        Serializer<String>::read(is, trainID);
        Serializer<String>::read(is, from_station);
        Serializer<String>::read(is, to_station);
        is.read(reinterpret_cast<char*>(&date), sizeof(date));
    }
};

class order {
   public:
    static constexpr int MAX_STATUS_LEN = 100;

    Ticket ticket;
    int num;
    char status[MAX_STATUS_LEN];  // Fixed-size status
    String UserID;
    int pos = -1;
    order() : ticket(), num(0) {
        status[0] = '\0';
    }
    order(const Ticket& ticket, int num, String UserID, string status)
        : ticket(ticket), num(num), UserID(UserID) {
        size_t len = status.size() < MAX_STATUS_LEN - 1 ? status.size() : MAX_STATUS_LEN - 1;
        std::memcpy(this->status, status.data(), len);
        this->status[len] = '\0';
    }
    ~order() = default;
    bool operator==(const order& other) const {
        return ticket == other.ticket;
    }
    bool operator!=(const order& other) const {
        return !(*this == other);
    }

    // serialization for order
    void serialize(std::ostream &os) const {
        Serializer<Ticket>::write(os, ticket);
        os.write(reinterpret_cast<const char*>(&num), sizeof(num));
        Serializer<String>::write(os, UserID);
        os.write(status, MAX_STATUS_LEN);
        os.write(reinterpret_cast<const char*>(&pos), sizeof(pos));
    }
    void deserialize(std::istream &is) {
        Serializer<Ticket>::read(is, ticket);
        is.read(reinterpret_cast<char*>(&num), sizeof(num));
        Serializer<String>::read(is, UserID);
        is.read(status, MAX_STATUS_LEN);
        is.read(reinterpret_cast<char*>(&pos), sizeof(pos));
    }
};

}  // namespace sjtu