## Perfect Forwarding
假如你想写一个通用的代码，该代码能转发传入参数的基本属性
* 可修改的对象转发后仍然可以修改
* 常量转发后依然只读
* 可以移动的对象（该对象即将销毁）转发后依然是可移动对象

不使用模板的话，需要为这三种情况编写好几个函数(X&,const X&,X&&)，为此C++11引入了特殊的法则来实现完美参数转发(perfect forwarding).大概像这样
```cpp
template<typename T>
void f(T && val){
    g(std::forward<T>(val));
}
```
T&&这类叫做通用引用，和X&&不同
* X&&是一个X类型的右值引用，他只可以绑定一个可移动对象
* T&& 是一个通用引用，它可以绑定到各种对象上，比如可变，不可变(i.e,const)或者可移动对象。

## Special Member Function Templates
成员类模板同样可以用(不是当)作特殊成员函数，包括构造函数。但是这可能导致意想不到的行为。
```cpp
#include <utility>
#include <string>
#include <iostream>

class Person
{
private:
	std::string name;
public:
	//generic constructor for passed initial name:
	template<typename STR>
	explicit Person(STR&& n) :name(std::forward<STR>(n)) {
		std::cout << "TMPL-CONSTR for' " << name << std::endl;
	}

	// copy and move constructor
	Person(const Person& p) :name(p.name)
	{
		std::cout << "COPY-CONSTR Person" << name << std::endl;
	}

	Person(Person&& p) :name(std::move(p.name)) {
		std::cout << "MOVE-CONSTR Person " << name << std::endl;
	}
};
int main() {
	std::string s = "sname";
	Person p1(s); 
	Person p2("tmp");
	Person p3(p2); //ERROR
	return 0;
}
```
根据C++重载法则，一个nonconstant lvalue Person p的成员模板
```cpp
template<typename STR>
Person(STR&&n);
```
匹配优先级高于拷贝构造函数
为了解决这一问题，我们要用enable_if<>.
## Disable Templates with enable_if<>
C++标准库提供一个helper 模板std::enable_if<>在编译时来忽略该函数模板。比如
```cpp
template<typename T>
typename std::enable_if<(sizeof(T)> 4)>::type
foo(){
    
}
```
foo<>()定义在sizeof(T)<=4的时候会被忽略,如果sizeof(T)>4, 函数将会拓展为
```cpp
void foo(){
}
```
这里，std::enable_if<> 是一个类型trait，它计算传入的编译时间表达式并且有一下行为
* 如果该表达式为真，他的类型成员将会产生类型
  * void,如果没有第二个模板参数传入
  * 否则将会是第二个模板参数
* 如果该表达式为假，该成员类型没有被定义，该模板的特性称为SFINAE(substitution failure is not an error), 这将会在第八章介绍到, 作用就是该函数模板将会被忽略。

C++14中我们可以写为std::enable_if_t<...>来替代 enable_if<...>::type
我们可以用using来简化一下enable_if
```cpp
template<typename T>
using EnableIfSizeGreater4=std::enable_if_t<(sizeof<T>4)>;

template<typename T,typename =EnableIfSizeGreater4<T>>
void foo(){

}
```
## Using enable_if<>
下面我们用enable_if来解决我们上面所遇到的问题。
```cpp
template<typename STR,
        typename = std::enable_if_t<std::is_convertible_v<STR,std::string>>>
        Person(STR&&n);
```
这里,std::convertible<FROM,TO>. 看名字就知道是干啥的吧。
_v后缀C++17专属，如果是更低的版本就老老实实写::value.

**Disabling Special Member Function**
注意，通常我们不可以使用enable_if<>来禁用预先定义的拷贝/移动构造函数和赋值operator，成员函数模板不算特殊成员函数有的时候会被忽略掉，比如
```cpp
class C{
    public:
    template<typename T>
    C (const T &){
        std::cout<<"tmpl copy constructor";
    }
/*
    C(const C & )=delete; 
*/
}
C x;
C y{x}; // still uses the predefined copy operator.
```
我们可以用delete来禁用

## Using Concepts to Simplify enable_if<> Expressions
告诉你一个好消息,C++20 已经支持Concept了。
这是一个期盼已久的语言特性*concepts*, 他允许我们为模板制定简单的语法要求。
用个关键字require
```cpp
	template<typename STR>
	requires std::is_convertible_v<STR,std::string>
	explicit Person(STR&& n) :name(std::forward<STR>(n)) {
		std::cout << "TMPL-CONSTR for' " << name << std::endl;
	}
```
我们可以将这个要求作为通用的concept
```cpp
template<typename T>
concept ConvertibleToString=std::is_convertible_v<T,std::string>;
template<typename STR>
requires ConvertibleToString<STR>
Person(STR && n):name(std::forward<STR>(n)){
	...
}
```
同样可以写为
```cpp
template<ConvertibleToString STR>
Person(STR && n):name(std::forward<STR>(n)){
	...
}
```
自己去Google一下吧。