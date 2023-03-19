cmake_minimum_required(VERSION 3.0)

project(johnnyscube)

find_package(Vulkan)

add_subdirectory(SDL)

add_subdirectory(src)