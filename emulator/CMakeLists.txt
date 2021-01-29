project(cx16-emulator)
cmake_minimum_required(VERSION 3.0)

set(SOURCES
	"Config.cpp"
	"CPU.cpp"
	"Cx16.cpp"
	"Display.cpp"
	"Gfx.cpp"
	"HDD.cpp"
	"main.cpp"
)

add_executable(cx16-emulator ${SOURCES})
target_link_libraries(cx16-emulator pthread)
