## Nontype Class Template Parameters
Nontype Template Parameters 只能是整数，nullptr,引用等等，其他是不行的，比如double就不可以，字符串也不可以。
注意模板参数中有>和<时要加括号。
## Restrictions for Nontype Template Parameters
* 模板参数传入指针或引用时，该对象不能是C风格字符串，临时变量，数据成员或者其他子对象
```cpp
template <char const *name>
class MyClass{
    ...
};
MyClass<"Hello">x ;// Error:string literal "hello" not allowed.
```
* 在C++11之前，对象必须有external linkage 
* C++11中，对象必须有external 或者 internal linkage.
* C++17 如例子。
```cpp
extern char const s03[]="hi"; //external linkage 
char const s11[]="hi";        //internal linkage 
int main(){
    Message<s03>m03;    //OK all version
    Message<s11> m11;   // OK since C++ 11
    static char const s17[]="hi"; // no linkage.
    Message<s17>m17;    // OK since C++ 17
}
```


## Template Parameter Type auto 
```cpp
template <typename T,auto Maxsize>
class Stack
{
    ...
};
```
is_same<T1,T2>::value (在C++17中可以用is_same_v<T1,T2>来代替)判断这两个类型是否相同。
一个例子
```cpp
#include <iostream>
template<auto T>
class Message
{
    public:
        void print(){
            std::cout<<T<<'\n';
        }
};

int main(){
    Message<42>msg1;
    msg1.print();
    static char const s[]="hello";
    Message<s>msg2;
    msg2.print();
}
//Note also that even template <decltype(auto) N >is possible, which allows instantiation of N as a reference

template<decltype(auto) N>
class C{
    ...
};
int i;
C<(i)> x; // N is int&
```