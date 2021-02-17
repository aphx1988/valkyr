file(GLOB VKAPP_SRC "*.cpp" "*.h") 

add_executable(vkapp ${VKAPP_SRC})

include_directories(external/glm)
include_directories(external/glfw)

IF(LINUX)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  target_link_libraries(vtest PUBLIC Threads::Threads)
ENDIF()

IF (NOT CMAKE_VERSION VERSION_LESS 3.7.0)
  find_package(Vulkan)
ENDIF()

IF (NOT Vulkan_FOUND)
	find_library(Vulkan_LIBRARY NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib" REQUIRED)
	IF (Vulkan_LIBRARY)
    set(Vulkan_INCLUDE_DIRS "$ENV{VULKAN_SDK}/include")
		set(Vulkan_FOUND ON)
    MESSAGE("Using Vulkan SDK")
	ENDIF()
ENDIF()

