cmake_minimum_required(VERSION 3.10.2)

project(demo-record LANGUAGES C CXX)



#移除多余的二进制文件
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -s -std=c++11 -fexceptions -fpermissive -fpic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -s  -fexceptions -fpermissive -fpic")

include_directories(${CMAKE_SOURCE_DIR}/util)
add_library( # Specifies the name of the library.
        demo-record
        SHARED
        demo_record_callback.cpp )


#add_library( # Specifies the name of the library.
#        demo-record-callback
#        SHARED
#        demo_record.cpp )




target_link_libraries(demo-record log android aaudio  )
