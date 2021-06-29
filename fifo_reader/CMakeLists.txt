cmake_minimum_required(VERSION 3.4.1)

message(STATUS "ANDROID_ABI: " ${ANDROID_ABI})
message(STATUS "fifo_reader only support armeabi-v7a")

if(ANDROID_ABI MATCHES "^armeabi-v7a")
  set(LIBRARY_NAME "fifo_reader32")

  message(STATUS "CMAKE_CURRENT_SOURCE_DIR:" ${CMAKE_CURRENT_SOURCE_DIR})
  if(NOT DEFINED PROTO_EXECUTABLE)
      message(ERROR "PROTOC_EXECUTABLE not defined")
  endif(NOT DEFINED PROTO_EXECUTABLE)
  message(STATUS "PROTO_EXECUTABLE:" ${PROTO_EXECUTABLE})
  
  set(PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/proto)
  set(CPP_OUT ${CMAKE_CURRENT_SOURCE_DIR}/src/proto)
  message(STATUS "PROTO_PATH:" ${PROTO_PATH})
  message(STATUS "CPP_OUT:" ${CPP_OUT})
  file(GLOB PROTOBUF_FILES
      src/proto/*.proto
  )
  set(PROTO_CPP_SOURCES)
  FOREACH(FIL ${PROTOBUF_FILES})
      get_filename_component(FIL_WE ${FIL} NAME_WE)
      
      message(STATUS "proto file, name:" ${FIL_WE} ", path:" ${FIL})
      EXECUTE_PROCESS(
          COMMAND ${PROTO_EXECUTABLE} --proto_path=${PROTO_PATH} --cpp_out=${CPP_OUT} ${FIL}
      )
      set(PROTO_CPP_SRCS ${PROTO_CPP_SRCS} "${CPP_OUT}/${FIL_WE}.pb.cc")
      set(PROTO_CPP_HDRS ${PROTO_CPP_HDRS} "${CPP_OUT}/${FIL_WE}.pb.h")
  ENDFOREACH()
  ## find_library找不到，FUCK!!!
  # find_library(PROTOBUF_STATIC_LIBRARY NAMES libprotobuf.a PATHS D:/Download/tmp/_build_armeabi-v7a/built NO_DEFAULT_PATH)
  # if(PROTOBUF_STATIC_LIBRARY_NOTFOUND)
  #   message(FATAL_ERROR  "PROTOBUF_STATIC_LIBRARY not found")
  # endif()
  set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty)
  message(STATUS "THIRD_PARTY_DIR=" ${THIRD_PARTY_DIR})
  set(PROTOBUF_STATIC_LIBRARY ${THIRD_PARTY_DIR}/protobuf/${ANDROID_ABI}/libprotobuf.a)
  message(STATUS "PROTOBUF_STATIC_LIBRARY=" ${PROTOBUF_STATIC_LIBRARY})

  set(SOURCES
        src/FIFOReader.cpp
        src/main.cpp
        ${PROTO_CPP_SRCS}
        ${PROTO_CPP_HDRS}
      )
  if (CMAKE_SYSTEM_NAME MATCHES "^Android")
    add_executable(${LIBRARY_NAME} ${SOURCES})
    target_include_directories(${LIBRARY_NAME} PRIVATE ${THIRD_PARTY_DIR}/protobuf/include)
    target_link_libraries(${LIBRARY_NAME} log ${PROTOBUF_STATIC_LIBRARY})
  endif()
endif()
