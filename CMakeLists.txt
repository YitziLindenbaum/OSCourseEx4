cmake_minimum_required(VERSION 3.15)
project(Ex4)

set(CMAKE_CXX_STANDARD 11)

add_executable(Ex4SimpleTest VirtualMemory.cpp PhysicalMemory.cpp PhysicalMemory.h MemoryConstants.h VirtualMemory.h SimpleTest.cpp)
add_executable(Ex4 VirtualMemory.cpp PhysicalMemory.cpp PhysicalMemory.h MemoryConstants.h VirtualMemory.h)