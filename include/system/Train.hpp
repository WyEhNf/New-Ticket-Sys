// #ifdef _TRAIN_HPP_
// #define _TRAIN_HPP_
#pragma once
#include "../container/String.hpp"
#include "../container/vector.hpp"
#include "../memoryriver/memoryriver.hpp"
using namespace std;
namespace sjtu {
class Train {
   public:
    static constexpr int MAX_STATIONS = 100;
    static constexpr int MAX_DAYS = 92;  // 6/1 to 8/31 inclusive = 92 days
    static constexpr int MAX_VECTOR_SIZE = 100;  // Reduced from 120 to save memory

    char type;
    String ID;
    bool released = false;
    int stationNum, seatNum;
    int startTime, sale_begin, sale_end;
    vector<String> stations;
    int* seat_res;  // Dynamically allocated array: seat_res[MAX_DAYS][MAX_STATIONS-1]
    vector<int> prices, travelTimes, stopoverTimes;
    vector<int> date;
    friend class Ticket;
    friend class TrainSystem;
    friend class UserSystem;
    friend class TicketSystem;
    friend class System;

    Train() : released(false), stationNum(0), seatNum(0), startTime(0), sale_begin(0), sale_end(0), seat_res(nullptr) {
        // Allocate seat_res
        seat_res = new int[MAX_DAYS * (MAX_STATIONS - 1)];
        // Initialize to 0
        for (int i = 0; i < MAX_DAYS * (MAX_STATIONS - 1); i++) {
            seat_res[i] = 0;
        }
    }
    Train(char type, String ID, int stationNum, int seatNum, int startTime,
          int sale_begin, int sale_end, vector<String> stations,
          vector<int> prices, vector<int> travelTimes,
          vector<int> stopoverTimes)
        : type(type),
          ID(ID),
          stationNum(stationNum),
          seatNum(seatNum),
          startTime(startTime),
          sale_begin(sale_begin),
          sale_end(sale_end),
          stations(stations),
          prices(prices),
          travelTimes(travelTimes),
          stopoverTimes(stopoverTimes),
          released(false),
          seat_res(nullptr) {
        // Allocate and initialize seat_res
        seat_res = new int[MAX_DAYS * (MAX_STATIONS - 1)];
        for (int i = 0; i < MAX_DAYS * (MAX_STATIONS - 1); i++) {
            seat_res[i] = 0;
        }
    }
    Train(const Train& other)
        : type(other.type),
          ID(other.ID),
          stationNum(other.stationNum),
          seatNum(other.seatNum),
          startTime(other.startTime),
          sale_begin(other.sale_begin),
          sale_end(other.sale_end),
          stations(other.stations),
          prices(other.prices),
          travelTimes(other.travelTimes),
          stopoverTimes(other.stopoverTimes),
          released(other.released),
          date(other.date) {
        // Allocate and copy seat_res
        seat_res = new int[MAX_DAYS * (MAX_STATIONS - 1)];
        for (int i = 0; i < MAX_DAYS * (MAX_STATIONS - 1); i++) {
            seat_res[i] = other.seat_res[i];
        }
    }
    ~Train() {
        delete[] seat_res;
        seat_res = nullptr;
    }
    Train& operator=(const Train& other) {
        if (this == &other) return *this;
        type = other.type, ID = other.ID, stationNum = other.stationNum,
        seatNum = other.seatNum;
        startTime = other.startTime, sale_begin = other.sale_begin,
        sale_end = other.sale_end;
        stations = other.stations, prices = other.prices,
        travelTimes = other.travelTimes;
        stopoverTimes = other.stopoverTimes;
        released = other.released;
        date = other.date;
        // Copy seat_res
        delete[] seat_res;
        seat_res = new int[MAX_DAYS * (MAX_STATIONS - 1)];
        for (int i = 0; i < MAX_DAYS * (MAX_STATIONS - 1); i++) {
            seat_res[i] = other.seat_res[i];
        }
        return *this;
    }
    void initialise() {
        int time = startTime;
        int base = startTime;
        date.push_back(0);
        for (int i = 1; i < stationNum; i++) {
            time += travelTimes[i - 1];
            if (i != stationNum - 1) time += stopoverTimes[i - 1];
            date.push_back((time / 1440 - base / 1440));
        }
        // for(auto x:date) cout<<x<<' ';
        // cout<<'\n';
    }
    bool operator==(const Train& other) const {
        return ID == other.ID;
    }
    bool operator!=(const Train& other) const {
        return ID != other.ID;
    }
    bool operator<(const Train& other) const {
        return ID < other.ID;
    }
    bool operator<=(const Train& other) const {
        return ID <= other.ID;
    }
    bool operator>(const Train& other) const {
        return !(*this <= other);
    }
    bool operator>=(const Train& other) const {
        return !(*this < other);
    }

