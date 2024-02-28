cmake_minimum_required(VERSION 3.10)
project(printPinning)

find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

# One can switch of MPI support entirely with this
# if(MPI_FOUND)
#     include_directories(${MPI_INCLUDE_PATH})
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS}")
#     set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} ${MPI_CXX_LINK_FLAGS}")
# else(MPI_FOUND)
#     message(SEND_ERROR "This application cannot compile without MPI")
#     return()       
# endif(MPI_FOUND)

# # Add executable 
# add_executable(${PROJECT_NAME} printPinning.cpp)

# # Link against MPI, OpenMP and -fopenmp
# target_link_libraries(${PROJECT_NAME} ${MPI_LIBRARIES} OpenMP::OpenMP_CXX)
# set_target_properties(${PROJECT_NAME} PROPERTIES
#     COMPILE_FLAGS "${MPI_CXX_COMPILE_FLAGS} ${OpenMP_CXX_FLAGS}"
#     LINK_FLAGS "${MPI_CXX_LINK_FLAGS} ${OpenMP_CXX_FLAGS}"
# )


add_executable(printPinning printPinning.cpp)

target_link_libraries(printPinning MPI::MPI_CXX OpenMP::OpenMP_CXX)
