## Variadic Templates
Example:
```cpp
#include <iostream>
void print() {

}

template<typename T, typename... Types>
void print(T firstArg, Types... args)
{
	std::cout << firstArg;
	print(args...);
}
int main() {
	print("Hey ",123);
	return 0;
}
```
这个例子非常清楚了

**Overloading Variadic and Novariadic Templates**
如果两个函数模板只是有没有trailing parameter pack(末尾的参数包？不会翻译)的区别, 没有trailing parameter pack的函数会被优先选用。

**Operator sizeof...**
C++11引入了一种全新的sizeof...运算符.它可以计算参数包中参数类型或者实参的个数。
```cpp
template<typename T, typename... Types>
void print(T firstArg, Types... args)
{
	std::cout << sizeof...(Types) << '\n'; //print number of remaining types
	std::cout << sizeof...(args) << '\n';	// print number of remaining args;
}

int main() {
	print("Hey ",123,2,3,4);
	return 0;
}
// output 4 4
```
注意在使用C++17之前的版本, if是不可以编译期计算的。比如下面的代码
```cpp
template<typename T, typename... Types>
void print(T firstArg, Types... args)
{
	//print something
	if (sizeof...(args) > 0)
	{
        //print something
	}
}

```
只有在C++17中才可以编译。

### Fold Expression
自从C++17，我们可以将双目运算符应用到参数包里面的所有参数。比如
```cpp
template<typename... T>
auto foldSum(T ...s)
{
    return (...+s);
}
```
这个函数返回所有参数的和，如果参数包是空的，会报错。


Fold Expression | Evaluation
-|-
(... op pack)|(((pack1 op pack 2)op pack3)... op packN)
(pack op ...) | (pack1 op (...(packN-1 op packN)))
(init op ... op pack) | (((init op pack1) op pack2) ... op packN)
(pack op ... op init) |(pack1 op(...(packN op init)))

### Application of Variadic Templates
* passing arguments to the construtor of a new heap object owned by a sharedd pointer:
```cpp
auto sp=std::make_shared<std::complex<float>> (4.2,7.7);
```
* Passing arguments to a thread, which is started by the library:
```cpp
std::thread t(foo,42,"hello"); // call foo(42,"hello") in a seperate thread.
```
* Passing arguments to the constructor of a new element pushed into a vector

### Variadic Class Templates and Variadic Expression
**Variadic Expressions**
```cpp
template<typename ... T>
void printDoubled(T const & ... args)
{
    print(args+args...);
}
template<typename ...T>
void AddOne(T const & ... args)
{
    print(args+1 ...); //注意1后面有一个空格，不然就变了1...词法错误
}
```

**Variadic Indices**
```cpp
template<typename C,typename ..Idx>
void printElems (C const & coll,Idx ... idx)
{
    print(coll[idx]...);
}
```
That is, when calling.
```cpp
std::vector<std::string> coll={"good","times","say","bye"};
printElems(coll,2,0,3);
```
我们还可以用nontype templates parameter来替代
```cpp
template<size_t ... Idx,typename C>
void printIdx(C const &coll)
{
    print(coll[Idx]...);
}
printIdx<2,0,3>(coll);
```

**Variadic Class Templates**
也可以有可变参数的类模板, 比如tuple
```cpp
template<typename... Elements>
class Tuple;
Tuple<int,std::string,char> t; //t can hold integer,string AND character.
```
第25章将会详述，另一个例子是variant;
```cpp
template<typename... Type>
class Variant;
Variant<int,std::string,char> v; // t can hold integer,string OR character.
```
26章将会详述.

你可以定义一个类型，用来表示一串索引

```cpp
template<std::size_t ...>
struct Indices{
};
template<typename T,std::size_t ... Idx>
void PrintByIdx(T t,Indices<Idx...>)
{
    print(std::get<Idx>(t)...);
}

std::array<std::string,5> arr={"HEllo","my","new","!","World"};
printByIdx(arr,Indices<0,4,3>());
```

**Variadic Deduction Guides**
Even deduction Guides can be variadic. For example
```cpp
namespace std{
    template<typename T,typename ...U> 
    array(T,U...)
    ->array<
        enable_if_t<
            (is_same_v<T,U>&& ...),T
        >,
        (1+sizeof...(U))
    >;
}
std::array a{42,45,77}; // -> std::array<int,3>
```
这里T将会被推导为元素类型，U...就是后继元素的类型，然后1+sizeof...(U)应该很显然了。
