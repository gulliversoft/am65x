#-----------------------------------------------------------------------------------------------------
# The following ugly work arround (next 6 lines) is due to:
# - CMake parses toolchain-files several times
# - In one of the subsequent calls cache variables are not minded
# - To check whether the PROJECT_ROOT-Variable is set the check is just executed on first run
# - If the variable is not set correctly a hint is provided and cmake execution is stopped with error
if(NOT DEFINED ENV{RECURRENT_CALL})
    set(ENV{RECURRENT_CALL} "TRUE")
  if(NOT DEFINED PROJECT_ROOT)
    message(FATAL_ERROR "Error: (linux) Project-Dir(${PROJECT_ROOT}) not specified -> please set -DPROJECT_ROOT:PATH=<Path-to-your-top-level-CMakeLists.txt-Directory>")
  endif()
endif()
#-----------------------------------------------------------------------------------------------------  

SET(CMAKE_SYSTEM_NAME       Linux)
SET(CMAKE_SYSTEM_PROCESSOR  x86_64)
SET(CMAKE_CROSSCOMPILING    TRUE)
SET(CMAKE_GCOV gcov)
#set(CPPCHECK_PATH "/usr/bin/cppcheck" CACHE STRING "Path to cppcheck executable")



