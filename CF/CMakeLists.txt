cmake_minimum_required (VERSION 3.10)
project (emp-min)
set(NAME "emp-min")

set (CMAKE_CXX_STANDARD 17)

include(cmake/common.cmake)
include(cmake/source-of-randomness.cmake)
include(cmake/threading.cmake)

find_package(OpenSSL REQUIRED)
find_package(relic REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(emp-tool REQUIRED)
find_package(emp-ot REQUIRED)
find_package(GMP REQUIRED)

set(sources src/main.cc)

set(EMP-TOOL_LIBRARIES
  ${EMP-TOOL_LIBRARY}
  ${RELIC_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${GMP_LIBRARIES})

add_executable(${NAME} ${sources})

target_link_libraries(${NAME} ${EMP-TOOL_LIBRARIES})

target_include_directories(${NAME} PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${RELIC_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})
