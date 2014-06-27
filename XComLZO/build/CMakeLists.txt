PROJECT(XComLZO)

SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -Wall -fexceptions -O2")

ADD_EXECUTABLE(XComLZO ../main.cpp)

ADD_LIBRARY(minilzo ../minilzo.c ../minilzo.h ../lzodefs.h ../lzoconf.h)

TARGET_LINK_LIBRARIES(XComLZO minilzo)
