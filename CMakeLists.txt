cmake_minimum_required(VERSION 3.16)
project(IntegralCalculating)

set(CMAKE_CXX_STANDARD 14)


FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()
find_package(Threads REQUIRED)

add_executable(IntegralCalculating src/main.cpp include/configuration_t.h src/configuration_t.cpp src/func_to_integrate.cpp include/func_to_integrate.h)
target_link_libraries(${PROJECT_NAME} Threads::Threads)