cmake_minimum_required(VERSION 3.10)
project(printPinning)

# Find dependencies
find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

# Create target and link dependencies
add_executable(printPinning printPinning.cpp)
target_compile_features(printPinning PRIVATE cxx_std_17)
target_link_libraries(printPinning MPI::MPI_CXX OpenMP::OpenMP_CXX)

# Get system parameters and parametrize the target
# Number of threads per core
execute_process(
    COMMAND lscpu
    COMMAND grep "Thread(s) per core:"
    COMMAND grep --only-matching "[0-9]*"
    OUTPUT_VARIABLE THREADS_PER_CORE 
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE result
)
if("${THREADS_PER_CORE}" STREQUAL "" AND NOT ${result} EQUAL 0)
    message(WARNING "Failed to get the number of threads per core.")
endif()
set(THREADS_PER_CORE ${THREADS_PER_CORE} CACHE STRING "Number of threads per core")

# Number of NUMA nodes
execute_process(
    COMMAND lscpu
    COMMAND grep --extended-regexp "NUMA node[0-9]\+"
    COMMAND grep --extended-regexp --only-matching "\([0-9]\+-[0-9]\+,*\)\+"
    OUTPUT_VARIABLE NUMA_DOMAINS
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE result
)
if("${NUMA_DOMAINS}" STREQUAL "" AND NOT ${result} EQUAL 0)
    message(WARNING "Failed to get the number of NUMA nodes.")
endif()
# transform the parsed intervals to vector initializer syntax
string(REPLACE "\n" "|" NUMA_DOMAINS "${NUMA_DOMAINS}")
string(REPLACE "," "_" NUMA_DOMAINS "${NUMA_DOMAINS}")
set(NUMA_DOMAINS ${NUMA_DOMAINS} CACHE STRING "Number of NUMA nodes")

target_compile_definitions(
    ${PROJECT_NAME}
    PUBLIC
    THREADS_PER_CORE=${THREADS_PER_CORE}
    NUMA_DOMAINS=${NUMA_DOMAINS}
)
