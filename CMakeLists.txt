# CMakeList.txt : CMake project for assembler, include source and define
# project specific logic here.
 
cmake_minimum_required (VERSION 3.8)
project ("assembler" VERSION 1.0.0)
 
message("-- Compiler version : " ${CMAKE_CXX_COMPILER_VERSION} " | Compiler id : " ${CMAKE_CXX_COMPILER_ID})
 
# Include libraries.
#add_subdirectory()
 
# Add source to this project's executable.
add_executable (assembler
	src/main.cpp
 )
 
# Point to include directory
target_include_directories(assembler PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/include"
)
 
# Link libraries
#target_link_libraries (assembler PUBLIC
#)
