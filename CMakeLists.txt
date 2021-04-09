cmake_minimum_required(VERSION 3.9)

project("dynamic-giggle" VERSION 0.0.1 DESCRIPTION "dynamic runtime symbol loader")

set(CMAKE_CXX_STANDARD 17)


## Sets the library name
set(LIB_NAME "dynamic-giggle")
if (UNIX)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ggdb3 -Wall -Wextra -Weffc++ -Wshadow -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wsign-conversion -Wlogical-op -Wuseless-cast")
    set(CMAKE_CXX_FLAGS_DEBUG "-O3 -ggdb3 -Wall -Wextra -Wpedantic")
endif (UNIX)

## Sources
file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
file(GLOB_RECURSE LIB_HEADERS "include/*.hpp")

add_library(${LIB_NAME} SHARED ${LIB_SOURCES} ${LIB_HEADERS})
add_library(${LIB_NAME}-static STATIC ${LIB_SOURCES} ${LIB_HEADERS})
set_target_properties(${LIB_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${LIB_NAME}-static PROPERTIES VERSION ${PROJECT_VERSION})
if (UNIX)
    target_link_libraries(${LIB_NAME} dl)
    target_link_libraries(${LIB_NAME}-static dl)
endif (UNIX)
target_include_directories(${LIB_NAME} PRIVATE "./include")
target_include_directories(${LIB_NAME}-static PRIVATE "./include")