file(GLOB VTEST_SRC "*.cpp" "*.h" "*.hpp")
file(GLOB VTEST_H "*.h" "*.hpp")

add_executable(vtest ${VTEST_SRC})

IF(LINUX)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  target_link_libraries(vtest PUBLIC Threads::Threads)
ENDIF()

