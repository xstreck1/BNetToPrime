cmake_minimum_required (VERSION 2.4)
PROJECT(PrimeImplicantEnumerator)

# Target
FILE(GLOB MY_HEADERS ${PrimeImplicantEnumerator_SOURCE_DIR}/source/*.h)
ADD_EXECUTABLE(PrimeImplicantEnumerator main.cpp ${MY_HEADERS})


