cmake_minimum_required(VERSION 3.7)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
project(wxdfast)
set(PROJECT_VERSION "0.70.4")

set(WXDFAST_INCLUDE_DIRS "")
set(WXDFAST_LIBS "")

if(MSVC_VERSION LESS "1900")
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
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

set(LANGS
ar
bg
bn
cs
de
es
fo
fr
he
hi
hu
id
it
ja
ko
lv
ms
nl
pl
pt_BR
ru
sv
tr
uk
ur
)

set(DX_WX_CONFIG wx-config CACHE STRING "wx-config executable")
if(MSYS)
    set(wxWidgets_FIND_STYLE "unix")
endif(MSYS)
if(UNIX)
    execute_process(COMMAND which ${DX_WX_CONFIG} OUTPUT_VARIABLE WX_TOOL OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT WX_TOOL)
        message(FATAL_ERROR "\nNo functional wx_config script was found in your PATH.\nIs the wxWidgets development package installed?")
    else()
        execute_process(COMMAND sh ${WX_TOOL} --version OUTPUT_VARIABLE WX_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
        string(SUBSTRING "${WX_VERSION}" "0" "1" wxMAJOR_VERSION)
        string(SUBSTRING "${WX_VERSION}" "2" "1" wxMINOR_VERSION)
        string(SUBSTRING "${WX_VERSION}" "4" "1" wxRELEASE_NUMBER)
        if(wxMAJOR_VERSION LESS 3)
        message(FATAL_ERROR "\nI'm afraid your wxWidgets version is too old.\nBuilding wxdfast requires at least wxWidgets-3.0.0")
        endif()
    endif()
    message("-- wx-config used is: ${WX_TOOL}")
    message("-- wxWidgets version is: ${WX_VERSION}")
    if(NOT APPLE AND NOT MINGW)
        # Is the wx we are using built on gtk2 or 3?
        execute_process(COMMAND ${WX_TOOL} --selected_config OUTPUT_VARIABLE WX_GTK_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
        string(SUBSTRING "${WX_GTK_VERSION}" "3" "1" GTK_VERSION)
        message("-- gtk version is: ${GTK_VERSION}")
    endif()
    set(wxWidgets_CONFIG_EXECUTABLE ${WX_TOOL})
endif(UNIX)

set(wxWidgets_USE_LIBS)
option(WX_MONOLITHIC "Use wx monolithic build" FALSE)
if(WX_MONOLITHIC)
    find_package(wxWidgets COMPONENTS mono)
else(WX_MONOLITHIC)
    find_package(wxWidgets COMPONENTS core base adv net xml)
endif(WX_MONOLITHIC)
if(wxWidgets_FOUND)
   include("${wxWidgets_USE_FILE}")
else(wxWidgets_FOUND)
   # For convenience. When we cannot continue, inform the user
   message("wxWidgets not found!")
endif(wxWidgets_FOUND)

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBARIA2 libaria2)
    list(APPEND WXDFAST_INCLUDE_DIRS ${LIBARIA2_INCLUDE_DIRS})
    list(APPEND WXDFAST_LIBS ${LIBARIA2_LIBRARIES})
endif(PKG_CONFIG_FOUND)

if(NOT LIBARIA2_FOUND)
    message(FATAL_ERROR "Aria2 library not found. Can't build wxDownload Fast")
endif()

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
        list(APPEND WXDFAST_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR})
        list(APPEND WXDFAST_LIBS ${OPENSSL_LIBRARIES})
else()
    message(FATAL_ERROR "OpenSSL library not found. Can't build wxDownload Fast")
endif()

if(NOT WIN32)
# Source distribution
set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})
add_custom_target(dist COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} &&
    rm -rf ${ARCHIVE_NAME} &&
    svn export . ${ARCHIVE_NAME} &&
    tar czf ${ARCHIVE_NAME}.tar.gz ${ARCHIVE_NAME} &&
    rm -rf ${ARCHIVE_NAME}
)
endif(NOT WIN32)

add_subdirectory(po)
add_subdirectory(resources)
add_subdirectory(src/wxjson)
add_subdirectory(integrator)

set(SRCS
src/widgets/dxlistctrl.cpp
src/widgets/dxtoolbar.cpp
src/Icons.cpp
src/wxDFast.cpp
src/Options.cpp
src/Graph.cpp
src/UtilFunctions.cpp
src/OptionsDialog.cpp
src/Progress.cpp
src/Engine.cpp
src/FileInfo.cpp
src/hash/hash.cpp
src/NewDialog.cpp
src/BatchDialog.cpp
)

if(WIN32)
    set(SRCS ${SRCS} src/resource.rc)
endif(WIN32)

