
cmake_minimum_required(VERSION 2.8)

###
# Add our sources
set(PROJECT_SOURCES
    Selection.cpp
	Selection.h
	)

add_executable(${PROJECT_NAME}
	MACOSX_BUNDLE
    ${PROJECT_SOURCES}
	)

target_link_libraries(${PROJECT_NAME} ${LIBS})

install (TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin/
		BUNDLE DESTINATION ./ )
