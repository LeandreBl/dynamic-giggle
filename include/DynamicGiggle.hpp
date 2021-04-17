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
    static const std::string SHARED_OBJECT_SUFFIX;
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
    std::function<T> getFunction(const std::string &symbol) const
    {
        return std::function<T>(get<T>(symbol));
    }

    template <typename T>
    T &getGlobal(const std::string &symbol) const
    {
        return get<T>(symbol);
    }

    template <typename T>
    T &get(const std::string &symbol) const
    {
        auto sym = reinterpret_cast<T *>(getSymbol(symbol.c_str()));

        if (sym == nullptr) {
            throw std::invalid_argument(getError());
        }
        return *sym;

    }

    bool isOpen() const noexcept;
    const std::string &getLibraryPath() const noexcept;
    static std::string addLibrarySuffix(const std::string &str) noexcept;
    static void addLibrarySuffix(std::string &str) noexcept;


private:
    std::string _libraryName;
    std::string getError() const noexcept;
#ifdef __linux__
    void *_handle;

    void *getSymbol(const char *symbol) const noexcept;
#elif _WIN32
    HMODULE _handle;

    FARPROC getSymbol(LPCSTR lpProcName) const noexcept;
#endif
};