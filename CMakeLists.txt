cmake_minimum_required(VERSION 3.13)

project(codecrafters-git)

file(GLOB_RECURSE SOURCE_FILES src/*.c)

set(CMAKE_C_STANDARD 23) # Enable the C23 standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(git ${SOURCE_FILES})
target_include_directories(git PUBLIC inc)

target_link_libraries(git PRIVATE OpenSSL::Crypto)
target_link_libraries(git PRIVATE ZLIB::ZLIB)
