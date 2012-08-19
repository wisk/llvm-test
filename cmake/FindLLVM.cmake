# FindLLVM
# Only compatible with Windows and Visual studio
# (tested on w7sp1amd64 with vs2010)
#
# LLVM_FOUND
# LLVM_INCLUDE_DIRS
# LLVM_LIBRARIES

mark_as_advanced(LLVM_ROOT)
mark_as_advanced(LLVM_BUILD_ROOT)
mark_as_advanced(LLVM_INCLUDE_DIRS)
mark_as_advanced(LLVM_LIBRARIES)
mark_as_advanced(LLVM_LIBRARIES_RELEASE)
mark_as_advanced(LLVM_LIBRARIES_DEBUG)

set(LLVM_FOUND FALSE)

if(NOT WIN32 OR CYGWIN)
  message("Not yet supported")
else (Windows)

if(NOT LLVM_ROOT)
  message("LLVM_ROOT is _NOT_ set")
endif()

if(NOT LLVM_BUILD_ROOT)
  message("LLVM_BUILD_ROOT is _NOT_ set")
endif()

set(LLVM_INCLUDE_DIRS "${LLVM_ROOT}/include" "${LLVM_BUILD_ROOT}/include")

if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  set(LLVM_LIBRARIES_RELEASE "")
  set(LLVM_LIBRARIES_DEBUG   "")

  foreach (COMPONENTS ${LLVM_FIND_COMPONENTS})
    list(APPEND LLVM_LIBRARIES_RELEASE optimized ${LLVM_BUILD_ROOT}/lib/Release/LLVM${COMPONENTS}.lib)
  endforeach()

  foreach (COMPONENTS ${LLVM_FIND_COMPONENTS})
    list(APPEND LLVM_LIBRARIES_DEBUG debug ${LLVM_BUILD_ROOT}/lib/Debug/LLVM${COMPONENTS}.lib)
  endforeach()

endif()

set(LLVM_LIBRARIES ${LLVM_LIBRARIES_RELEASE} ${LLVM_LIBRARIES_DEBUG})

message(${LLVM_INCLUDE_DIRS})
message(${LLVM_LIBRARIES})
message(${LLVM_LIBRARIES_RELEASE})
message(${LLVM_LIBRARIES_DEBUG})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  LLVM DEFAULT_MSG
  LLVM_INCLUDE_DIRS LLVM_LIBRARIES)

endif()
