# Coding style


```
#!c++
#include "OurHeader.h"
#include <OtherHeader.h>
#include <stdlib>

void someFreeFuntion() {

}

class MyClass {
public:
    MyClass();
    void someRandomMethod(int foo, int bar);

private:
    double _memberVar; // Note the _
    float _otherVar;
};

// No initializer list:
MyClass::MyClass() {

}

// With initializer list:
MyClass::MyClass()
    : _memberVar(1.0)
    , _otherVar(1.0f)
{
}

void MyClass::someRandomMethod(int foo, int bar) {
    // Some random code
}
```