cmake_minimum_required(
	VERSION 3.1
)

project(SGUI
)


add_executable(SGUI
	test03-library.cpp
)

target_include_directories(SGUI
	PRIVATE .
)

include (
	${CMAKE_ROOT}/Modules/FindX11.cmake
)

include_directories(
	${X11_INCLUDE_DIR}
)

target_link_libraries(SGUI 
	${X11_LIBRARIES}
)
