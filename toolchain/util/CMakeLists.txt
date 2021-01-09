set(SOURCES
    "exec.cpp"
    "args.cpp")

add_library(scc-util ${SOURCES})
target_include_directories(scc-util PUBLIC ${CMAKE_SOURCE_DIR}/include)
