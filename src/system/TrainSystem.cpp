#include "../../include/system/TrainSystem.hpp"
using namespace std;
namespace sjtu {
bool TrainSystem::add_train(const Train& new_train) {
    auto res = train_tree.find(new_train.ID);
    if (res.size() != 0) return false;
    train_tree.insert(new_train.ID, new_train);
    return true;
}
bool TrainSystem::delete_train(const String& train_id) {
    auto res = train_tree.find(train_id);
    if (res.size() == 0) return false;
    if(res[0].value.released) return false;
    train_tree.erase(res[0].index, res[0].value);
    return true;
}
Train TrainSystem::find_train(const String& train_id) {
    auto res = train_tree.find(train_id);
    if (res.size() == 0) return Train();
    Train t = res[0].value;
    return t;
}
bool TrainSystem::release_train(const String& train_id) {
    auto res = train_tree.find(train_id);
    if (res.size() == 0) return false;
    Train t = res[0].value;
    if (t.released) return false;
    t.released = true;
    // Initialize seat_res with seatNum for each day and segment
    for(int day = 0; day < t.sale_end - t.sale_begin + 1; day++) {
        for(int seg = 0; seg < t.stationNum - 1; seg++) {
            t.seat_res[day * (Train::MAX_STATIONS - 1) + seg] = t.seatNum;
        }
    }
    // Initialize date vector for ticket generation
    t.initialise();
    train_tree.update(t.ID, res[0].value, t);
    return true;
}
void TrainSystem::clean_up() {
    train_tree.clean_up();
}
void TrainSystem::flush_all() {
    train_tree.flush_all();
}
// namespace sjtu
}