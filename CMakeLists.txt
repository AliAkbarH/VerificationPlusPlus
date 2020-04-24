cmake_minimum_required(VERSION 2.8)
project(MyASTToolProject)
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_SOURCE_DIR}/cmake/modules"
)

find_package(LibClangTooling REQUIRED)

add_definitions(${LibClangTooling_DEFINITIONS})

include_directories(
  ${LibClangTooling_INCLUDE_DIRS} "/home/ali/FYP/llvm-project-master/clang/include"
)

add_executable(MyASTTool src/MyASTTool.cpp src/TypeTheoryGenerator.h src/TypeTheoryGenerator.cpp src/TypeTheory.h src/TypeTheory.cpp)
target_link_libraries(MyASTTool
  ${LibClangTooling_LIBRARIES}
)
