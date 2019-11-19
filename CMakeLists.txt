cmake_minimum_required(VERSION 3.15)
project(xerrada)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()


set(CMAKE_CXX_STANDARD 17)

add_executable(branch-prediction branch-prediction.cpp)
add_executable(small-datastructures small-datastructures.cpp)
add_executable(memory-allocations memory-allocations.cpp)
add_executable(xerrada main.cpp ColumnarDatabase.hpp )
add_executable(memory-layout-matrix memory-layout-matrix.cpp)
target_link_libraries(xerrada ${CONAN_LIBS})
target_link_libraries(memory-layout-matrix ${CONAN_LIBS})
target_link_libraries(branch-prediction ${CONAN_LIBS})
target_link_libraries(memory-allocations ${CONAN_LIBS})
target_link_libraries(small-datastructures ${CONAN_LIBS})