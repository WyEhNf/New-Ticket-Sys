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
    // setlocale(LC_ALL, "zh_CN.UTF-8");
    // freopen("8.in","r",stdin);
    // ios::sync_with_stdio(false);
    // cin.tie(0);
    // cout.tie(0);
    clock_t START=clock();
    // remove_data_file();
    // std::cerr << "Creating system..." << std::endl;
    sjtu::System sys("ticket_system");
    // std::cerr << "Running system..." << std::endl;
    sys.run();
    clock_t END=clock();
    // std::cerr<<"Total Time: "<<(double)(END-START)/CLOCKS_PER_SEC<<'\n';
    return 0;
}