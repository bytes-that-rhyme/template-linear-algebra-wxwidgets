cmake_minimum_required(VERSION 3.29)
project(vcpkg_wxwidgets)

set(CMAKE_CXX_STANDARD 20)

set(HEADER_ONLY ON CACHE BOOL "Link to BLAS and LAPACK functions directly without using wrapper" FORCE)

include(FetchContent)

FetchContent_Declare(
    armadillo
    GIT_REPOSITORY https://gitlab.com/conradsnicta/armadillo-code.git
    GIT_TAG 14.4.x
)

FetchContent_MakeAvailable(armadillo)

# Project configuration
# Add your source and header files here.
add_executable(vcpkg_wxwidgets WIN32 main.cpp)

# Since we are using the header-only version of Armadillo, we need to tell
# Armadillo which libraries it should use.
target_compile_definitions(vcpkg_wxwidgets PRIVATE
  ARMA_USE_LAPACK
  ARMA_USE_SUPERLU
  ARMA_USE_BLAS
  ARMA_DONT_USE_WRAPPER)

# Get packages installed by vcpkg
find_package(wxWidgets REQUIRED COMPONENTS net core base)
message(STATUS "wxWidgets libraries: ${wxWidgets_LIBRARIES}")
target_link_libraries(vcpkg_wxwidgets PRIVATE ${wxWidgets_LIBRARIES})

find_package(superlu CONFIG REQUIRED)
target_link_libraries(vcpkg_wxwidgets PRIVATE superlu::superlu)

find_package(openblas CONFIG REQUIRED)
target_link_libraries(vcpkg_wxwidgets PRIVATE OpenBLAS::OpenBLAS)

find_package(lapack CONFIG REQUIRED)
message(STATUS "Lapack libraries: ${LAPACK_LIBRARIES}")
target_link_libraries(vcpkg_wxwidgets PRIVATE ${LAPACK_LIBRARIES})

# metis is needed by SuperLU
find_package(metis CONFIG REQUIRED)
target_link_libraries(vcpkg_wxwidgets PRIVATE metis)

# Add the include directory for Armadillo. We don't link the library
# since we are using the header-only library instead.
target_include_directories(vcpkg_wxwidgets PRIVATE ${armadillo_SOURCE_DIR}/include)

add_custom_command(TARGET vcpkg_wxwidgets POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:vcpkg_wxwidgets> $<TARGET_FILE_DIR:vcpkg_wxwidgets>
        COMMAND_EXPAND_LISTS
)
