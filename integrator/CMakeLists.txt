CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
project(wxdfast-integrator)
set(PROJECT_VERSION "1.0.0")

if(MSVC_VERSION LESS "1900")
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
    return()
endif()

if(NOT MSVC)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
    if(COMPILER_SUPPORTS_CXX14)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
        return()
    endif()
endif()

set(INTEGRATOR_LIBS "")
set(INTEGRATOR_INCLUDE_DIRS "")

include(CheckFunctionExists)
CHECK_FUNCTION_EXISTS("inet_ntop" HAVE_INET_NTOP_EXISTS)
if(HAVE_INET_NTOP_EXISTS)
    set(HAVE_INET_NTOP 1)
endif()
CHECK_FUNCTION_EXISTS("inet_pton" HAVE_INET_PTON_EXISTS)
if(HAVE_INET_PTON_EXISTS)
    set(HAVE_INET_PTON 1)
endif()
if(WIN32)
    CHECK_FUNCTION_EXISTS("strncasecmp" HAVE_STRNCASECMP_EXISTS)
    if(HAVE_STRNCASECMP_EXISTS)
        set(HAVE_STRNCASECMP 1)
    endif()
endif(WIN32)
include(CheckIncludeFiles)
CHECK_INCLUDE_FILES(sys/param.h HAVE_SYS_PARAM_H)
CHECK_INCLUDE_FILES(sys/socket.h HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILES(netinet/in.h HAVE_NETINET_IN_H)
CHECK_INCLUDE_FILES(arpa/inet.h HAVE_ARPA_INET_H)

list(APPEND INTEGRATOR_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
wxdfast-integrator.cpp
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
add_definitions(-DDEBUG)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

include_directories(${INTEGRATOR_INCLUDE_DIRS}
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_BINARY_DIR}
)

if(WIN32)
list(APPEND INTEGRATOR_LIBS ws2_32)
endif(WIN32)

ADD_EXECUTABLE(wxdfast-integrator ${SOURCES})
if(WIN32)
    TARGET_LINK_LIBRARIES(wxdfast-integrator "-Wl,--subsystem,console -mwindows" ${INTEGRATOR_LIBS})
else(WIN32)
    TARGET_LINK_LIBRARIES(wxdfast-integrator ${INTEGRATOR_LIBS})
endif(WIN32)

INSTALL(TARGETS wxdfast-integrator DESTINATION bin)

