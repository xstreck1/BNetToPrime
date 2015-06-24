cmake_minimum_required (VERSION 2.0)
PROJECT(PrimeImplicantEnumerator)

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif(UNIX)

# Target
FILE(GLOB MY_SOURCES ${PrimeImplicantEnumerator_SOURCE_DIR}/source/*.cpp)
FILE(GLOB MY_HEADERS ${PrimeImplicantEnumerator_SOURCE_DIR}/source/*.h)
ADD_EXECUTABLE(PrimeImplicantEnumerator ${MY_SOURCES} ${MY_HEADERS})


