cmake_minimum_required(VERSION 3.26)
project(fractalCUDA CUDA)

set(CMAKE_CUDA_STANDARD 17)
enable_language(CXX CUDA)
add_executable(fractalCUDA
        main.cpp
        kernel.cu)
find_package(fmt)
find_package(SFML)
target_include_directories(fractalCUDA PRIVATE ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
target_link_libraries(fractalCUDA fmt::fmt)
target_link_libraries(fractalCUDA sfml::sfml)
target_link_libraries(fractalCUDA ${CUDART_LIBRARY}
)

set_target_properties(fractalCUDA PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
