
# *************************************************************************
# how to use
# *************************************************************************
# mkdir build
# cd build
# cmake .. -DCMAKE_BUILD_TYPE=Release  (Windows/Linux/FreeBSD)
# cmake .. -DCMAKE_BUILD_TYPE=Release -G"Unix Makefiles"  (macOS)
# cmake --build . --config Release
# *************************************************************************

cmake_minimum_required(VERSION 3.24)
project(utility VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# *************************************************************************
# The basic library we are making
# *************************************************************************

add_library(utility STATIC
    buffer.cpp
    filemap.cpp
    timer.cpp
    
    buffer.hpp
    sfilemap.hpp
    timer.hpp
    strutil.hpp
    numinc.hpp
)

if (WIN32)

	target_compile_definitions( utility PRIVATE
		_LIB
		$<$<CONFIG:Release>:NDEBUG>
		$<$<CONFIG:Debug>:_DEBUG>
	)
	
	target_compile_options( utility PRIVATE
		$<$<CONFIG:Release>:/O2>
	)
	
else()

	if (APPLE)
		target_compile_options( utility PRIVATE
			$<$<CONFIG:Release>:-Os>
		)
	else()
		target_compile_options(utility PRIVATE
			$<$<CONFIG:Release>:-O2>
		)
	endif()
endif(WIN32)



