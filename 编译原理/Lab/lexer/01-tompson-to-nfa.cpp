// Tompson Alg
// By Htto 2023 1-3
// Tiny Regular Expression.

#include <iostream>
#include <algorithm>
#include <vector>
#include <tuple>
#include <map>
#include <set>
#include <queue>

// decls
namespace Alg
{
    using std::tuple;
    using std::vector;

    struct Edge;

    struct Node
    {
    public:
        Edge *head = nullptr;
        bool is_end = false;
        void push_edge(Edge *e);

    public:
        void destroy();
    };

    struct Edge
    {
    public:
        Edge(bool _is_ep, char _val, Node *_dst) : is_epsilon(_is_ep), value(_val), dest(_dst) {}

    public:
        bool is_epsilon;
        char value;
        Edge *next = nullptr;
        Node *dest;
    };

    struct Graph
    {
        Graph(Node *s, Node *e) : start(s), end(e) {}

        Node *start;
        Node *end;

    public:
        // implemented by dfs
        // format: <u,v,char>
        // you can view the graph by : https://csacademy.com/app/graph_editor/
        vector<tuple<int, int, char>> traverse_graph()
        {
            vector<tuple<int, int, char>> ret;
            std::map<Node *, int> tab;
            // ensure bfs visit once.
            std::set<Node *> vis;

            int cnt = 0;

            auto find_node = [&](Node *n)
            {
                auto it = tab.find(n);
                if (it != tab.end())
                    return it->second;
                tab.insert({n, ++cnt});
                return cnt;
            };

            std::queue<Node *> q;
            q.push(start);
            while (!q.empty())
            {
                auto cur = q.front();
                q.pop();
                int u = find_node(cur);
                for (auto i = cur->head; i; i = i->next)
                {
                    ret.push_back({u, find_node(i->dest), i->value});
                    if (!vis.count(i->dest))
                    {
                        q.push(i->dest);
                        vis.insert(i->dest);
                    }
                }
            }
            return ret;
        }

    public:
        static Graph *build_graph_by_char(char ch)
        {
            Node *start = new Node(), *end = new Node();
            Edge *edge = new Edge(false, ch, end);

            start->push_edge(edge);
            return new Graph(start, end);
        }

        // g1,g2 will destory!
        static Graph *concat(Graph *g1, Graph *g2)
        {
            Edge *epsilon_edge = new Edge(true, '*', g2->start);
            g1->end->push_edge(epsilon_edge);
            g1->end->is_end = false;
            Graph *ret = new Graph(g1->start, g2->end);

            delete g1;
            delete g2;
            return ret;
        }
        // g1,g2 will destroy!
        static Graph *cup(Graph *g1, Graph *g2)
        {
            Node *start = new Node();
            Node *end = new Node();

            Edge *ep_edges[4];

            // gen 4 epsilon edges
            for (int i = 0; i < 4; i++)
            {
                ep_edges[i] = new Edge(true, '*', nullptr);
            }
            // link edges
            ep_edges[0]->dest = g1->start;
            ep_edges[1]->dest = g2->start;
            ep_edges[2]->dest = ep_edges[3]->dest = end;

            start->push_edge(ep_edges[0]), start->push_edge(ep_edges[1]);
            g1->end->push_edge(ep_edges[2]), g2->end->push_edge(ep_edges[3]);

            end->is_end = true;
            delete g1;
            delete g2;
            return new Graph(start, end);
        }
        // g will destroy!
        static Graph *repeat(Graph *g, bool enable_zero)
        {
            Node *start = new Node();
            Node *end = new Node();
            end->is_end = true;
            Edge *ep1 = new Edge(true, '*', g->start), *ep2 = new Edge(true, '*', end);
            Edge *back_e = new Edge(true, '*', g->start);

            start->push_edge(ep1), g->end->push_edge(ep2), g->end->push_edge(back_e);

            if (enable_zero)
            {
                delete g;
                return new Graph(start, end);
            }
            Edge *ep3 = new Edge(true, '*', end);
            start->push_edge(ep3);
            g->end->is_end = false;
            delete g;
            return new Graph(start, end);
        }
    };

    struct REParser
    {
    public:
    private:
        char *content;
    };
}
// defs
namespace Alg
{
    void Node::destroy()
    {
        if (!head)
            return;
        for (Edge *cur = head; cur;)
        {
            cur->dest->destroy();
            Edge *tmp = cur;
            cur = cur->next;
            delete tmp;
        }
    }
    void Node::push_edge(Edge *e)
    {
        if (head == nullptr)
            head = e;
        else
        {
            e->next = head->next;
            head->next = e;
        }
    }
}

namespace Test
{
    void test_tompson()
    {
        using namespace Alg;
        Graph *a = Graph::build_graph_by_char('a');
        Graph *b = Graph::build_graph_by_char('b');
        Graph *c = Graph::repeat(Graph::cup(a, b),false);
        // std::cout << "HELLO" << std::endl;
        auto res = c->traverse_graph();
        for (auto [a, b, c] : res)
        {
            std::cout << a << " " << b << " " << c << std::endl;
        }
    }
}

int main()
{
    Test::test_tompson();
    std::cout << "The program is exited!";
    return 0;
}