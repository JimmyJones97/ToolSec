cmake_minimum_required(VERSION 3.4.1)

message("CMAKE_ANDROID_ARCH_ABI: "${CMAKE_ANDROID_ARCH_ABI})
message(WARNING "readname only support armeabi-v7a")

if(CMAKE_ANDROID_ARCH_ABI MATCHES "^armeabi-v7a")
    set(LIBRARY_NAME "ue4common")
    set(SOURCES 
            src/include/SceneView.cpp
            src/include/MySimulation.cpp
            )

    if (CMAKE_SYSTEM_NAME MATCHES "^Android")
        add_library(${LIBRARY_NAME} STATIC ${SOURCES})
        target_include_directories(${LIBRARY_NAME} PUBLIC src/include)

        target_link_libraries(${LIBRARY_NAME} log)
    endif()
endif()
