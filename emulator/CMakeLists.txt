project(cx16-emulator)
cmake_minimum_required(VERSION 3.0)

set(SOURCES
	"CPU.cpp"
	"Cx16.cpp"
	"main.cpp"
)

add_executable(cx16-emulator ${SOURCES})

