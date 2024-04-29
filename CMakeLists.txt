cmake_minimum_required(VERSION 3.28)
project(AMSL)

set(CMAKE_CXX_STANDARD 23)

include(AMSL.cmake)

add_amsl_target(helloworld examples/helloworld.amsl)
