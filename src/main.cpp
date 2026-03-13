#pragma GCC optimize("Ofast")
#include "../include/system/System.hpp"
using namespace std;
void remove_data_file() {
    std::filesystem::path p0 = "../build/ticket_system_ticket_tree.data";
    std::filesystem::path p1 = "../build/ticket_system_train_tree.data";
    std::filesystem::path p2 = "../build/ticket_system_user_tree.data";
    if(std::filesystem::exists(p0))
        std::filesystem::remove(p0);
    if(std::filesystem::exists(p1))
        std::filesystem::remove(p1);
    if(std::filesystem::exists(p2))
        std::filesystem::remove(p2);
}
int main()
{
    sjtu::System sys("ticket_system");
    sys.run();
    return 0;
}