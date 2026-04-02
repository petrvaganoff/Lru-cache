cmake_minimum_required(VERSION 3.16)

project(caches LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cache main.cpp
    LruCache.hpp
    LfuCache.hpp
)

include(GNUInstallDirs)
install(TARGETS cache
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
