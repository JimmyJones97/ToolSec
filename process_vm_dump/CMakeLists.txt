cmake_minimum_required(VERSION 3.4.1)

message("process_vm_dump only support arm64-v8a")
if(CMAKE_ANDROID_ARCH_ABI MATCHES "^arm64-v8a")

set(LIBRARY_NAME "process_vm_dump")
set(SOURCES
      src/pdump.cpp
    )

if (CMAKE_SYSTEM_NAME MATCHES "^Android")
add_executable(${LIBRARY_NAME} ${SOURCES})
  target_link_libraries(${LIBRARY_NAME} log)
endif()

endif()

