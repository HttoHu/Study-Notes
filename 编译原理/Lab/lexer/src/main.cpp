// #include "../includes/subset-cons.h"
#include "../includes/mbitset.h"
#include <time.h>
#include <unordered_map>
#include <iostream>

namespace Test
{
    namespace Hopcroft
    {
        void case1();
    }
}
namespace ReParser
{
    namespace Test
    {
        void case1();
    }
}
int main()
{
    int cnt = 0;
    time_t s = clock();
    DS::BitSet bs;
    bs.insert(4);
    bs.insert(7);
    bs.insert(12345);
    std::cout << "NY"[bs.count(12345)] << "\n";
    std::vector<int> ans = bs.to_vector();

    std::unordered_map<DS::BitSet, int, DS::BitSetHash> tab;
    tab.insert({bs, 1});
    std::cout<<tab[bs];
    // for (int i = 1; i <= 10; i++)
    // {
    //     cnt++;
    //     // printf("%d\n", cnt);
    //     ReParser::Test::case1();
    // }
    time_t e = clock();
    std::cout << (double)(e - s) / CLOCKS_PER_SEC;
    return 0;
}