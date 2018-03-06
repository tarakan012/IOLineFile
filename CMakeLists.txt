cmake_minimum_required(VERSION 3.9)
project(TestAuriga)

set(CMAKE_CXX_STANDARD 11)
#set(SOURCES src/test.cpp examples/basic_with_three_tasks.cpp  examples/single_thread_lock_free.cpp examples/benchmark_simple.cpp examples/statistical_key_facts.cpp examples/benchmark_statistical.cpp)
add_executable(TestAuriga main.cpp FunctorDataClass.h Task.h) #${SOURCES})
target_link_libraries(TestAuriga -pthread)