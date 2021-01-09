set(SOURCES
    "main.cpp")

add_executable(${SCC_PREFIX}-ld ${SOURCES})
target_include_directories(${SCC_PREFIX}-ld PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${SCC_PREFIX}-ld scc-util)
