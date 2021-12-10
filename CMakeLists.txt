cmake_minimum_required(VERSION 3.17)
project(OOP_Exercise_2)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(Google_tests)

add_executable(
        OOP_Exercise_2
        main.cpp
        HashTable.cpp
        HashTable.h
        HashTableKeyNotFoundException.cpp
        HashTableKeyNotFoundException.h
)
