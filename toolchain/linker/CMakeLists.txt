set(SOURCES
    "main.cpp")

add_executable(pcos-cx16-ld ${SOURCES})
target_include_directories(pcos-cx16-ld PUBLIC ${CMAKE_SOURCE_DIR}/include)
