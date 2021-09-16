cmake_minimum_required (VERSION 3.10)
project (dex-mpc)
set(NAME "dex-mpc")

set (CMAKE_CXX_STANDARD 17)

include(cmake/common.cmake)
include(cmake/source_of_randomness.cmake)
include(cmake/threading.cmake)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(emp-tool REQUIRED)
find_package(emp-ot REQUIRED)
find_package(emp-sh2pc REQUIRED)
find_package(GMP REQUIRED)

set(EMP-TOOL_LIBRARIES
  ${EMP-TOOL_LIBRARY}
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${GMP_LIBRARIES})

add_executable(gen_auction src/circuit_generator.cc)
target_link_libraries(gen_auction
  ${EMP-TOOL_LIBRARY}
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${GMP_LIBRARIES})
target_include_directories(gen_auction PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${EMP-OT_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})

add_executable(sh2pc_auction src/sh2pc_auction.cc)
target_link_libraries(sh2pc_auction ${EMP-TOOL_LIBRARIES})
target_include_directories(sh2pc_auction PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${EMP-OT_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})

add_executable(sh2pc_auction_benchmark src/sh2pc_auction_benchmark.cc)
target_link_libraries(sh2pc_auction_benchmark ${EMP-TOOL_LIBRARIES})
target_include_directories(sh2pc_auction_benchmark PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${EMP-OT_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})

add_executable(sh2pc_noop_benchmark src/sh2pc_noop_benchmark.cc)
target_link_libraries(sh2pc_noop_benchmark ${EMP-TOOL_LIBRARIES})
target_include_directories(sh2pc_noop_benchmark PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${EMP-OT_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})

add_executable(sh2pc_noop_circuit_generator src/sh2pc_noop_circuit_generator.cc)
target_link_libraries(sh2pc_noop_circuit_generator ${EMP-TOOL_LIBRARIES})
target_include_directories(sh2pc_noop_circuit_generator PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${EMP-OT_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})

add_executable(sh2pc_noop_circuit_benchmark src/sh2pc_noop_circuit_benchmark.cc)
target_link_libraries(sh2pc_noop_circuit_benchmark ${EMP-TOOL_LIBRARIES})
target_include_directories(sh2pc_noop_circuit_benchmark PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${EMP-OT_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})

add_executable(ag2pc_auction src/ag2pc_auction.cc)
target_link_libraries(ag2pc_auction ${EMP-TOOL_LIBRARIES})
target_include_directories(ag2pc_auction PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${EMP-OT_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})

add_executable(agmpc_multiatt_circuit_generator src/agmpc_multiatt_circuit_generator.cc)
target_link_libraries(agmpc_multiatt_circuit_generator ${EMP-TOOL_LIBRARIES})
target_include_directories(agmpc_multiatt_circuit_generator PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${EMP-OT_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})

add_executable(agmpc_multiatt_auction src/agmpc_multiatt_auction.cc)
target_link_libraries(agmpc_multiatt_auction ${EMP-TOOL_LIBRARIES})
target_include_directories(agmpc_multiatt_auction PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${EMP-OT_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})

add_executable(agmpc_singleatt_circuit_generator src/agmpc_singleatt_circuit_generator.cc)
target_link_libraries(agmpc_singleatt_circuit_generator ${EMP-TOOL_LIBRARIES})
target_include_directories(agmpc_singleatt_circuit_generator PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${EMP-OT_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})

add_executable(agmpc_singleatt_auction src/agmpc_singleatt_auction.cc)
target_link_libraries(agmpc_singleatt_auction ${EMP-TOOL_LIBRARIES})
target_include_directories(agmpc_singleatt_auction PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${EMP-OT_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})

add_executable(plain_singleatt_auction src/plain_singleatt_auction.cc)
target_link_libraries(plain_singleatt_auction ${EMP-TOOL_LIBRARIES})
target_include_directories(plain_singleatt_auction PUBLIC
  ${EMP-TOOL_INCLUDE_DIR}
  ${EMP-OT_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR})
