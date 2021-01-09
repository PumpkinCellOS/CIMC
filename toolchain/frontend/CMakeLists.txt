set(SOURCES
    "main.cpp")

add_executable(${SCC_PREFIX}-s++ ${SOURCES})
target_include_directories(${SCC_PREFIX}-s++ PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${SCC_PREFIX}-s++ scc-util)
