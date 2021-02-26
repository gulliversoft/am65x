# Martin Shishkov 2021
#part of the am65x library
##########################
cmake_minimum_required (VERSION 3.13)

project ("PCA9685servo" VERSION "1.1" LANGUAGES CXX)
message( STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}" )

list(APPEND I2C_LIBRARY "${CMAKE_SOURCE_DIR}/dependency/${ARCHITECTURE}/libi2c.a")

if( ${ARCHITECTURE} MATCHES "armv61")
    set(TARGET_DEPENDENCY "${CMAKE_SOURCE_DIR}/dependency/${ARCHITECTURE}/")
else()
    set(TARGET_DEPENDENCY "src/")
endif()

add_executable(${PROJECT_NAME})


target_sources(${PROJECT_NAME}
	PRIVATE
		"src/main.cpp"
		"${TARGET_DEPENDENCY}servo.cpp"
		"../am65x.c"
		"src/DEV_Config.c"
		"../i2cfunc.c"
		"src/PCA9685.c"
)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD		14
		CXX_STANDARD_REQUIRED	ON
		CXX_EXTENSIONS		OFF
)

target_compile_options (${PROJECT_NAME}
PRIVATE
$<$<CXX_COMPILER_ID:GNU>:-Wall>
$<$<CXX_COMPILER_ID:GNU>:-Wextra>
$<$<CXX_COMPILER_ID:GNU>:-Wpedantic>
$<$<CXX_COMPILER_ID:GNU>:-pedantic-errors>
$<$<CXX_COMPILER_ID:GNU>:-Wsign-conversion>
$<$<CXX_COMPILER_ID:GNU>:-Wshadow>
$<$<CXX_COMPILER_ID:GNU>:-Wcast-qual>
$<$<CXX_COMPILER_ID:GNU>:-Wcast-align>
$<$<CXX_COMPILER_ID:GNU>:-Wmissing-declarations>
$<$<CXX_COMPILER_ID:GNU>:-Wwrite-strings>
$<$<CXX_COMPILER_ID:GNU>:-fmessage-length=0>
$<$<CXX_COMPILER_ID:GNU>:-fexceptions>
$<$<CXX_COMPILER_ID:GNU>:-Weffc++>
)

target_compile_features (${PROJECT_NAME}
PRIVATE
$<$<CXX_COMPILER_ID:GNU>:cxx_constexpr>
$<$<CXX_COMPILER_ID:GNU>:cxx_final>
$<$<CXX_COMPILER_ID:GNU>:cxx_overrid>e
$<$<CXX_COMPILER_ID:GNU>:cxx_nullptr>
$<$<CXX_COMPILER_ID:GNU>:cxx_alignas>
$<$<CXX_COMPILER_ID:GNU>:cxx_auto_type>
$<$<CXX_COMPILER_ID:GNU>:cxx_range_for>
$<$<CXX_COMPILER_ID:GNU>:cxx_attributes>
$<$<CXX_COMPILER_ID:GNU>:cxx_strong_enums>
$<$<CXX_COMPILER_ID:GNU>:cxx_user_literals>
$<$<CXX_COMPILER_ID:GNU>:cxx_inline_namespaces>
$<$<CXX_COMPILER_ID:GNU>:cxx_deleted_functions>
$<$<CXX_COMPILER_ID:GNU>:cxx_defaulted_functions>
$<$<CXX_COMPILER_ID:GNU>:cxx_delegating_constructors>
$<$<CXX_COMPILER_ID:GNU>:cxx_generalized_initializers>
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		${I2C_LIBRARY}
)

