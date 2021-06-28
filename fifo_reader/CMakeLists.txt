cmake_minimum_required(VERSION 3.4.1)

message("CMAKE_ANDROID_ARCH_ABI: "${CMAKE_ANDROID_ARCH_ABI})
message(WARNING "fifo_reader only support armeabi-v7a")

if(CMAKE_ANDROID_ARCH_ABI MATCHES "^armeabi-v7a")
  set(LIBRARY_NAME "fifo_reader32")
  set(SOURCES
        src/FIFOReader.cpp
        src/main.cpp
      )
  if (CMAKE_SYSTEM_NAME MATCHES "^Android")
    add_executable(${LIBRARY_NAME} ${SOURCES})
      target_link_libraries(${LIBRARY_NAME} log)
    endif()
endif()