    // serialization helpers - FIXED SIZE for B+ tree compatibility
    void serialize(std::ostream &os) const {
        os.write(&type, sizeof(type));
        Serializer<String>::write(os, ID);
        os.write(reinterpret_cast<const char*>(&released), sizeof(released));
        os.write(reinterpret_cast<const char*>(&stationNum), sizeof(stationNum));
        os.write(reinterpret_cast<const char*>(&seatNum), sizeof(seatNum));
        os.write(reinterpret_cast<const char*>(&startTime), sizeof(startTime));
        os.write(reinterpret_cast<const char*>(&sale_begin), sizeof(sale_begin));
        os.write(reinterpret_cast<const char*>(&sale_end), sizeof(sale_end));

        // Write stations as fixed-size array (MAX_VECTOR_SIZE Strings)
        size_t sz = stations.size();
        os.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
        for (size_t i = 0; i < sz; ++i)
            Serializer<String>::write(os, stations[i]);
        // Write padding for remaining slots to ensure fixed size
        for (size_t i = sz; i < MAX_VECTOR_SIZE; ++i) {
            String empty;
            Serializer<String>::write(os, empty);
        }

        // Write seat_res as raw bytes (MAX_DAYS * (MAX_STATIONS - 1) ints)
        int total_size = MAX_DAYS * (MAX_STATIONS - 1);
        os.write(reinterpret_cast<const char*>(seat_res), total_size * sizeof(int));

        // Write prices as fixed-size array (MAX_VECTOR_SIZE ints)
        sz = prices.size();
        os.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
        for (size_t i = 0; i < sz; ++i)
            os.write(reinterpret_cast<const char*>(&prices[i]), sizeof(int));
        // Padding
        for (size_t i = sz; i < MAX_VECTOR_SIZE; ++i) {
            int zero = 0;
            os.write(reinterpret_cast<const char*>(&zero), sizeof(int));
        }

        // Write travelTimes as fixed-size array
        sz = travelTimes.size();
        os.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
        for (size_t i = 0; i < sz; ++i)
            os.write(reinterpret_cast<const char*>(&travelTimes[i]), sizeof(int));
        // Padding
        for (size_t i = sz; i < MAX_VECTOR_SIZE; ++i) {
            int zero = 0;
            os.write(reinterpret_cast<const char*>(&zero), sizeof(int));
        }

        // Write stopoverTimes as fixed-size array
        sz = stopoverTimes.size();
        os.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
        for (size_t i = 0; i < sz; ++i)
            os.write(reinterpret_cast<const char*>(&stopoverTimes[i]), sizeof(int));
        // Padding
        for (size_t i = sz; i < MAX_VECTOR_SIZE; ++i) {
            int zero = 0;
            os.write(reinterpret_cast<const char*>(&zero), sizeof(int));
        }

        // Write date as fixed-size array
        sz = date.size();
        os.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
        for (size_t i = 0; i < sz; ++i)
            os.write(reinterpret_cast<const char*>(&date[i]), sizeof(int));
        // Padding
        for (size_t i = sz; i < MAX_VECTOR_SIZE; ++i) {
            int zero = 0;
            os.write(reinterpret_cast<const char*>(&zero), sizeof(int));
        }
    }

