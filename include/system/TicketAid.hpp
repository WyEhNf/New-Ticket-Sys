#include "Train.hpp"
namespace sjtu {

int getPrice(Train& train, String from_station,
                     String to_station)  {
    int from_index = -1, to_index = -1;
    for (int i = 0; i < train.stationNum; i++) {
        if (train.stations[i] == from_station) from_index = i;
        if (train.stations[i] == to_station) to_index = i;
    }
    int total_price = 0;
    for (int i = from_index; i < to_index; i++) {
        total_price += train.prices[i];
    }
    return total_price;
}

int getTime(Train& train, int date, String from_station,
                    String to_station)  {
    int from_index = -1, to_index = -1;
    for (int i = 0; i < train.stationNum; i++) {
        if (train.stations[i] == from_station) from_index = i;
        if (train.stations[i] == to_station) to_index = i;
    }
    int time = train.startTime;
    for (int i = 1; i < to_index; i++) {
        time += train.travelTimes[i - 1];
        if (i <= from_index) time += train.stopoverTimes[i - 1];
    }
    return time;
}

void printTicket(Train& tr, int date, String from_station,
                         String to_station) {
    cout << tr.ID << ' ' << from_station << ' '
         << tr.getTime(from_station,date) << " -> " << to_station
         << ' ' << tr.getTime(to_station,date) << ' '
         << getPrice(tr, from_station, to_station) << ' '
         << tr.get_seat_res(from_station, to_station, date) << endl;
}
}