#include "Ticket.hpp"
#include "../container/bpt.hpp"
#include "TrainSystem.hpp"
using namespace std;
namespace sjtu {
    enum CompareType {TIME, PRICE};
    class TicketSystem {
    private:
        struct TicketKey{
            int date;
            String from_station;
            bool operator == (const TicketKey& o) const {
                return date == o.date && from_station == o.from_station;
            }
            bool operator != (const TicketKey& o) const {
                return !(*this == o);
            }
            bool operator < (const TicketKey& o) const {
                if (date != o.date) return date < o.date;
                return from_station < o.from_station;
            }
            bool operator > (const TicketKey& o) const {
                return o < *this;
            }
            bool operator <= (const TicketKey& o) const {
                return !(o < *this);
            }
            bool operator >= (const TicketKey& o) const {
                return !(*this < o);
            }
            
            // serialization for TicketKey
            void serialize(std::ostream &os) const {
                os.write(reinterpret_cast<const char*>(&date), sizeof(date));
                sjtu::Serializer<String>::write(os, from_station);
            }
            void deserialize(std::istream &is) {
                is.read(reinterpret_cast<char*>(&date), sizeof(date));
                sjtu::Serializer<String>::read(is, from_station);
            }
        };
        BPlusTree<TicketKey, Ticket> ticket_tree;
        vector<order> waiting_list;
        TrainSystem* train_system_ptr;
        friend class System;

    public:
        TicketSystem(string filename) : ticket_tree(filename) {}
        ~TicketSystem(){}
        bool add_ticket(const Train& train);
        bool query_ticket(const String& from_station,
                          const String& to_station, int date, CompareType cmp_type);
        bool query_transfer_ticket(const String& from_station,
                                   const String& to_station, int date,CompareType cmp_type);
        bool buy_ticket(Train &tr,Ticket& ticket, int num,bool if_wait,order &result, const String& UserID);
         order refund_ticket(const Ticket& ticket, int num);

        static bool Compare_with_cost(BPlusTree<TicketKey, Ticket>::Key &A, BPlusTree<TicketKey, Ticket>::Key &B);
        static bool Compare_with_time(BPlusTree<TicketKey, Ticket>::Key &A, BPlusTree<TicketKey, Ticket>::Key &B);
        void printTicket(Ticket t, String from_station, String to_station);
        void clean_up();
    };
}