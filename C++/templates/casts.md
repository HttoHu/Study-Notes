```cpp
const_cast<T>(arg);
/*
const cast可以移除const变量的const属性，但是改变常量是一个未定义的行为例：
 */
 void *getOpaqueValue() 
 { return const_cast<Type*>(T); 
 }
```
  