cmake_minimum_required(VERSION 3.0)
project(CIMC_Toolchain)

set(SCC_PREFIX "pcos-cx16")

add_subdirectory(compiler)
add_subdirectory(frontend)
add_subdirectory(linker)
add_subdirectory(util)
