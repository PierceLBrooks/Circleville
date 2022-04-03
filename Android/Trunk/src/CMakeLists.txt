cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(Circleville)

#https://github.com/android-ndk/ndk/issues/381
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

set(INC ${CMAKE_CURRENT_LIST_DIR}/../../../Project/circleville/include/cv)
set(SRC ${CMAKE_CURRENT_LIST_DIR}/../../../Project/circleville/src/cv)
set(LIBRARIES )
set(DEFINES )

find_library(log-lib log)
list(APPEND "${log-lib}")

list(APPEND LIBRARIES sfml-system)
list(APPEND LIBRARIES sfml-window)
list(APPEND LIBRARIES sfml-graphics)
list(APPEND LIBRARIES sfml-audio)
list(APPEND LIBRARIES sfml-network)
list(APPEND LIBRARIES sfml-main)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../SFML-2.4.1/include)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../include)
include_directories(${INC})

list(APPEND DEFINES "SFML_JNI_CLASS=\"com/piercelbrooks/sfml/app/MainActivity\"")

set(PROJECT_FILES
	${CMAKE_CURRENT_LIST_DIR}/main.cpp
	${CMAKE_CURRENT_LIST_DIR}/native-lib.cpp
	${CMAKE_CURRENT_LIST_DIR}/../include/native-lib.hpp
	${SRC}/Circleville.cpp
	${INC}/Circleville.hpp
	${SRC}/Random.cpp
	${INC}/Random.hpp
	${CMAKE_CURRENT_LIST_DIR}/Log.cpp
	${CMAKE_CURRENT_LIST_DIR}/../include/Log.hpp
)

message(${LIBRARIES})

add_library(circleville SHARED ${PROJECT_FILES})
target_link_libraries(circleville ${LIBRARIES})
target_compile_definitions(circleville PRIVATE ${DEFINES})
