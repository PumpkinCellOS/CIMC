set(SOURCES
    "main.cpp")

add_executable(pcos-cx16-s++ ${SOURCES})
target_include_directories(pcos-cx16-s++ PUBLIC ${CMAKE_SOURCE_DIR}/include)
