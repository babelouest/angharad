#
# Angharad
#
# CMake file used to build program
#
# Copyright 2018-2023 Nicolas Mora <mail@babelouest.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the MIT License
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#

cmake_minimum_required(VERSION 3.5)

project(angharad)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

# library info

set(AGRD_LIBS )
set(BENOIC_DEVICES )

set(PROGRAM_VERSION_MAJOR "1")
set(PROGRAM_VERSION_MINOR "3")
set(PROGRAM_VERSION_PATCH "0")
set(PROJECT_DESCRIPTION "Online password and secret locker")
set(PROJECT_BUGREPORT_PATH "https://github.com/babelouest/angharad/issues")
set(ORCANIA_VERSION_REQUIRED "2.3.4")
set(YDER_VERSION_REQUIRED "1.4.21")
set(ULFIUS_VERSION_REQUIRED "2.7.16")
set(HOEL_VERSION_REQUIRED "1.4.30")
set(RHONABWY_VERSION_REQUIRED "1.1.14")
set(IDDAWC_VERSION_REQUIRED "1.1.10")

set(BENOIC_MODULES_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/device-modules/")
set(CARLEON_MODULES_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/carleon/service-modules/")
set(BENOIC_DEVICES "")
set(CARLEON_SERVICES "")

include(GNUInstallDirs)
include(CheckSymbolExists)
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)

# cmake modules

set(A_CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
list(APPEND CMAKE_MODULE_PATH "${A_CMAKE_MODULE_PATH}")

# check if _GNU_SOURCE is available

if (NOT _GNU_SOURCE)
		check_symbol_exists(__GNU_LIBRARY__ "features.h" _GNU_SOURCE)

		if (NOT _GNU_SOURCE)
				unset(_GNU_SOURCE CACHE)
				check_symbol_exists(_GNU_SOURCE "features.h" _GNU_SOURCE)
		endif ()
endif ()

if (_GNU_SOURCE)
		add_definitions(-D_GNU_SOURCE)
endif ()

list(APPEND AGRD_LIBS "-ldl -lcrypt")

find_package(Threads REQUIRED)
list(APPEND AGRD_LIBS ${CMAKE_THREAD_LIBS_INIT})

include(FindJansson)
set(JANSSON_MIN_VERSION 2.4)
find_package(Jansson ${JANSSON_MIN_VERSION} REQUIRED)
list(APPEND AGRD_LIBS Jansson::Jansson)

include(FindLibconfig)
find_package(Libconfig REQUIRED)
list(APPEND AGRD_LIBS LibConfig::LibConfig)

find_package(ZLIB REQUIRED)
list(APPEND AGRD_LIBS ZLIB::ZLIB)

# build

option(DISABLE_OAUTH2 "Disable Oauth2 security callback" OFF)
if (DISABLE_OAUTH2)
    add_executable(angharad ${CMAKE_CURRENT_SOURCE_DIR}/src/angharad.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/static_compressed_inmemory_website_callback.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/http_compression_callback.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/static_compressed_inmemory_website_callback.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/http_compression_callback.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/profile.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/scheduler.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/script.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/webservice.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/benoic.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/benoic.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/device.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/device-element.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/carleon/carleon.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/carleon/carleon.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/carleon/service.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/angharad.c )
else ()
    add_executable(angharad ${CMAKE_CURRENT_SOURCE_DIR}/src/angharad.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/iddawc_resource.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/static_compressed_inmemory_website_callback.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/http_compression_callback.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/iddawc_resource.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/static_compressed_inmemory_website_callback.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/http_compression_callback.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/profile.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/scheduler.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/script.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/webservice.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/benoic.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/benoic.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/device.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/device-element.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/carleon/carleon.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/carleon/carleon.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/carleon/service.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/angharad.c )
endif ()
target_link_libraries(angharad ${AGRD_LIBS})
set_target_properties(angharad PROPERTIES COMPILE_OPTIONS "-Wextra;-Wconversion;-Wpedantic")

find_package(Orcania ${ORCANIA_VERSION_REQUIRED} REQUIRED)
if ("${ORCANIA_VERSION_STRING}" VERSION_GREATER_EQUAL "${ORCANIA_VERSION_REQUIRED}")
    message(STATUS "Orcania found: ${ORCANIA_VERSION_STRING}")
else ()
    message( FATAL_ERROR "Orcania version required: ${ORCANIA_VERSION_REQUIRED} - version installed: ${ORCANIA_VERSION_STRING}")
endif ()
list(APPEND AGRD_LIBS Orcania::Orcania)

find_package(Yder ${YDER_VERSION_REQUIRED} REQUIRED)
if ("${YDER_VERSION_STRING}" VERSION_GREATER_EQUAL "${YDER_VERSION_REQUIRED}")
    message(STATUS "Yder found: ${YDER_VERSION_STRING}")
else ()
    message( FATAL_ERROR "Yder version required: ${YDER_VERSION_REQUIRED} - version installed: ${YDER_VERSION_STRING}")
endif ()
list(APPEND AGRD_LIBS Yder::Yder)

find_package(Ulfius ${ULFIUS_VERSION_REQUIRED} REQUIRED)
if ("${ULFIUS_VERSION_STRING}" VERSION_GREATER_EQUAL "${ULFIUS_VERSION_REQUIRED}")
    message(STATUS "Ulfius found: ${ULFIUS_VERSION_STRING}")
else ()
    message( FATAL_ERROR "Ulfius version required: ${ULFIUS_VERSION_REQUIRED} - version installed: ${ULFIUS_VERSION_STRING}")
endif ()
list(APPEND AGRD_LIBS Ulfius::Ulfius)

find_package(Hoel ${HOEL_VERSION_REQUIRED} REQUIRED)
if ("${HOEL_VERSION_STRING}" VERSION_GREATER_EQUAL "${HOEL_VERSION_REQUIRED}")
    message(STATUS "Hoel found: ${HOEL_VERSION_STRING}")
else ()
    message( FATAL_ERROR "Hoel version required: ${HOEL_VERSION_REQUIRED} - version installed: ${HOEL_VERSION_STRING}")
endif ()
list(APPEND AGRD_LIBS Hoel::Hoel)

if (DISABLE_OAUTH2)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISABLE_OAUTH2")
else ()
    find_package(Rhonabwy ${RHONABWY_VERSION_REQUIRED} REQUIRED)
    if ("${RHONABWY_VERSION_STRING}" VERSION_GREATER_EQUAL "${RHONABWY_VERSION_REQUIRED}")
        message(STATUS "Rhonabwy found: ${RHONABWY_VERSION_STRING}")
    else ()
        message( FATAL_ERROR "Rhonabwy version required: ${RHONABWY_VERSION_REQUIRED} - version installed: ${RHONABWY_VERSION_STRING}")
    endif ()
    list(APPEND AGRD_LIBS Rhonabwy::Rhonabwy)

    find_package(Iddawc ${IDDAWC_VERSION_REQUIRED} REQUIRED)
    if ("${IDDAWC_VERSION_STRING}" VERSION_GREATER_EQUAL "${IDDAWC_VERSION_REQUIRED}")
        message(STATUS "Iddawc found: ${IDDAWC_VERSION_STRING}")
    else ()
        message( FATAL_ERROR "Iddawc version required: ${IDDAWC_VERSION_REQUIRED} - version installed: ${IDDAWC_VERSION_STRING}")
    endif ()
    list(APPEND AGRD_LIBS Iddawc::Iddawc)
endif ()

target_link_libraries(angharad ${AGRD_LIBS} pthread m dl)

# benoic device modules

option(WITH_MOCKDEV "Build Mock device" off)
if (WITH_MOCKDEV)
		set(MOCK_LIB_SRC ${BENOIC_MODULES_SRC_PATH}/device-mock.c)

		set(MOCK_LIBS "-ldl")

		add_library(devmock MODULE ${MOCK_LIB_SRC})
		set_target_properties(devmock PROPERTIES
						COMPILE_OPTIONS "-Wextra;-Wconversion;-Wpedantic")
		target_link_libraries(devmock ${MOCK_LIBS})
		list(APPEND BENOIC_DEVICES devmock)
endif ()

option(WITH_ZWAVE "Build ZWave device" on)
if (WITH_ZWAVE)
		set(ZWAVE_LIB_SRC ${BENOIC_MODULES_SRC_PATH}/device-zwave.cpp)
    set(ZWAVE_LIBS )
    include(FindOpenZWAVE)
    find_package(OpenZWAVE REQUIRED)
    list(APPEND ZWAVE_LIBS libopenzwave::libopenzwave)
		if (OPENZWAVE_FOUND)
				list(APPEND ZWAVE_LIBS "-ldl -Wl,-R '-Wl,-R$$ORIGIN'")
		endif ()

		add_library(devzwave MODULE ${ZWAVE_LIB_SRC})
		set_target_properties(devzwave PROPERTIES
						COMPILE_OPTIONS "-Wextra;-Wconversion;-Wpedantic")
		target_link_libraries(devzwave ${ZWAVE_LIBS})
		list(APPEND BENOIC_DEVICES devzwave)
endif ()

option(WITH_TAULAS "Build Taulas device" on)
if (WITH_TAULAS)
		set(TAULAS_LIB_SRC ${BENOIC_MODULES_SRC_PATH}/device-taulas.c)
    set(TAULAS_LIBS )

    list(APPEND TAULAS_LIBS "-ldl")

		add_library(devtaulas MODULE ${TAULAS_LIB_SRC})
		set_target_properties(devtaulas PROPERTIES
						COMPILE_OPTIONS "-Wextra;-Wconversion;-Wpedantic")
		target_link_libraries(devtaulas ${TAULAS_LIBS})
		set(BENOIC_DEVICES ${BENOIC_DEVICES} devtaulas)
endif ()

option(WITH_ESP8266 "Build Taulas device" on)
if (WITH_ESP8266)
		set(ESP8266_LIBS_SRC ${BENOIC_MODULES_SRC_PATH}/device-esp8266.c)
    set(ESP8266_LIBS )

    list(APPEND ESP8266_LIBS "-ldl")

		add_library(devesp8266 MODULE ${ESP8266_LIBS_SRC})
		set_target_properties(devesp8266 PROPERTIES
						COMPILE_OPTIONS "-Wextra;-Wconversion;-Wpedantic")
		target_link_libraries(devesp8266 ${ESP8266_LIBS})
		set(BENOIC_DEVICES ${BENOIC_DEVICES} devesp8266)
endif ()

# carleon services modules

option(WITH_MOCKSERVICE "Build Mock service" off)
if (WITH_MOCKSERVICE)
		set(MOCK_LIB_SRC ${CARLEON_MODULES_SRC_PATH}/service-mock.c)
    set(MOCK_LIBS )

		list(APPEND MOCK_LIBS "-ldl")

		add_library(servicemock MODULE ${MOCK_LIB_SRC})
		set_target_properties(servicemock PROPERTIES
						COMPILE_OPTIONS "-Wextra;-Wconversion;-Wpedantic")
		target_link_libraries(servicemock ${MOCK_LIBS})
		set(CARLEON_SERVICES ${CARLEON_SERVICES} servicemock)
endif ()

option(WITH_MOTION "Build Motion service" on)
if (WITH_MOTION)
		set(MOTION_LIB_SRC ${CARLEON_MODULES_SRC_PATH}/service-motion.c)
    set(MOTION_LIBS )

		list(APPEND MOTION_LIBS "-ldl")

		add_library(servicemotion MODULE ${MOTION_LIB_SRC})
		set_target_properties(servicemotion PROPERTIES
						COMPILE_OPTIONS "-Wextra;-Wconversion;-Wpedantic")
		target_link_libraries(servicemotion ${MOTION_LIBS})
		set(CARLEON_SERVICES ${CARLEON_SERVICES} servicemotion)
endif ()

option(WITH_LIQUIDSOAP "Build Liquidsoap service" on)
if (WITH_LIQUIDSOAP)
		set(LIQUIDSOAP_LIB_SRC ${CARLEON_MODULES_SRC_PATH}/service-liquidsoap.c)
    set(LIQUIDSOAP_LIBS )

		list(APPEND LIQUIDSOAP_LIBS "-ldl")

		add_library(serviceliquidsoap MODULE ${LIQUIDSOAP_LIB_SRC})
		set_target_properties(serviceliquidsoap PROPERTIES
						COMPILE_OPTIONS "-Wextra;-Wconversion;-Wpedantic")
		target_link_libraries(serviceliquidsoap ${LIQUIDSOAP_LIBS})
		set(CARLEON_SERVICES ${CARLEON_SERVICES} serviceliquidsoap)
endif ()

option(WITH_MPD "Build MPD service" on)
if (WITH_MPD)
		set(MPD_LIB_SRC ${CARLEON_MODULES_SRC_PATH}/service-mpd.c)
    set(LIBMPDCLIENT_LIBRARIES )

		include(FindLibMPDClient)
		find_package(LibMPDClient REQUIRED)
    list(APPEND LIBMPDCLIENT_LIBRARIES LibMPDClient::LibMPDClient)
		if (LIBMPDCLIENT_FOUND)
				list(APPEND LIBMPDCLIENT_LIBRARIES "-ldl")
		endif ()

		add_library(servicempd MODULE ${MPD_LIB_SRC})
		set_target_properties(servicempd PROPERTIES
						COMPILE_OPTIONS "-Wextra;-Wconversion;-Wpedantic")
		target_link_libraries(servicempd ${LIBMPDCLIENT_LIBRARIES})
		set(CARLEON_SERVICES ${CARLEON_SERVICES} servicempd)
endif ()

option(WITH_WEATHERMAP "Build Weathermap service" on)
if (WITH_WEATHERMAP)
		set(WEATHERMAP_LIB_SRC ${CARLEON_MODULES_SRC_PATH}/service-weathermap.c)
    set(LIBWEATHERMAP_LIBRARIES )

		add_library(serviceweathermap MODULE ${WEATHERMAP_LIB_SRC})
		set_target_properties(serviceweathermap PROPERTIES
						COMPILE_OPTIONS "-Wextra;-Wconversion;-Wpedantic")
		target_link_libraries(serviceweathermap ${LIBWEATHERMAP_LIBRARIES})
		set(CARLEON_SERVICES ${CARLEON_SERVICES} serviceweathermap)
endif ()

# tests

option(BUILD_TESTING "Build the testing tree." OFF)

if (BUILD_TESTING)
    find_package(Check REQUIRED)
    if (CHECK_FOUND)
        if (NOT WIN32 AND NOT APPLE)
            include(FindSubunit)
            find_package(Subunit REQUIRED)
        endif ()

        enable_testing()

        set(CMAKE_CTEST_COMMAND ctest -V)

        set(TST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
        set(U_RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/docs/resources/ulfius/)
        set(TST_LIBS Check::Check)
        if (NOT WIN32)
            find_package(Threads REQUIRED)
            list(APPEND TST_LIBS ${CMAKE_THREAD_LIBS_INIT} m)
        endif ()
        if (NOT APPLE AND NOT WIN32)
            list(APPEND TST_LIBS rt)
        endif ()
        if (NOT WIN32 AND NOT APPLE)
            list(APPEND TST_LIBS Subunit::Subunit)
        endif ()

        list(APPEND TST_LIBS ${AGRD_LIBS})

        set(TESTS )
				list(APPEND TESTS angharad_submodule
                          angharad_script
                          angharad_scheduler
                          angharad_profile
                          benoic_device
                          benoic_device_element
                          benoic_monitor
                          carleon_service
                          carleon_liquidsoap
                          carleon_motion
                          carleon_mpd)
						

				configure_file(
								"${A_CMAKE_MODULE_PATH}/CTestCustom.cmake.in"
								"${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake"
								@ONLY)

        foreach (t ${TESTS})
            add_executable(${t} EXCLUDE_FROM_ALL ${TST_DIR}/${t}.c ${TST_DIR}/unit-tests.c ${TST_DIR}/unit-tests.h)
            target_include_directories(${t} PUBLIC ${TST_DIR})
            target_link_libraries(${t} PUBLIC ${TST_LIBS})
            add_test(NAME ${t}
                    COMMAND ${t} "private-test.jwks")
        endforeach ()
		endif ()
endif ()

# install target

install(TARGETS angharad
				RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES docs/angharad.conf.sample 
				DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/angharad RENAME angharad.conf COMPONENT config)
install(TARGETS ${BENOIC_DEVICES}
				DESTINATION ${CMAKE_INSTALL_LIBDIR}/angharad/benoic)
install(TARGETS ${CARLEON_SERVICES}
				DESTINATION ${CMAKE_INSTALL_LIBDIR}/angharad/carleon)
install(DIRECTORY webapp/
				DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/angharad/webapp/ COMPONENT runtime)
install(FILES webapp/config.json.sample 
				DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/angharad/webapp/ RENAME config.json COMPONENT config)
install(FILES README.md
				DESTINATION ${CMAKE_INSTALL_DOCDIR}/ COMPONENT runtime)
install(FILES docs/API.md
				DESTINATION ${CMAKE_INSTALL_DOCDIR}/ COMPONENT runtime)
install(FILES docs/angharad-init
				DESTINATION ${CMAKE_INSTALL_DOCDIR}/ COMPONENT runtime)
install(FILES docs/angharad.service
				DESTINATION ${CMAKE_INSTALL_DOCDIR}/ COMPONENT runtime)
install(FILES docs/angharad.mariadb.sql
				DESTINATION ${CMAKE_INSTALL_DOCDIR}/ COMPONENT runtime)
install(FILES docs/angharad.sqlite3.sql
				DESTINATION ${CMAKE_INSTALL_DOCDIR}/ COMPONENT runtime)

set(TARGETS angharad)

install(TARGETS ${TARGETS}
				RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
				CONFIGURATIONS Release)

# uninstall target

if (NOT TARGET uninstall)
		configure_file(
						"${A_CMAKE_MODULE_PATH}/CMakeUninstall.cmake.in"
						"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
						IMMEDIATE @ONLY)
		add_custom_target(uninstall
						COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif ()

# packaging

set(CPACK_PACKAGE_VERSION_MAJOR ${PROGRAM_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROGRAM_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROGRAM_VERSION_PATCH})

set(PACKAGE_FILE_NAME
				"${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(PACKAGE_IGNORED_FILES
				"${CMAKE_CURRENT_BINARY_DIR}/;/.git/;.gitignore;~$;${CPACK_SOURCE_IGNORE_FILES}")

set(CPACK_GENERATOR "TGZ;DEB")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "mail@babelouest.org")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/babelouest/angharad")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.4), libconfig9, libjansson4 (>= 2.7), liborcania (>= 1.2), libyder (>= 1.3), libulfius (>= 2.3), libhoel (>= 1.4), lsb-base (>= 3.0-6)")
set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_FILE_NAME})

set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${PACKAGE_FILE_NAME})
set(CPACK_SOURCE_IGNORE_FILES ${PACKAGE_IGNORED_FILES})

include(CPack)

add_custom_target(dist
				COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
