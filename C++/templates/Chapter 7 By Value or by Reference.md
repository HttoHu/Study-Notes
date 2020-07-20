## Passing by Value
调用拷贝构造函数的代价很高，但是我们有很多方法来避免昂贵的拷贝带价，即使我们是按值传递的。事实上编译器可能会优化掉拷贝操作将其替换为移动语义。
```cpp
template<typename T>
void printV(T arg)
{
    ...
}
```
当参数是一个int时，函数模板实例化为
```cpp
void printV(int arg)
{
    ...
}
```
传入一个string
```cpp
std::string s="hi";
printV(s);

void printV(std::string arg)
{

}
```
传入一个string s，arg将会时s的拷贝。这是一个高代价的操作，但是拷贝构造函数不总是被执行考虑如下代码
```cpp
std::string returnString();
std::string s="hi";
printV(s);                  // copy constructor
printV(std::string ("hi")); // copy usually optimized away(if not,move constructor)
printV(returnString());     // ditto
printV(std::move(s));       //move constructor
```
因此调用一个按值传递printV的实例，只有在传入左值时才会调用高代价的拷贝。

**Passing by Value Decays**
按值传递的还有一个重要属性，按值传递参数时，类型将会decay。raw array将会decay为pointer. const属性和volatile属性将会被移除.
```cpp
std::string const c="hi";
printV(c);  //c decays so that arg has type std::string
printV("hi")    //decays to pointer so that arg has type char const *
int arr[4];
printV(arr) // decays pointer so that arg has type int*
```
## Passing By Reference
现在讨论按引用传递，在所有情况下，按引用传递不会拷贝对象，同样传入的参数不会decays. 但是有的时候不能按引用传递参数，有的时候可以，有的情况下会导致推断出来的模板参数类型带来不少问题。
### Passing by constant Reference.
在传入非临时对象时，为了避免任何(不必要)的拷贝。我们可以将函数模板写成这样
```cpp
template<typename T>
void printR(const T& arg)
{
    ...
}
std::string returnString();
std::string s="hi";
printR(s);  //no copy
printR(std::string("hi")); //ditto
printR(returnString()); //ditto
printR(std::move(s));   //ditto
```
当参数是引用时，他们不会退化，裸数组不会退化为指针。const和volatile属性不会被移除.
```cpp
template<typename T>
void printR(T const &arg)
{
    ...
}
const std::string c="hi";
printR(c);  // T deduced as std::string, arg is const  std::string &
printR("hi");   //T deduced as char[3], arg is char const(&)[3]
int arr[4];
printR(arr); // T deduced as int[4], arg is int const(&)[4]
```

### Passing by Nonconstant Reference
当你像返回参数时，你需要使用非常量引用。考虑如下代码
```cpp
template<typename T>
void outR(T &arg)
{
    ...
}
```
outR不能传入右值，右值引用啥的，传入裸数组也不会退化。你可以可修改元素，例如处理数组元素个数。例如
```cpp
template<typename T>
void outR(T& arg){
    if(std::is_array<T>::value){
        std::cout<<"got array of"<< std::extend<T>::value<<" elems\n";
    }
    ...
}
```
如果你传入了一个常量参数，arg可能会被推断为constant reference, 这意味着可以传递有右值但是模板期盼的是左值。
```cpp
const std::string c="hi";
outR(c);    // OK: T deduced as const std::string 
outR(returnConstString());  // OK
outR(std::move(c)); // OK: T deduced as const std::string
outR("hi"); // OK: T deduced as char const[3]
```
当然在这种情况下，企图改变传入的参数将会导致错误。如果想禁止传递const对象可以
* 使用static_assert
  ```cpp
    template<typename T>
    void outR(T &arg){
        static_assert(!std::is_const<T>::value,"out parameter of foo<T>(T&) is const");
        ...
    }
* 用std::enable_if
    ```cpp
    template<typename T, typename = std::enable_if_t<!std::is_const<T>::value>
    void outR(T& arg){
        ...
    }
    ```
* 用concept
    ```cpp
    template<typename T>
    requires !std::is_const_v<T>
    void outR(T&arg){
        ...
    }
    ```
### Passing by Forwarding Reference
考虑如下代码
```cpp
template<typename T>
void passR(T && arg)    // arg declared as forwarding reference
{
    ...
}
```
你可以传入任何东西来转发引用.和往常一样，按传入引用时不会发生拷贝。当然有些你意想不到的特例（如果你没读过Effective Modern C++）
```cpp
std::string const c="hi";
passR(c);       //OK: T deduced as const std::string &
passR("hi");    //OK: T deduced as char const(&)[3]
int arr[4]; 
passR(arr);     //OK: T deduced as int (&)[4]
```

## Using std::ref() and std::cref()
自从C++11, 你可以让调用者决定在函数模板中按值传递还是按引用传递。如果模板参数声明为按值传递，就可以用std::ref()和std::cref() (定义在了<functional>的头文件中)，以引用的方式传递一个参数.例如
```cpp
template<typename T>
void printT(T arg){
    ...
}

