project(cx16-emulator)
cmake_minimum_required(VERSION 3.0)

set(SOURCES
	"Cx16.cpp"
	"main.cpp"
)

add_executable(cx16-emulator ${SOURCES})