    void deserialize(std::istream &is) {
        is.read(&type, sizeof(type));
        Serializer<String>::read(is, ID);
        is.read(reinterpret_cast<char*>(&released), sizeof(released));
        is.read(reinterpret_cast<char*>(&stationNum), sizeof(stationNum));
        is.read(reinterpret_cast<char*>(&seatNum), sizeof(seatNum));
        is.read(reinterpret_cast<char*>(&startTime), sizeof(startTime));
        is.read(reinterpret_cast<char*>(&sale_begin), sizeof(sale_begin));
        is.read(reinterpret_cast<char*>(&sale_end), sizeof(sale_end));

        // Read stations as fixed-size array
        size_t sz;
        is.read(reinterpret_cast<char*>(&sz), sizeof(sz));
        stations.clear();
        for (size_t i = 0; i < sz; ++i) {
            String s;
            Serializer<String>::read(is, s);
            stations.push_back(s);
        }
        // Skip padding
        for (size_t i = sz; i < MAX_VECTOR_SIZE; ++i) {
            String s;
            Serializer<String>::read(is, s);
        }

        // Read seat_res
        int total_size = MAX_DAYS * (MAX_STATIONS - 1);
        if (seat_res != nullptr) {
            delete[] seat_res;
        }
        seat_res = new int[total_size];
        is.read(reinterpret_cast<char*>(seat_res), total_size * sizeof(int));

        // Read prices as fixed-size array
        is.read(reinterpret_cast<char*>(&sz), sizeof(sz));
        prices.clear();
        for (size_t i = 0; i < sz; ++i) {
            int val;
            is.read(reinterpret_cast<char*>(&val), sizeof(int));
            prices.push_back(val);
        }
        // Skip padding
        for (size_t i = sz; i < MAX_VECTOR_SIZE; ++i) {
            int val;
            is.read(reinterpret_cast<char*>(&val), sizeof(int));
        }

        // Read travelTimes as fixed-size array
        is.read(reinterpret_cast<char*>(&sz), sizeof(sz));
        travelTimes.clear();
        for (size_t i = 0; i < sz; ++i) {
            int val;
            is.read(reinterpret_cast<char*>(&val), sizeof(int));
            travelTimes.push_back(val);
        }
        // Skip padding
        for (size_t i = sz; i < MAX_VECTOR_SIZE; ++i) {
            int val;
            is.read(reinterpret_cast<char*>(&val), sizeof(int));
        }

        // Read stopoverTimes as fixed-size array
        is.read(reinterpret_cast<char*>(&sz), sizeof(sz));
        stopoverTimes.clear();
        for (size_t i = 0; i < sz; ++i) {
            int val;
            is.read(reinterpret_cast<char*>(&val), sizeof(int));
            stopoverTimes.push_back(val);
        }
        // Skip padding
        for (size_t i = sz; i < MAX_VECTOR_SIZE; ++i) {
            int val;
            is.read(reinterpret_cast<char*>(&val), sizeof(int));
        }

        // Read date as fixed-size array
        is.read(reinterpret_cast<char*>(&sz), sizeof(sz));
        date.clear();
        for (size_t i = 0; i < sz; ++i) {
            int val;
            is.read(reinterpret_cast<char*>(&val), sizeof(int));
            date.push_back(val);
        }
        // Skip padding
        for (size_t i = sz; i < MAX_VECTOR_SIZE; ++i) {
            int val;
            is.read(reinterpret_cast<char*>(&val), sizeof(int));
        }
    }
    void release() {
        released = true;
    }
    bool is_released() {
        return released;
    }
    int get_seat_res(String from_station, String to_station, int date) {
        int day_index = date - sale_begin;
        int from, to;
        for (int i = 0; i < stationNum; i++) {
            if (stations[i] == from_station) from = i;
            if (stations[i] == to_station) to = i;
        }
        // std::cerr<<from<<' '<<to<<'\n';
        // std::cerr<<day_index<<'\n';
        // std::cerr << "get_seat_res " << from << ' ' << to << ' ' << day_index << endl;
        int min_seat = seat_res[day_index * (MAX_STATIONS - 1) + from];
        for (int i = from; i < to; i++) {
            if (seat_res[day_index * (MAX_STATIONS - 1) + i] < min_seat)
                min_seat = seat_res[day_index * (MAX_STATIONS - 1) + i];
        }
        return min_seat;
    }
    void update_seat_res(String from_station, String to_station, int date,
                         int num) {
        int day_index = date - sale_begin;
        int from, to;
        for (int i = 0; i < stationNum; i++) {
            if (stations[i] == from_station) from = i;
            if (stations[i] == to_station) to = i;
        }
        for (int i = from; i < to; i++) {
            seat_res[day_index * (MAX_STATIONS - 1) + i] -= num;
        }
    }
    string int_to_date(int date) const {
        if (date <= 30)
            return "06-" + String::FromInt(date);
        else if (date <= 61)
            return "07-" + String::FromInt(date - 30);
        else
            return "08-" + String::FromInt(date - 61);
    }
    string int_to_time(int time) const {
        int hour = time / 60;
        int minute = time % 60;
        return String::FromInt(hour) + ":" + String::FromInt(minute);
    }
    String realTime(int time, int date) const {
        int realdate = date + time / 1440;
        int realtime = time % 1440;
        return int_to_date(realdate) + " " + int_to_time(realtime);
    }
    String getTime(String station, int date,bool isArrive=1) {
        int time = startTime;
        if (stations[0] == station) return realTime(time, date);
        for (int i = 1; i < stationNum; i++) {
            // std::cerr<<"station "<<stations[i]<<endl;
            time += travelTimes[i - 1];
            if (stations[i] == station&&!isArrive) return realTime(time, date);
            if (i != stationNum - 1) time += stopoverTimes[i - 1];
             if (stations[i] == station&&isArrive) return realTime(time, date);
        }
        return String("error!");
    }
};

}  // namespace sjtu
// #endif