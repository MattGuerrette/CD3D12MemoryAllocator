cmake_minimum_required(VERSION 3.14)
project(CD3D12MemoryAllocator)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

add_library(${PROJECT_NAME} src/CD3D12MemoryAllocator.cpp src/CD3D12MemoryAllocator.h D3D12MemoryAllocator/src/D3D12MemAlloc.h D3D12MemoryAllocator/src/D3D12MemAlloc.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE D3D12MemoryAllocator/src PUBLIC src)