#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
struct Production
{
    Production(char n) : name(n) {}
    std::vector<std::vector<Production *>> prods;
    char name;
    bool is_terminal = true;
    bool has_null = false;
};
std::map<char, Production *> grammar;
std::map<char, std::set<char>> FIRST;
std::map<char, std::set<Production *>> FOLLOW;
std::map<char, bool> nullable;
std::vector<std::string> input;
Production *find_info(char ch)
{
    auto finder = grammar.find(ch);
    Production *ret;
    if (finder == grammar.end())
    {
        ret = new Production(ch);
        grammar.insert({ch, ret});
    }
    else
    {
        ret = finder->second;
    }
    return ret;
}
void build_production()
{
    for (auto &a : input)
    {
        int pos = 3;
        Production *new_prod = find_info(a[0]);
        if (a.size() < 3 && a[1] != '-' && a[2] != '>')
            throw std::runtime_error("syntax error!");
        new_prod->is_terminal = false;
        if (a.size() == 3)
        {
            new_prod->has_null = true;
            continue;
        }
        while (pos < a.size() && a[pos] == ' ')
            pos++;
        new_prod->prods.push_back(std::vector<Production *>());
        auto &vec = new_prod->prods.back();
        for (; pos < a.size(); pos++)
        {
            if (a[pos] == ' ')
                while (pos < a.size() && a[pos] == ' ')
                    pos++;
            vec.push_back(find_info(a[pos]));
        }
    }
}
/*
Z->d
Z->X Y Z
Y->
Y->c
X->Y
X->a
#
*/
void print_grammar()
{
    for (auto a : grammar)
    {
        if (a.second->has_null)
        {
            std::cout << "(HAS TERMINAL)";
        }
        if (a.second->is_terminal)
        {
            std::cout << "(TERMINAL:" << a.second->name << ")" << std::endl;
        }
        for (auto b : a.second->prods)
        {
            std::cout << a.second->name << "->";
            for (auto c : b)
            {
                std::cout << c->name << " ";
            }
            std::cout << std::endl;
        }
    }
}
bool null_with_range(const std::vector<Production *> &p, int start, int end)
{
    if(start<end)
        return false;
    for (; start < end; start++)
    {
        if (!p[start]->has_null)
            return false;
    }
    return true;
}
void add(std::set<char> &vec, std::set<char> &vec2)
{
    for (auto t : vec2)
    {
        vec.insert(t);
    }
}
void process()
{
    for (auto a : grammar)
    {
        if (a.second->is_terminal)
        {
            FIRST.insert({a.first, {a.second->name}});
            continue;
        }

        if (a.second->has_null)
        {
            nullable[a.first] = true;
        }
        //对于每一个产生式
        for (auto &b : a.second->prods)
        {
            if (null_with_range(b, 0, b.size() - 1))
            {
                nullable[a.first] = true;
            }
            if(b.size()!=0)
            {
                add(FIRST[a.first],FIRST[b[0]->name]);
            }
            for (int i = 1; i < b.size(); i++)
            {
                
                for (int j = i; j < b.size(); j++)
                {
                    if (null_with_range(b, 0, i - 1))
                    {
                        add(FIRST[a.first], FIRST[b[i]->name]);
                    }
                    /*if (null_with_range(b, i + 1, b.size() - 1))
                        add(FOLLOW[b[i]->name], FOLLOW[a.first]);
                    if (null_with_range(b, i + 1, j - 1))
                        add(FOLLOW[b[i]->name], FIRST[b[j]->name]);*/
                }
            }
        }
    }
}
int main()
{
    while (true)
    {
        std::string tmp_input;
        std::getline(std::cin, tmp_input);
        if (tmp_input == "#")
            break;
        input.push_back(tmp_input);
    }
    build_production();
    std::cout << "\n ===================Grammar============================\n";
    print_grammar();
    std::cout << "\n ===================Process============================\n";
    process();
    std::cout << "FIRST SET:\n";
    for (auto &a : FIRST)
    {
        std::cout << a.first << ":";
        for (auto b : a.second)
        {
            std::cout << b << " ,";
        }
        std::cout << std::endl;
    }
    std::cout << "\nFOLLOW SET:\n";
    for (auto &a : FOLLOW)
    {
        std::cout << a.first << ":";
        for (auto b : a.second)
        {
            std::cout << b->name << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\nnullable:";
    for (auto &a : nullable)
    {
        std::cout << a.first ;
    }
    return 0;
}