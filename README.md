# dynamic-giggle
Dynamic library loader for Linux shared objects and Windows dynamic link library (.so and .dll)

```cpp
#include <iostream>

#include "DynamicGiggle.hpp"

int main(int ac, char **av)
{
    DynamicGiggle lib;

    if (ac != 2) {
        std::cerr << "USAGE: " << av[0] << " [DYNAMIC LIBRARY]" << std::endl;
        return 1;
    }
    try {
        lib.open(av[1]);
    }
    catch (const std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // As an example we use a shared library containing a strlen and strchr implementation

    // returns a C like function pointer
    auto f = lib.get<size_t(const char *)>("strlen");
    std::cout << f("salut") << std::endl;

    // returns a C++ std::function
    auto f2 = lib.getFunction<char *(const char *, int)>("strchr");
    std::cout << f2("salut", 'l') << std::endl;

    // returns a global variable by reference
    auto &g = lib.get<size_t>("my_tiny_global");
    std::cout << g << std::endl;
    g++;

    // explicitly returns a global variable reference
    auto &g2 = lib.getGlobal<size_t>("my_tiny_global");
    std::cout << g2 << std::endl;
    g2++;

    // explicitly returns a global variable copy
    auto g3 = lib.getGlobal<size_t>("my_tiny_global");
    // g3 is equal to the initial global value + 2
    std::cout << g3 << std::endl;

    return 0;
}
```