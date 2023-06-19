cmake_minimum_required(
	VERSION 3.1
)

project(SGtest
)


add_executable(SGtest
	#main.cpp
	#test05-filescanner.cpp
	#test04-numericpad.cpp
	#test03-library.cpp
	#test02-registerform.cpp
	#test01-packager.cpp
	test00-helloworld.cpp
)

target_include_directories(SGtest
	PRIVATE .
)

include (
	${CMAKE_ROOT}/Modules/FindX11.cmake
)

include_directories(
	${X11_INCLUDE_DIR}
)

target_link_libraries(SGtest 
	${X11_LIBRARIES}
)
