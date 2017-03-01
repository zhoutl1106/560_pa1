cmake_minimum_required(VERSION 3.6)
project(mkfs)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp mkfs.cpp mkfs.h mkfs_file.cpp mkfs_file.h utils.cpp utils.h mkfs_dir.cpp mkfs_dir.h mkfs_net.cpp mkfs_net.h)
add_executable(mkfs ${SOURCE_FILES})
