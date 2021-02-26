# Martin Shishkov 2021
#part of the am65x library
##########################
cmake_minimum_required (VERSION 3.13)


EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
message( STATUS "Architecture: ${ARCHITECTURE}" )

add_subdirectory(PCA9685servo)
add_subdirectory(TSL2581)



