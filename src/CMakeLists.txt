cmake_minimum_required(VERSION 2.8.11)

project(speedcrunch)
set(speedcrunch_VERSION "master" )

set(CMAKE_AUTOMOC ON)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)
find_package(Qt5Widgets)

if(APPLE)
    set(PROGNAME SpeedCrunch)
    set(MACOSX_BUNDLE_ICON_FILE speedcrunch.icns)
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${speedcrunch_VERSION})
    set(MACOSX_BUNDLE_VERSION ${speedcrunch_VERSION})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING Version ${speedcrunch_VERSION})
    set(CMAKE_OSX_ARCHITECTURES ppc;i386)
else(APPLE)
    set(PROGNAME speedcrunch)
endif(APPLE)

ADD_DEFINITIONS("-DSPEEDCRUNCH_VERSION=\"${speedcrunch_VERSION}\"")
ADD_DEFINITIONS(-DQT_USE_QSTRINGBUILDER)

include(SourceFiles.cmake)

if(MINGW)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/speedcrunchico.o
                       COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/resources/speedcrunch.rc -o ${CMAKE_CURRENT_BINARY_DIR}/speedcrunchico.o)
    set(speedcrunch_SOURCES ${speedcrunch_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/speedcrunchico.o)
else(MINGW)
    set(speedcrunch_SOURCES ${speedcrunch_SOURCES} resources/speedcrunch.rc)
endif(MINGW)

string(COMPARE EQUAL "${CMAKE_CXX_COMPILER_ID}" MSVC USING_MSVC)
if(USING_MSVC)
    add_definitions(/Wall)
else()
    add_definitions(-Wall)
    add_definitions(-std=c++0x)
endif()

qt5_add_RESOURCES( speedcrunch_RESOURCES_SOURCES ${speedcrunch_RESOURCES} )
add_executable(${PROGNAME} WIN32 MACOSX_BUNDLE ${speedcrunch_SOURCES} ${speedcrunch_HEADERS_MOC} ${speedcrunch_RESOURCES_SOURCES} ${speedcrunch_FORMS_HEADERS})

if(APPLE)
    set( speedcrunch_RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources )
    add_custom_command(TARGET ${PROGNAME} POST_BUILD
                       COMMAND mkdir ARGS -p ${speedcrunch_RESOURCE_DIR}
                       COMMAND cp ARGS -f resources/${MACOSX_BUNDLE_ICON_FILE} ${speedcrunch_RESOURCE_DIR})
endif(APPLE)

add_custom_target(confclean COMMAND rm -rf Makefile CMakeFiles/ CMakeCache.txt cmake_install.cmake DartTestfile.txt install_manifest.txt)

set(QT_LIBRARIES Qt5::Widgets)

find_library(x11 X11)
if(x11)
    target_link_libraries(${PROGNAME} ${QT_LIBRARIES} X11)
else(x11)
    target_link_libraries(${PROGNAME} ${QT_LIBRARIES})
endif(x11)

enable_testing()

add_executable(testhmath ${testhmath_SOURCES})
add_test(testhmath testhmath)

add_executable(testevaluator ${testevaluator_SOURCES} ${testevaluator_HEADERS_MOC})
target_link_libraries(testevaluator ${QT_LIBRARIES})
add_test(testevaluator testevaluator)

add_executable(testfloatnum ${testfloatnum_SOURCES})
add_test(testfloatnum testfloatnum)

include_directories(${CMAKE_BINARY_DIR} thirdparty core gui math)

################################# INSTALL ######################################

if(NOT WIN32)
    set(SHAREDIR "share/")
    set(MENUDIR "${SHAREDIR}/applications/")
    set(ICONDIR "${SHAREDIR}/pixmaps/")
endif(NOT WIN32)

option(PORTABLE "Enable to build in portable mode" OFF)
if(PORTABLE_SPEEDCRUNCH)
    add_definitions(-DSPEEDCRUNCH_PORTABLE)
endif(PORTABLE_SPEEDCRUNCH)

install(TARGETS ${PROGNAME} DESTINATION bin)

if(NOT WIN32)
    install(FILES ../pkg/speedcrunch.desktop DESTINATION ${MENUDIR})
    install(FILES resources/speedcrunch.png DESTINATION ${ICONDIR})
endif(NOT WIN32)

################################ UNINSTALL #####################################

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
                "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
                IMMEDIATE @ONLY)

add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

