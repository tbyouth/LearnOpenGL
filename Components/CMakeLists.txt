cmake_minimum_required(VERSION 3.2)	
project(Components)
option(GROUP_BY_EXPLORER ON)

#LIST(APPEND ComponentAllSubDir "Math")
#LIST(APPEND ComponentAllSubDir "Shader")
LIST(APPEND ComponentAllSubDir "Common")
#LIST(APPEND ComponentAllSubDir "Window")


set(PROJECT_COMPONENTS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(AllTestDir "")
foreach(dirname ${ComponentAllSubDir}) 
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${dirname}/CMakeLists.txt")
		add_subdirectory(${dirname})
		LIST(APPEND AllTestDir "${dirname}")
	endif()
endforeach()

if (ENABLE_TEST)
	enable_testing()
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	foreach(dir ${AllTestDir})
		SET(absolutePath "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
		FILE(GLOB_RECURSE TEST_FILES  ${absolutePath}*Test.cc)
		if (TEST_FILES)
#			message("TEST_FILES: ${TEST_FILES}")
#			message("HEADER_FILES: ${HEADER_FILES}")
#			message("SOURCE_FILES: ${SOURCE_FILES}")
			add_executable("${dir}Test" ${TEST_FILES} ${HEADER_FILES} ${SOURCE_FILES})
#			message("````````21313123: ${dir}Test")
#			message("Dir: ${dir}")
			target_include_directories("${dir}Test" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
			target_link_libraries("${dir}Test" PUBLIC ${dir})
			set_target_properties("${dir}Test" PROPERTIES FOLDER "AllTest")
		endif()
	endforeach()
endif()