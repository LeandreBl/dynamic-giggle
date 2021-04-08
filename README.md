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

    auto f = lib.get<size_t(const char *)>("strlen");
    std::cout << f("salut") << std::endl;

    auto f2 = lib.get<char *(const char *, int)>("strchr");
    std::cout << f2("salut", 'l') << std::endl;
    return 0;
}
```