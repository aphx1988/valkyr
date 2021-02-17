cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
cmake_policy(VERSION 2.8)

set(NAME valkyr)
project(${NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
IF(UNIX AND NOT APPLE)
	set(LINUX TRUE)
ENDIF()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

#file(GLOB VCORE_H "vcore/*.h")
#file(GLOB VCONTAINER_H "vcontainer/*.h")
#file(GLOB VMALLOC_H "vmalloc/*.h")
#file(GLOB VTASK_H "vtask/*.h")
#file(GLOB VECS_H "vecs/*.h")
#include_directories(vfg)

#file(GLOB SOURCE *.cpp )

add_subdirectory(build/vtest)


