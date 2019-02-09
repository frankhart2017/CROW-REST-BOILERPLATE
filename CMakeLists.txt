cmake_minimum_required(VERSION 3.7)

project(cpp-rest)

set(CMAKE_CXX_STANDARD 11)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(Threads REQUIRED)
find_package(libmongocxx REQUIRED)

add_executable(cpp-rest main.cpp)
target_include_directories(cpp-rest  PRIVATE ${Boost_INCLUDE_DIRS} ${LIBMONGOCXX_INCLUDE_DIRS})
target_link_libraries(cpp-rest ${Boost_LIBRARIES} Threads::Threads ${LIBMONGOCXX_LIBRARIES})
