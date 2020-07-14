## Keyword typename 
关键字typename是用来阐明该标识符是一个type比如
```cpp
template<typename T>
class MyClass
{
    public:
    ...
    void foo(){
        typename T::SubType* ptr;
    }
};
```
这里，第二个typename 用来表示SubType是一个类型而不是T的一个nontype 成员(比如静态成员).不加typename 不一定是一个错误但是意思不同。
In general, typename has to be used whenever a name that depends on a template parameter is a type.

下面是typename 的一个应用
```cpp
#include <iostream>

//print elements of an STL container
template<typename T>
void printColl(T const &coll)
{
    typename T::const_iterator pos; //iterator to iterate over coll
    typename T::const_iterator end(coll.end()); //end position
    for(pos=coll.begin();pos!=end;++pos)
    {
        std::cout<<*pos<<' ';
    }
    std::cout<<'\n';
}
```
这里，函数参数是一个标准的容器，为了遍历这个容器的所有元素需要使用其迭代器。

## Zero Initialization
```cpp
template<typename T>
void foo(){
    T x; 
}
```
那么问题来了该如何初始化呢。使用大括号, 即使是基本类型也会被初始化（false->bool, 0->int, nullptr->pointers)
```cpp
template<typename T>
class MyCalss
{
private:
    T x; // or T x{};
public:
    MyClass():x{}
    {

    }
}

template<typename T>
void foo(T p=T{}) // T p{}=> ERROR
{
    ...
}
```

## Using this->
For class templates with base classes that depend on template parameters, using a name x by itself is not always equivalent to this->x, even though a member ax is inherited. For example.
```cpp
template<typename T>
class Base{
    public:
    void bar();
};

template<typename T>
class Derived:Base<T>{
    public:
    void foo(){
        bar();   // call external bar() or error
    }
};
```

## Member Templates
Class members can also be templates
```cpp
template<typename T>
class Stack {
private:
	std::deque<T> elems;	//elements
public:
	void push(T const&);
	void pop();
	T const& top()const;
	bool empty() const {
		return elems.empty();
	}

	// assign stack of elements of type T2
	template<typename T2>
	Stack& operator=(Stack<T2> const&);
};
```
operator=的实现大致如下
```cpp
template<typename T>
	template<typename T2>
Stack<T>& Stack<T>::operator=(Stack<T2> const& op2)
{
	Stack<T2> tmp(op2);
	elems.clear();
	while (!tmp.empty()) {
		elems.push_front(tmp.top());
		tmp.pop();
	}
	return *this;
}
```

Member function can also be partially or fully specialized.

**Special Member Function Templates**
Templates member functions can be used wherever special member functions allow copying or moving objects. Note that template constructors or template assignment operators don't replace predefined constructors or assignment operators. In this example, for assignments of stacks of the same type, the default assignment operator is still called.
* It can that a template constructor or assignment operator is a better match than the predefined copy/move constructor or assignment operator, although a template version is provided for initialization of other types only.
* It is not easy to "templify" a copy/move constructor, for example, to be able to constrain its existence. 

### the .template Construct
为了表明<是一个模板参数的开始，我们需要使用.template比如
```cpp
template<unsigned long N>
void printBitset(std::bitset<N>const &bs){
    std::cout<<bs.template to_string<char,std::char_traits<char>,
                                    std::allocator<char>>();
}
```
我们显式的说明了to_string的类型信息。如果没有额外的.template ,编译器不知道<是less-than还是模板参数的开始,（我用VS 2019不加好像也行。。。）

### Generic Lambdas and Member Templates
generic lambda比如
```cpp
[](auto x,auto y){
    return x+y;
}
```
is the shortcut for a default-constructed object of the following class:
```cpp
class SomeCompilerSpecificName{
public:
    SomeCompilerSpecificName();
    template<typename T1,typename T2>
    auto operator()(T1 x,T2 y)const{
        return x+y;
    }
}
```
## 5.6 Variable Templates
```cpp
	template<typename T>
	constexpr T pi{3.1415926};
```
用模板声明一个可变变量
我们可以这样使用
```cpp
	std::cout<< pi<int> <<'\n';
	std::cout< <pi<double> <<'\n';
```
我们甚至可以修改变量
如
```c++
	#include <iostream>
	template<typename T>
	T var{};
	int main()
	{
		var<int> = 12;
		std::cout << var<int>;
		std::cout << var<float>;
		std::cin.get();
		return 0;
	}
```
会显示120
甚至我们可以
	var<>;
但是直接使用var是错误的.
我们还可以用于数组和一些黑魔法
```C++
	template<int N>
	 std::array<int,N> arr{}
	 template<auto N>
	 constexpr decltype(N) dval=N;

	 int main()
	 {
	     std::cout<<dval<'c'> <<'\n';
	     arr<10> [0] =42;
	     for (std::size_t i=0;i<arr[10].size();++i)
	     {
	         std::cout<<arr<10>[i] <<'\n';
	     }
	 }
```
**Variable Templates for Data Members.**

```c++
	template <typename T>
	class MyClass
	{
	public:
	    static constexpr int max=1000;
	}
	template <typename T>
	int myMax=MyClass<T>::max;
  //so that application programmers can just write

	auto i=myMax<std::string>;
	instead of
	auto i=MyClass<std::string>::max;
```

**Type Traits Suffix_v**

Since C++17, the standard library uses the technique of variable templates to define shortcuts for all type traits in the standard liberary that yield a (Boolean) value. For example, to be able to write

	std::is_const_v<T>
instead of

	std::is_const<T>::value;
the standard library defines

	namespace std
	{
		template<typename T> constexpr bool is_const_v=is_const<T>::value;
	}
## 5.7 Template Template Parameters
如果模板参数也是一个模板类，我们需要特地声明元素两次（对于我们在前面编写的stack)如
```cpp
    Stack<int,std::vector<int>> vStack; //integer stack that uses a vector
```
使用模板的模板参数可以简化声明像
```cpp
    Stack<int,std::vector> // integer stack that uses a vector.
```
具体如下:
```cpp
template<typename T, template<typename Elem> class Cont=std::deque>
class Stack{
  private:
    Cont<T> elems;
  public:
    void push(T const&);
    void pop();
    T const& top()const;
    bool empty()const{
        return elems.empty();
    }
    ...
}
```
成员函数如下
```c++
template<typename T, template<typename> class Cont>
void Stack<T,Cont>::push (T const& elem)
{
	elems.push_back(elem); // append copy of passed elem
}
```
**Template Template Argument Matching**

如果你尝试使用新版本的Stack, 你可能会得到一个错误提醒“默认值std::deque和模板模板参数Cont不兼容”.
原因是deque等容器其实有两个模板参数，我们加上就好
```cpp
template<typename T,    template<typename Elem,
			typename Alloc=std::allocator<Elem>> class Cont=std::deque>
class Stack {
private:
	Cont<T> elems;
public:
	bool empty()const {
		return elems.empty();
	}
	...
};
```