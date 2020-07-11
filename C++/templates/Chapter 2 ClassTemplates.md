### 2.4 Friends
```cpp
template<typename T>
class Stack{
    ...
    void printOn(std::ostream &strm)const
    {
        ...
    }
    friend std::ostream& operator<<(std::ostream &strm,Stack<T> const &s){
        s.printOn();
        return strm
    }
};
```

operator<<不属于Stack,他不是一个函数模板，只是有实例化的Stack作为参数。如果我们尝试定义这个友元函数我们有如下的问题.
1. 如果我们要隐式的声明一个新的函数模板，我们就要用一个不同的模板参数例如U.
```cpp
template<typename T>
class Stack{
    ...
    template<typename U>
    friend std::ostream & operator<<(std::ostream &,Stack<T> const &);
};
```
用T来作为模板参数或者省略都是不行的.

2. 我们可以提前在外面声明Stack<T>的输出操作符。但是我们首先要声明Stack<T>
```cpp
template<typename T>
class Stack
template<typename T>
std::ostream &operator<<(std::ostream &,Stack<T> const &);
```
这样我们就可以把这个操作符声明为友元
```cpp
template<typename T>
class Stack{
    ...
    friend std::ostream & operator<< <T>(std::ostream & ,Stack<T> const &);
};
```
函数名operator<<后面有一个\<T>。这样我们就声明了一个特例化的非成员函数作为友元。没有\<T>的话我们就声明了一个新的nontemplate function. 

无论哪一种情况，你可以使用没有定义<<运算符的成员，但只要用Stack调用operator<<就会报错.

### 类模板的特例化
大致长得像这样
```cpp
template<>
class Stack<std::string>{
    ...
};
```
特例化的类中，每一个成员函数都必须定义为一个平常的成员函数.
```cpp
void Stack<std::string>::push(std::string const & elem)
{
    elems.push_back(elem); // append copy of passed elem.
}
```

### 部分特例化(Partial Specialization, 偏特化)
我们在特定情况下实现某些方法，但是模板参数仍然需要用户定义.例如，我么可以为指针实现一个Stack<>
```cpp
#include "stack1.cpp"

// partial specialization of class Stack<> for pointers. 
template<typename T>
class Stack<T*>{
    private:
        std::vector<T*>elems; //elements
    public:
        void push(T*);
        T* pop();
        T* top() const;
        bool empty()const{
            return elems.empty();
        }
};
template<typename T>
void Stack<T*>::push(T* elem)
{
    elemes.push_back(elem); //append copy of passed elem
}

template<typename T>
T* Stack<T*>::pop(){
    assert(!elems.empty());
    T* p=elems.back();
    elems.pop_back();
    return p;
}

template<typename T>
T* Stack<T*>::top()const
{
    assert(!elems.empty());
    return elems.back();
}
```

**多元参数的部分特例化**
如
```cpp
template<typename T1,typename T2>
class MyClass{
    ...
};
```
可以有各种组合比如一个参数是模板参数，一个是实例化的类型
```cpp
template<typename T>
class MyClass<T,int>{
    ...
};
template<typename T>
class MyClass<T,T>{
    ...
};
template<typename T1,typename T2>
class MyClass<T1*,T2*>{
    ....
};
```
有些情况可能会引起歧义比如
```cpp
MyClass<int,int> m;
MyClass<int*,int*>m2;
```

### Type Aliases 类型别名
**alias templates**
```cpp
template<typename T>
using DequeStack=Stack<T,std::deque<T>>;
```
### Templatized Aggregates ### 
in C++17 you can even define deduction guides for aggregate class templates:
```cpp
ValueWithComment(char const *,char const *)->ValueWithComment<std::string>;
ValueWithComment vc2={"Hello","initial value"};
```
std::array<>也是一个聚合类(aggregate class)，模板参数是类型和元素数量。C++17也给他加了几个deduction guides. 见第四章4.4.4