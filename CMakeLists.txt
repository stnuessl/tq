cmake_minimum_required(VERSION 2.6)

# project name
project(tq)

add_definitions(
    -Wall 
    -Wextra
    -Wpedantic
    -Weffc++
    -O2
    -DNDEBUG
#     -g3
)

set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")

#set(CMAKE_BUILD_TYPE "release")
#set(CMAKE_BUILD_TYPE "debug")

set(CMAKE_INSTALL_PREFIX /usr/local)
set(TARGET_INSTALL_DIR bin)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(CURL REQUIRED)
find_package(JSONCPP REQUIRED)
find_package(Boost
             COMPONENTS program_options filesystem system
             REQUIRED)


# <XPREFIX>_FOUND          ... set to 1 if module(s) exist
# <XPREFIX>_LIBRARIES      ... only the libraries (w/o the '-l')
# <XPREFIX>_LIBRARY_DIRS   ... the paths of the libraries (w/o the '-L')
# <XPREFIX>_LDFLAGS        ... all required linker flags
# <XPREFIX>_LDFLAGS_OTHER  ... all other linker flags
# <XPREFIX>_INCLUDE_DIRS   ... the '-I' preprocessor flags (w/o the '-I')
# <XPREFIX>_CFLAGS         ... all required cflags
# <XPREFIX>_CFLAGS_OTHER   ... the other compiler flags
                    

include_directories(${CURL_INCLUDE_DIRS})
include_directories(${JSONCPP_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})


# link_directories(${CMAKE_LIBRARY_PATH})

add_executable(tq
    src/query/query.cpp
    src/query/url-client.cpp
    src/adapter/query-adapter.cpp
    src/adapter/query-results.cpp
    src/bookmarks.cpp
    src/config.cpp
    src/file.cpp
    src/main.cpp
    src/stream-opener.cpp
)

target_link_libraries(tq ${CURL_LIBRARIES})
target_link_libraries(tq ${JSONCPP_LIBRARIES})
target_link_libraries(tq ${Boost_LIBRARIES})

install(PROGRAMS ${CMAKE_BINARY_DIR}/tq DESTINATION ${TARGET_INSTALL_DIR})
install(PROGRAMS bash-completion/tq 
        DESTINATION /usr/share/bash-completion/completions/)

# uninstall target
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
 