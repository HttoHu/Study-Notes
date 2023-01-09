#include "../includes/subset-cons.h"

int main()
{
    using namespace Alg;
    using namespace std;
    Graph *g1 = Graph::build_graph_by_char('a');
    Graph *g2 = Graph::build_graph_by_char('b');
    Graph *g3 = Graph::cup(g1, g2);
    Graph *g4 = Graph::repeat(g3, false);
    Graph *g5 = Graph::build_graph_by_char('c');
    Graph *g6 = Graph::cup(g4, g5);
    // (a|b)*|c
    SubsetAlg sa(g6);
    sa.gen_state_tab().print_tab();

    return 0;
}