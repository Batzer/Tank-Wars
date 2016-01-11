# Coding style


```
#!c++
#include "TheCppsHeaderFile.h"

#include <vector>
#include <cstdlib>
#include <string>

#include <ThirdPartyHeader1.h>
#include <ThirdPartyHeader2.h>
#include <ThirdPartyHeader3.h>

#include "OurHeader1.h"
#include "OurHeader2.h"
#include "OurHeader3.h"

void someFreeFuntion() {

}

class MyClass {
public:
    MyClass();
    void someRandomMethod(int foo, int bar);

private:
    double memberVar;
    float otherVar;
};

MyClass::MyClass()
		: memberVar(1.0),
		  otherVar(1.0f) {
	// My code
}

void MyClass::someRandomMethod(int foo, int bar) {
    // Some random code
}
```