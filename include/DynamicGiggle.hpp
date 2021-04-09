#pragma once

#include <functional>
#include <exception>

#ifdef __linux__
#include <dlfcn.h>
#elif _WIN32
#include <windows.h>
#include <libloaderapi.h>
#endif

class DynamicGiggle {
public:
    enum Mode {
#ifdef __linux__
        DELAYED_RESOLVE = RTLD_LAZY,
        RESOLVE_NOW = RTLD_NOW,
        GLOBAL_RESOLVE = RTLD_GLOBAL,
#elif _WIN32
        DELAYED_RESOLVE = 0,
        RESOLVE_NOW = 0,
        GLOBAL_RESOLVE = 0,
#endif
    };

    DynamicGiggle() noexcept;

    DynamicGiggle(const std::string &dynamicLibraryPath, int mode = Mode::DELAYED_RESOLVE);

    void open(const std::string &dynamicLibraryPath, int mode = Mode::DELAYED_RESOLVE);

    void close();

    ~DynamicGiggle() noexcept;

    template <typename T>
    std::function<T> get(const std::string &symbol) const
    {
        auto f = reinterpret_cast<T *>(getSymbol(symbol.c_str()));

        if (f == nullptr) {
            throw std::invalid_argument(getError());
        }
        return std::function<T>(f);
    }

private:
    std::string getError() const noexcept;
#ifdef __linux__
    void *_handle;

    void *getSymbol(const char *symbol) const noexcept;
#elif _WIN32
    HMODULE _handle;

    FARPROC getSymbol(LPCSTR lpProcName) const noexcept;
#endif
};