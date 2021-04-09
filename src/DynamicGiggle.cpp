#include <iostream>

#include "DynamicGiggle.hpp"

static bool endsWith(const std::string &str, const std::string &with)
{
    if (with.size() > str.size()) {
        return false;
    }
    return std::equal(with.rbegin(), with.rend(), str.rbegin());
}

#ifdef __linux__
static int getLibraryHandle(const char *libraryPath, int mode, void **phandle) noexcept
#elif _WIN32
static int getLibraryHandle(LPCSTR libraryPath, DWORD mode, HMODULE *pmodule) noexcept
#endif
{
#ifdef __linux__
    *phandle = ::dlopen(libraryPath, mode);
    if (*phandle == nullptr) {
        return -1;
    }
    return 0;
#elif _WIN32
    *pmodule = LoadLibraryA(libraryPath);
    if (*pmodule == nullptr) {
        return -1;
    }
    return 0;
#endif
}

static std::string nativeGetError() noexcept
{
#ifdef __linux__
    return dlerror();
#elif _WIN32
    DWORD id = ::GetLastError();

    if (id == 0) {
        return "success";
    }
    LPSTR buffer;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);
    std::string msg(buffer, size);
    LocalFree(buffer);
    return msg;
#endif
}

const std::string DynamicGiggle::SHARED_OBJECT_SUFFIX =
#ifdef __linux__
".so"
#elif _WIN32
".dll"
#endif
;

#ifdef __linux__
void *DynamicGiggle::getSymbol(const char *symbol) const noexcept
#elif _WIN32
FARPROC DynamicGiggle::getSymbol(LPCSTR lpProcName) const noexcept
#endif
{
#ifdef __linux
    return dlsym(_handle, symbol);
#elif _WIN32
    return GetProcAddress(_handle, lpProcName);
#endif
}

#ifdef __linux__
static int closeHandle(void *handle) noexcept
#elif _WIN32
static int closeHandle(HMODULE hModule) noexcept
#endif
{
#ifdef __linux__
    return dlclose(handle);
#elif _WIN32
    BOOL status = FreeLibrary(hModule);
    if (status == 0) {
        return -1;
    }
    return 0;
#endif
}

DynamicGiggle::DynamicGiggle() noexcept
    : _handle(nullptr)
{
}

DynamicGiggle::DynamicGiggle(const std::string &dynamicLibraryPath, int mode)
{
    open(dynamicLibraryPath, mode);
}

void DynamicGiggle::open(const std::string &dynamicLibraryPath, int mode)
{
    int res = getLibraryHandle(dynamicLibraryPath.c_str(), mode, &_handle);
    if (res != 0) {
        throw std::invalid_argument(getError());
    }
    _libraryName = dynamicLibraryPath;
}

void DynamicGiggle::close()
{
    if (_handle != nullptr && closeHandle(_handle) != 0) {
        throw std::invalid_argument(getError());
    }
    _handle = nullptr;
    _libraryName.clear();
}

DynamicGiggle::~DynamicGiggle() noexcept
{
    try {
        close();
    }
    catch (const std::invalid_argument &e) {
        std::cerr << "Unhandled close error: " << e.what() << std::endl;
    }
}

std::string DynamicGiggle::getError() const noexcept
{
    return nativeGetError();
}

bool DynamicGiggle::isOpen() const noexcept
{
    return _handle != nullptr;
}

const std::string &DynamicGiggle::getLibraryPath() const noexcept
{
    return _libraryName;
}

std::string DynamicGiggle::addLibrarySuffix(const std::string &str) noexcept
{
    if (!endsWith(str, SHARED_OBJECT_SUFFIX))
        return str + SHARED_OBJECT_SUFFIX;
    return str;
}

void DynamicGiggle::addLibrarySuffix(std::string &str) noexcept
{
    if (!endsWith(str, SHARED_OBJECT_SUFFIX))
        str += SHARED_OBJECT_SUFFIX;
}