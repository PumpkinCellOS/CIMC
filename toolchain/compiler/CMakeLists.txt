set(SOURCES
    "main.cpp")

add_executable(pcos-cx16-cc ${SOURCES})
target_include_directories(pcos-cx16-cc PUBLIC ${CMAKE_SOURCE_DIR}/include)
