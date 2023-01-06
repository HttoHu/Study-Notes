## Callables
**Dealing with Member Functions and Additional Arguments**
如果我们要用到一个类的成员函数，我们需要使用std::invoke();
```cpp
#include <utility>
#include <functional>

template<typename Iter,typename Callable,typename ...Args>
void foreach(Iter current,Iter end,Callable op,const Args & ...args)
{
    while(current!=end){
        std::invoke(op,args...,*current);
        ++current;
    }
}
```
这里，除了可调用对象参数，我们还可以接受任意数量的参数。std::invoke()处理一下情况
* 如果可调用对象是一个指向成员的指针，他将第一个额外的参数当作this对象，所有剩余的额外参数将会被当作可调用对象调用的参数。
* 否则，所有额外的参数将会被当作可调用对象的参数。
```cpp
#include <iostream>
#include <vector>
#include <string>

class MyClass{
public:
    void memfunc(int i)const{
        std::cout<<"My Class::memfunc() called for: "<<i<<'\n';
    }
};

int main(){
    std::vector<int> primes={2,3,5,7,11,13,17,19};
    MyClass obj;
    foreach(primes.begin(),primes.end(),&MyClass::memfunc,obj);
}
```

**Wrapping Function Calls**

std::invoke的一大应用是包装一个单独的函数调用（eg. log the calls, measure their duration, or prepare some context such as starting a new thread for them). 现在我们可以借助完美转发可调用对象及其参数来支持移动语义。
```cpp
#include <utility>
#include <functional>
template<typename Callable, typename ... Args>
decltype(auto) call(Callable && op,Args&& ...args)
{
    return std::invoke(std::forward<Callable>(op),std::forward<Args>(args)...);
}
```
如果我们的函数返回值类型是void,我们又要把要返回的值临时存起来
```cpp
decltype(auto) ret={std::invoke...};
```
而我们又不能定义void变量，所以我们可以采取一下策略
* 使用if constexpr
```cpp
#include <utility>
#include <functional>
#include <type_traits>

template<typename Callable, typename ...Args>
decltype(auto) call(Callable&& op, Args&&... args)
{
	if constexpr (std::is_same_v<std::invoke_result_t<Callable, Args...>, void>) 
		std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...);
	else
		return std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...);
}
```
* 创建一个临时类型，定义一个他的变量，然后将函数结束的操作放到析构函数中

## Other Utilities to Implement Generic Liberaries
**Type Traits**

标准库提供了很多type traits来处理或计算类型。具体去看cpp reference吧

**std::addressof()**

如果&被重载了，我们可以使用std::addressof

**std::declval()**

std::declval()用于占位
```cpp
#include <utility>
template<typename T1,typename T2,
    typename RT=std::decay_t<decltype(true?std::declval<T1>():std::declval<T2>())>> RT max(T1 a, T2 b)
{
    return b<a?a:b;
}
```