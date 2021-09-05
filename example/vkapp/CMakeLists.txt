
include_directories(${CMAKE_SOURCE_DIR}/vcore)
include_directories(${CMAKE_SOURCE_DIR}/vmalloc)
include_directories(${CMAKE_SOURCE_DIR}/vtask)
include_directories(${CMAKE_SOURCE_DIR}/vcontainer)
include_directories(${CMAKE_SOURCE_DIR}/vecs)
include_directories(${CMAKE_SOURCE_DIR}/vfg)
include_directories(${CMAKE_SOURCE_DIR}/vrender)
include_directories(${CMAKE_SOURCE_DIR}/vrender/vk)
include_directories(${CMAKE_SOURCE_DIR}/vrender/external/glm)

file(GLOB VKAPP_SRC "*.cpp" "*.h" "*.hpp")
aux_source_directory(${CMAKE_SOURCE_DIR}/vrender/vk VRENDER_SRC)

add_executable(vkapp ${VKAPP_SRC} ${VRENDER_SRC})

IF(LINUX)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  target_link_libraries(vkapp PUBLIC Threads::Threads)
ENDIF()


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
include_directories(${CMAKE_SOURCE_DIR}/vrender/external/glfw)
target_link_libraries(vkapp glfw)


IF (NOT CMAKE_VERSION VERSION_LESS 3.7.0)
  find_package(Vulkan) 
  MESSAGE("Find Vulkan")
ENDIF()
IF (NOT Vulkan_FOUND)
  find_library(Vulkan_LIBRARY NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib" REQUIRED)
  IF (Vulkan_LIBRARY)
    set(Vulkan_INCLUDE_DIRS "$ENV{VULKAN_SDK}/include")
    set(Vulkan_FOUND ON)
    MESSAGE("Using Vulkan SDK")
  ENDIF()
ENDIF()
include_directories(${Vulkan_INCLUDE_DIRS})
target_link_libraries(vkapp ${Vulkan_LIBRARIES})

