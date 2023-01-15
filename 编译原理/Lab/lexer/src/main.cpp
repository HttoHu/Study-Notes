// #include "../includes/subset-cons.h"
#include "../includes/mbitset.h"
#include "../includes/to-nfa.h"

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
    using namespace Alg;
    int cnt = 0;
    time_t s = clock();

    // auto g1 = Graph::build_graph_by_char('a');
    // Graph::repeat(g1,true);
    // g1->traverse_graph();
    // MGraph mg(g1);

    // mg.print();
    for (int i = 1; i <= 1; i++)
    {
        cnt++;
        printf("%d\n", cnt);
        ReParser::Test::case1();
    }
    time_t e = clock();
    std::cout << (double)(e - s) / CLOCKS_PER_SEC;
    return 0;
}