std::string s="hello";
printT(s);              //pass s by value;
printT(std::cref(s));   //pass s "as if by reference"
```
std::cref()并没有改变函数模板内部处理参数的方式，它创建了std::reference_wrapper<>的对象，这个wrapper 几乎只支持一个操作: 隐式的转换为原始的类型，产生原始的对象。所以
```c++
template<typename T>
void printV(T arg)
{
    std::cout<<arg<<'\n';
}
std::string s="hello";
printV(std::cref(s));//ERROR,wrapper不支持<<
```
同样的，也没有各种比较。你可以写一个函数让他传递时隐式的发生转换.
```cpp
template<typename T>
void printV(T arg)
{
    RealPrintSomething(arg);
}
```
## Dealing with String Literals and Raw Array.
数组在传递中可能会退化为指针，这样你就不能分辨参数到底是指针还是数组了。
* 按值传递将会导致数组退化为指针
* 任何形式按引用传递都不会导致数组退化为指针。
```cpp
template<typename T>
void foo( const T&arg1,const T& arg2)
{
    ...
}
foo("hi","guy");    //ERROR
```
这里，foo("hi,"guy")未能成功编译，因为"hi"的类型是const char [3],"guy"的类型是const char [4];
```cpp
template<typename T>
void foo(T arg1,T arg2)
{
    ...
}
foo("hi","guy");    //Ok, but...
```
这并不意味着万事大吉了，编译时的问题变成了运行时问题
```cpp
template<typename T>
void foo(T arg1,T arg2)
{
    if(arg1==arg2){
        ...
    }
}
```
无论如何，有的时候数组退化也是有用的，特别是检查两个对象是否相同或者可以转换为相同类型。一个常见的用法是完美转发，这个时候就要用到std::decay<>()显式的退化。

### Special Implementations for String Literals and Raw Arrays.
你有时必须分别为指针和数组写出不同的实现。这里当然要求一个数组传递时不会退化。
* 你可以声明让函数模板只对数组有效。
    ```cpp
    template<typename T,std::size_t L1,std::size_t L2>
    void foo(T (&arg1)[L1],T (&arg2) [L2])
    {
        T* pa= arg1;
        T* pb= arg2;
        if(copmareArrays(pa,L1,pb,L2)){
            ...
        }
    }
    这里arg1，和arg2必须为相同元素类型的裸数组，支持不同类型的裸数组可能要更多的实现。
    ```
* 可以使用type traits来检测传入的是否是数组。
  ```cpp
  template<typename T,
  typename=std::enable_if_t<std::is_array_v<T>>>
  void foo(T&& arg1,T&&arg2)
  {
      ...
  }
  ```
  由于特殊处理的方式过于复杂，我们最好为他们专门写一个名字。只要字符串是裸数组，我们就要单独为他们考虑。
## Dealing With Return Value.
你可以返回一个值，也可以返回一个引用，当返回引用时可能会导致一些可怕的问题，因为引用的东西超过你的控制。这里有几个Tips
* 返回容器元素
* 允许修改的类成员
* 为链式条用传递一个对象（operator<< for streams and operator= for class objects in general)

我们要确保函数模板返回值是返回的值而不是引用时，就要用remove_reference
```cpp
template<typename T>
typename std::remove_reference_t<T> retV(T p)
{
    return T{...}; // always return by value.
}
```
std::decay<>也能隐式的移除引用.
我们可以让编译器自动推断返回值类型，auto总是会decay
```cpp
template<typename T>
auto retV(T p)
{
    return T{...};
}
```

## Recommended Template Parameter Declarations
* 将参数声明为按值传递
    这个方法会退化string字面值和裸数组，但是它不能保证性能，特别是对象十分大的时候。我们仍然可以用std::cref()或std::ref()来包装参数，但是一定要注意这样做是否有效。
* 将参数声明为按引用传递
    这个方法对大型对象性能友好，特别是传递
    * 将已经存在的对象
    * 临时对象或者移动的对象
    * 以上几种类型的对象按转发传递

### 一般建议
1. 默认按值传递，这比较简单，对字符串常量也可以正常处理。如果要传递大的对象可以用std::ref,std::cref()临时用一用。
2. 如果有足够充分的理由可以按引用传递
   * 如果你要将这个参数输入或输出，使用T &，然后使用concept屏蔽掉const看看上面.
   * 如果使用模板是为了转发参数，就用T&&,然后配合std::forward
   * 考虑使用std::decay和std::common_type<>来处理不同类型的字符串和裸数组
   * 如果性能要求很高参数有比较大就用const T&
3. 如果不知道怎么搞就多多测试.
4. 不要过度泛型化
### 一个std::make_shared()的例子
* 在第一个C++标准,C++98里，make_pair<>()长成这样
```cpp
template<typename T1,typename T2>
pair<T1,T2> make_pair(cont T1& a, const T2&b){
    return pair<T1,T2>(a,b);
}
```
* C++03中，改了
```cpp
template<typename T1,typename T2>
pair<T1,T2> make_pair(T1 a, T2 b){
    return pair<T1,T2>(a,b);
}
```
* C++11中.
```cpp
template<typename T1,typename T2>
constexpr pair<typename decay<T1>::type,typename decay<T2>::type>
make_pair(T1 && a, T2&&b)
{
    return pair<typename decay<T1>::type,typename decay<T2>::type>(forward<T1>(a),forward<T2>(b));
}