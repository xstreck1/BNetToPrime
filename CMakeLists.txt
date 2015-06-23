cmake_minimum_required (VERSION 3.0)
PROJECT(PrimeImplicantEnumerator)

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif(UNIX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${PrimeImplicantEnumerator_SOURCE_DIR}/cmake)

# Gecode
FIND_PACKAGE(Gecode)
include_directories(${Gecode_INCLUDE_DIRS})

# Jsoncpp
FILE(GLOB JSON_SOURCES ${PrimeImplicantEnumerator_SOURCE_DIR}/json/*.cpp)
ADD_LIBRARY(jsonccp ${JSON_SOURCES})

# Target
FILE(GLOB MY_SOURCES ${PrimeImplicantEnumerator_SOURCE_DIR}/source/*.cpp)
FILE(GLOB MY_HEADERS ${PrimeImplicantEnumerator_SOURCE_DIR}/source/*.h)
ADD_EXECUTABLE(PrimeImplicantEnumerator ${MY_SOURCES} ${MY_HEADERS})
target_link_libraries(PrimeImplicantEnumerator jsonccp ${Gecode_LIBRARIES} ${Gecode_LIBRARIES_D})


