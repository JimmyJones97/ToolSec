cmake_minimum_required(VERSION 3.4.1)

message("CMAKE_ANDROID_ARCH_ABI: "${CMAKE_ANDROID_ARCH_ABI})
message(WARNING "readname only support armeabi-v7a")

if(CMAKE_ANDROID_ARCH_ABI MATCHES "^armeabi-v7a")
set(LIBRARY_NAME "readname32")

set(SOURCES
      src/readname.cpp
      src/DumpUtils.cpp
      src/RemoteUtils.cpp
      src/UE4Names.cpp
      src/Tidy_BP_PlayerPawn_C.cpp
    )


if (CMAKE_SYSTEM_NAME MATCHES "^Android")
add_executable(${LIBRARY_NAME} ${SOURCES})
  target_link_libraries(${LIBRARY_NAME} log ue4common)
endif()
endif()
