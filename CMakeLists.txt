cmake_minimum_required(VERSION 2.8)
project(llvm-test)

include_directories(${CMAKE_SOURCE_DIR})

if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES Debug Release)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE
    STRING
    "Reset the configurations to what we need" FORCE)
endif()

set(LLVM_ROOT $ENV{LLVM_BUILD_ROOT})

if (NOT EXISTS ${LLVM_ROOT}/include/llvm)
  message(FATAL_ERROR "LLVM_ROOT (${LLVM_ROOT}) is not a valid LLVM install")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_ROOT}/share/llvm/cmake")
include(LLVMConfig)

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

message(${LLVM_INCLUDE_DIRS})

llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES jit native)


add_executable(llvm-test_add llvm-test_add.cpp)
target_link_libraries(llvm-test_add ${REQ_LLVM_LIBRARIES})

add_executable(llvm-test_struct llvm-test_struct.cpp)
target_link_libraries(llvm-test_struct ${REQ_LLVM_LIBRARIES})

add_executable(llvm-test_emu-and-jit llvm-test_emu-and-jit.cpp)
target_link_libraries(llvm-test_emu-and-jit ${REQ_LLVM_LIBRARIES})
