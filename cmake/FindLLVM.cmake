# FindLLVM
# Only compatible with Windows and Visual studio
# (tested on w7sp1amd64 with vs2010) using llvm installer
#
# LLVM_FOUND
# LLVM_INCLUDE_DIRS
# LLVM_LIBRARIES

set(LLVM_FOUND FALSE)

if (NOT WIN32 OR CYGWIN)
  message("Not yet implemented")
else (Windows)

if (NOT LLVM_ROOT)
  set(LLVM_ROOT "$ENV{ProgramFiles}/llvm") # LATER: doesn't work, version not handled
endif()

set(LLVM_INCLUDE_DIRS "${LLVM_ROOT}/include")

set(LLVM_LIBRARIES "")
foreach (COMPONENTS ${LLVM_FIND_COMPONENTS})
  list(APPEND LLVM_LIBRARIES "${LLVM_ROOT}/lib/LLVM${COMPONENTS}.lib")
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  LLVM DEFAULT_MSG
  LLVM_INCLUDE_DIRS LLVM_LIBRARIES)

endif()
