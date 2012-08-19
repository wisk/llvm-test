cmake_minimum_required(VERSION 2.8)
project(llvm-test)


if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES Debug Release)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE
    STRING
    "Reset the configurations to what we need" FORCE)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")


# TODO: Specify default libs in FindLLVM.cmake ...
find_package(LLVM COMPONENTS X86AsmParser X86Disassembler X86CodeGen SelectionDAG AsmPrinter MCParser X86Desc X86Info X86AsmPrinter X86Utils JIT RuntimeDyld ExecutionEngine CodeGen ScalarOpts InstCombine TransformUtils ipa ANalysis Target MC Object Core Support Interpreter REQUIRED)

include_directories(${LLVM_INCLUDE_DIRS})

add_executable(llvm-test_add llvm-test_add.cpp)
target_link_libraries(llvm-test_add ${LLVM_LIBRARIES})

message(${LLVM_LIBRARIES})

add_executable(llvm-test_struct llvm-test_struct.cpp)
target_link_libraries(llvm-test_struct ${LLVM_LIBRARIES})