if(NOT WIN32)
    if(APPLE)
    add_definitions(-DLOCALEDIR="../Resources")
    add_definitions(-DWXDFAST_DATADIR="../Resources")
    else()
    add_definitions(-DDESKTOPDIR="${CMAKE_INSTALL_PREFIX}/share/applications")
    add_definitions(-DLOCALEDIR="${CMAKE_INSTALL_PREFIX}/share/locale")
    add_definitions(-DWXDFAST_DATADIR="${CMAKE_INSTALL_PREFIX}/share/wxdfast")
    endif(APPLE)
else()
    add_definitions(-DLOCALEDIR="./locale")
    add_definitions(-DWXDFAST_DATADIR=".")
endif(NOT WIN32)

if(CMAKE_BUILD_TYPE MATCHES Debug)
add_definitions(-DDEBUG)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

configure_file(config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/src
 ${CMAKE_BINARY_DIR}
 ${WXJSON_INCLUDE_DIRS}
 ${WXDFAST_INCLUDE_DIRS}
)

if(APPLE)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
    set(PROGNAME wxDownloadFast)
    add_executable(wxDownloadFast MACOSX_BUNDLE ${SRCS})
    SET_TARGET_PROPERTIES(wxDownloadFast PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
    set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/resources/RipStop/icon/wxdfast.icns)
    set(wxdfast_RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/wxDownloadFast.app/Contents/Resources)
    ADD_CUSTOM_COMMAND(TARGET wxDownloadFast POST_BUILD
    COMMAND mkdir ARGS -p ${wxdfast_RESOURCE_DIR}
    COMMAND mkdir ARGS -p ${wxdfast_RESOURCE_DIR}/icons
    COMMAND mkdir ARGS -p ${wxdfast_RESOURCE_DIR}/icons/logo
    COMMAND mkdir ARGS -p ${wxdfast_RESOURCE_DIR}/icons/menubar
    COMMAND mkdir ARGS -p ${wxdfast_RESOURCE_DIR}/icons/toolbar
    COMMAND cp ARGS -f ${CMAKE_CURRENT_SOURCE_DIR}/resources/RipStop/icon/wxdfast.icns ${wxdfast_RESOURCE_DIR}
    COMMAND cp ARGS -f ${CMAKE_CURRENT_SOURCE_DIR}/resources/extensionid ${wxdfast_RESOURCE_DIR}
    COMMAND cp ARGS -f ${CMAKE_CURRENT_SOURCE_DIR}/resources/RipStop/icon/wxdfast.png ${wxdfast_RESOURCE_DIR}/icons
    COMMAND cp ARGS -f ${CMAKE_CURRENT_SOURCE_DIR}/resources/RipStop/logo/*.png ${wxdfast_RESOURCE_DIR}/icons/logo
    COMMAND cp ARGS -f ${CMAKE_CURRENT_SOURCE_DIR}/resources/RipStop/menubar/*.png ${wxdfast_RESOURCE_DIR}/icons/menubar
    COMMAND cp ARGS -f ${CMAKE_CURRENT_SOURCE_DIR}/resources/RipStop/toolbar/*.png ${wxdfast_RESOURCE_DIR}/icons/toolbar
    COMMAND cp ARGS -f ${CMAKE_CURRENT_BINARY_DIR}/integrator/wxdfast-integrator ${CMAKE_CURRENT_BINARY_DIR}/wxDownloadFast.app/Contents/MacOS
    COMMENT Setting up resources)
    foreach(lang ${LANGS})
        ADD_CUSTOM_COMMAND(TARGET wxDownloadFast POST_BUILD
        COMMAND mkdir ARGS -p ${wxdfast_RESOURCE_DIR}/${lang}.lproj
        COMMAND cp ARGS -f ${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}.mo ${wxdfast_RESOURCE_DIR}/${lang}.lproj/wxdfast.mo
        COMMENT Setting up translations)
    endforeach(lang)
    target_link_libraries(wxDownloadFast ${wxWidgets_LIBRARIES} ${WXJSON_LIBRARIES} ${WXDFAST_LIBS})
    install(TARGETS wxDownloadFast DESTINATION bin)
else()
    add_executable(wxdfast WIN32 ${SRCS})
    target_link_libraries(wxdfast ${wxWidgets_LIBRARIES} ${WXJSON_LIBRARIES} ${WXDFAST_LIBS})
    install(TARGETS wxdfast DESTINATION bin)
endif(APPLE)



# uninstall target
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/uninstall.cmake
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake/uninstall.cmake)

message(STATUS "wxDownload Fast")
message(STATUS "Install prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "wxWidgets version: ${WX_VERSION}")
if(NOT APPLE AND NOT MINGW)
    message(STATUS " gtk version:      ${GTK_VERSION}")
endif()
message(STATUS "LibAria2 version:  ${LIBARIA2_VERSION}")
message(STATUS "OpenSSL version:   ${OPENSSL_VERSION}")
