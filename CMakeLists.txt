#
# Angharad
#
# CMake file used to build program
#
# Copyright 2018 Nicolas Mora <mail@babelouest.org>
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

set(PROGRAM_VERSION_MAJOR "1")
set(PROGRAM_VERSION_MINOR "2")
set(PROGRAM_VERSION_PATCH "4")
set(PROJECT_DESCRIPTION "Online password and secret locker")
set(PROJECT_BUGREPORT_PATH "https://github.com/babelouest/angharad/issues")
set(ORCANIA_VERSION_REQUIRED "2.2.0")
set(YDER_VERSION_REQUIRED "1.4.13")
set(ULFIUS_VERSION_REQUIRED "2.7.3")
set(RHONABWY_VERSION_REQUIRED "0.9.999")
set(IDDAWC_VERSION_REQUIRED "1.0.0")
set(HOEL_VERSION_REQUIRED "1.4.17")

set(BENOIC_MODULES_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/device-modules/")
set(CARLEON_MODULES_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/carleon/service-modules/")
set(BENOIC_DEVICES "")
set(CARLEON_SERVICES "")

include(GNUInstallDirs)
include(CheckSymbolExists)

# cmake modules

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

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

include(FindJansson)
set(JANSSON_MIN_VERSION 2.4)
find_package(Jansson ${JANSSON_MIN_VERSION} REQUIRED)
if (JANSSON_FOUND)
		set(LIBS ${JANSSON_LIBRARIES})
endif ()

include(FindLibconfig)
find_package(Libconfig REQUIRED)
if (LIBCONFIG_FOUND)
		set(LIBS ${LIBS} ${LIBCONFIG_LIBRARIES})
		include_directories(${LIBCONFIG_INCLUDE_DIRS})
endif ()

include(FindZLIB)
find_package(ZLIB REQUIRED)
set(LIBS ${LIBS} ${ZLIB_LIBRARIES})

# build

add_executable(angharad ${CMAKE_CURRENT_SOURCE_DIR}/src/angharad.h
								${CMAKE_CURRENT_SOURCE_DIR}/src/iddawc_resource.h
								${CMAKE_CURRENT_SOURCE_DIR}/src/static_compressed_inmemory_website_callback.h
								${CMAKE_CURRENT_SOURCE_DIR}/src/http_compression_callback.h
								${CMAKE_CURRENT_SOURCE_DIR}/src/iddawc_resource.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/static_compressed_inmemory_website_callback.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/http_compression_callback.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/condition.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/profile.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/scheduler.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/script.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/trigger.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/webservice.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/benoic.h
								${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/benoic.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/device.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/benoic/device-element.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/carleon/carleon.h
								${CMAKE_CURRENT_SOURCE_DIR}/src/carleon/carleon.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/carleon/service.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/gareth/gareth.h
								${CMAKE_CURRENT_SOURCE_DIR}/src/gareth/gareth.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/gareth/alert.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/gareth/filter.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/gareth/message.c
								${CMAKE_CURRENT_SOURCE_DIR}/src/angharad.c )
target_link_libraries(angharad ${LIBS})

set(SEARCH_ORCANIA OFF CACHE BOOL "Force to false") # Avoid to search and download orcania during orcania search and download
set(SEARCH_YDER OFF CACHE BOOL "Force to false") # Avoid to search and download yder during ulfius and hoel search and download

set(SEARCH_ORCANIA_U OFF CACHE BOOL "Force to false") # Avoid to search and download orcania during ulfius search and download
set(BUILD_UWSC OFF CACHE BOOL "Force to false") # Avoid to build uwsc during ulfius search and download

set(SEARCH_ORCANIA_H OFF CACHE BOOL "Force to false") # Avoid to search and download orcania during hoel search and download

set(SEARCH_ORCANIA_R OFF CACHE BOOL "Force to false") # Avoid to search and download orcania during hoel search and download
set(SEARCH_YDER_R OFF CACHE BOOL "Force to false") # Avoid to search and download yder during ulfius and hoel search and download
set(SEARCH_ULFIUS_R OFF CACHE BOOL "Force to false") # Avoid to search and download ulfius during rhonabwy search and download

set(SEARCH_ORCANIA_I OFF CACHE BOOL "Force to false") # Avoid to search and download orcania during iddawc search and download
set(SEARCH_YDER_I OFF CACHE BOOL "Force to false") # Avoid to search and download yder during iddawc search and download
set(SEARCH_ULFIUS_I OFF CACHE BOOL "Force to false") # Avoid to search and download ulfius during iddawc search and download
set(SEARCH_RHONABWY_I OFF CACHE BOOL "Force to false") # Avoid to search and download rhonabwy during iddawc search and download

option(DOWNLOAD_DEPENDENCIES "Download required dependencies" ON)

set(Orcania_FIND_QUIETLY ON) # force to find Orcania quietly
include(FindOrcania)
find_package(Orcania ${ORCANIA_VERSION_REQUIRED} QUIET) # try to find orcania
if (ORCANIA_FOUND)
	include_directories(${orcania_SOURCE_DIR}/include)
elseif (NOT ORCANIA_FOUND AND DOWNLOAD_DEPENDENCIES)
	include(DownloadProject)
	message(STATUS "Download Orcania ${ORCANIA_VERSION_REQUIRED}")
	download_project(PROJ orcania # ... otherwise, download archive
			URL "https://github.com/babelouest/orcania/archive/v${ORCANIA_VERSION_REQUIRED}.tar.gz"
			QUIET)
	add_subdirectory(${orcania_SOURCE_DIR} ${orcania_BINARY_DIR})
	include_directories(${orcania_SOURCE_DIR}/include)
	add_dependencies(angharad orcania)
	set(ORCANIA_LIBRARIES orcania)
	include_directories(${orcania_BINARY_DIR})
else ()
	message( FATAL_ERROR "Orcania not found")
endif ()
set(LIBS ${LIBS} ${ORCANIA_LIBRARIES})

set(Yder_FIND_QUIETLY ON) # force to find Yder quietly
include(FindYder)
find_package(Yder ${YDER_VERSION_REQUIRED} QUIET) # try to find Yder
if (YDER_FOUND)
	include_directories(${yder_SOURCE_DIR}/include)
elseif (NOT YDER_FOUND AND DOWNLOAD_DEPENDENCIES)
	include(DownloadProject)
	message(STATUS "Download Yder ${YDER_VERSION_REQUIRED}")
	option(CHECK_ORCANIA "specific param" off)
	download_project(PROJ yder # ... otherwise, download archive
			URL "https://github.com/babelouest/yder/archive/v${YDER_VERSION_REQUIRED}.tar.gz"
			QUIET)
	add_subdirectory(${yder_SOURCE_DIR} ${yder_BINARY_DIR})
	include_directories(${yder_SOURCE_DIR}/include)
	include_directories(${orcania_SOURCE_DIR}/include)
	add_dependencies(angharad yder)
	add_dependencies(yder orcania)
	set(YDER_LIBRARIES yder)
	include_directories(${yder_BINARY_DIR})
else ()
	message( FATAL_ERROR "Yder not found")
endif ()
set(LIBS ${LIBS} ${YDER_LIBRARIES})

set(Ulfius_FIND_QUIETLY ON)
include(FindUlfius)
find_package(Ulfius ${ULFIUS_VERSION_REQUIRED} QUIET)
if (ULFIUS_FOUND)
	include_directories(${ulfius_SOURCE_DIR}/include)
elseif (NOT ULFIUS_FOUND AND DOWNLOAD_DEPENDENCIES)
	include(DownloadProject)
	option(WITH_WEBSOCKET "specific param" off)
	option(INSTALL_HEADER "specific param" off)
	option(CHECK_YDER "specific param" off)
	message(STATUS "Download Ulfius ${ULFIUS_VERSION_REQUIRED}")
	download_project(PROJ ulfius
			URL "https://github.com/babelouest/ulfius/archive/v${ULFIUS_VERSION_REQUIRED}.tar.gz"
			QUIET)
	add_subdirectory(${ulfius_SOURCE_DIR} ${ulfius_BINARY_DIR})
	include_directories(${yder_SOURCE_DIR}/include)
	include_directories(${orcania_SOURCE_DIR}/include)
	include_directories(${ulfius_SOURCE_DIR}/include)
	add_dependencies(angharad ulfius)
	add_dependencies(ulfius yder)
	add_dependencies(ulfius orcania)
	set(ULFIUS_LIBRARIES ulfius)
	include_directories(${ulfius_BINARY_DIR})
else ()
	message( FATAL_ERROR "Ulfius not found")
endif ()
set(LIBS ${LIBS} ${ULFIUS_LIBRARIES})

set(Hoel_FIND_QUIETLY ON)
include(FindHoel)
find_package(Hoel ${HOEL_VERSION_REQUIRED} QUIET)
if (HOEL_FOUND)
	include_directories(${hoel_SOURCE_DIR}/include)
elseif (NOT HOEL_FOUND AND DOWNLOAD_DEPENDENCIES)
	include(DownloadProject)
	option(INSTALL_HEADER "specific param" off)
	option(CHECK_YDER "specific param" off)
	message(STATUS "Download Hoel ${HOEL_VERSION_REQUIRED}")
	download_project(PROJ hoel
			URL "https://github.com/babelouest/hoel/archive/v${HOEL_VERSION_REQUIRED}.tar.gz"
			QUIET)
	add_subdirectory(${hoel_SOURCE_DIR} ${hoel_BINARY_DIR})
	include_directories(${hoel_SOURCE_DIR}/include)
	add_dependencies(angharad hoel)
	add_dependencies(hoel yder)
	add_dependencies(hoel orcania)
	set(HOEL_LIBRARIES hoel)
	include_directories(${hoel_BINARY_DIR})
else ()
	message( FATAL_ERROR "Hoel not found")
endif ()
set(LIBS ${LIBS} ${HOEL_LIBRARIES})

if (DISABLE_OAUTH2)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISABLE_OAUTH2")
else ()
	set(Rhonabwy_FIND_QUIETLY ON) # force to find Rhonabwy quietly
	include(FindRhonabwy)
	find_package(Rhonabwy ${RHONABWY_VERSION_REQUIRED} QUIET) # try to find rhonabwy
	if (RHONABWY_FOUND)
		include_directories(${rhonabwy_SOURCE_DIR}/include)
	elseif (NOT RHONABWY_FOUND AND DOWNLOAD_DEPENDENCIES)
		include(DownloadProject)
		message(STATUS "Download Rhonabwy ${RHONABWY_VERSION_REQUIRED}")
		download_project(PROJ rhonabwy # ... otherwise, download archive
				URL "https://github.com/babelouest/rhonabwy/archive/v${RHONABWY_VERSION_REQUIRED}.tar.gz"
				QUIET)
		add_subdirectory(${rhonabwy_SOURCE_DIR} ${rhonabwy_BINARY_DIR})
		include_directories(${rhonabwy_SOURCE_DIR}/include)
		set(RHONABWY_LIBRARIES rhonabwy)
		include_directories(${rhonabwy_BINARY_DIR})
		add_dependencies(rhonabwy orcania)
		add_dependencies(rhonabwy yder)
		add_dependencies(rhonabwy ulfius)
		add_dependencies(angharad rhonabwy)
		set (RHONABWY_FOUND ON)
	else ()
		message( FATAL_ERROR "Rhonabwy not found")
	endif ()
	set(LIBS ${LIBS} ${RHONABWY_LIBRARIES})

	set(Iddawc_FIND_QUIETLY ON) # force to find Iddawc quietly
	include(FindIddawc)
	find_package(Iddawc ${IDDAWC_VERSION_REQUIRED} QUIET) # try to find iddawc
	if (IDDAWC_FOUND)
		include_directories(${iddawc_SOURCE_DIR}/include)
	elseif (NOT IDDAWC_FOUND AND DOWNLOAD_DEPENDENCIES)
		include(DownloadProject)
		message(STATUS "Download Iddawc ${IDDAWC_VERSION_REQUIRED}")
		download_project(PROJ iddawc # ... otherwise, download archive
				URL "https://github.com/babelouest/iddawc/archive/v${IDDAWC_VERSION_REQUIRED}.tar.gz"
				QUIET)
		add_subdirectory(${iddawc_SOURCE_DIR} ${iddawc_BINARY_DIR})
		include_directories(${iddawc_SOURCE_DIR}/include)
		set(IDDAWC_LIBRARIES iddawc)
		include_directories(${iddawc_BINARY_DIR})
		set (IDDAWC_FOUND ON)
		add_dependencies(iddawc orcania)
		add_dependencies(iddawc yder)
		add_dependencies(iddawc ulfius)
		add_dependencies(iddawc rhonabwy)
		add_dependencies(angharad iddawc)
	else ()
		message( FATAL_ERROR "Iddawc not found")
	endif ()
	set(LIBS ${LIBS} ${IDDAWC_LIBRARIES})
endif ()

target_link_libraries(angharad ${LIBS} pthread m dl)

# benoic device modules

option(WITH_MOCKDEV "Build Mock device" off)
if (WITH_MOCKDEV)
		set(MOCK_LIB_SRC ${BENOIC_MODULES_SRC_PATH}/device-mock.c)

		set(MOCK_LIBS "-ldl")

		add_library(devmock MODULE ${MOCK_LIB_SRC})
		set_target_properties(devmock PROPERTIES
						COMPILE_OPTIONS -Wextra)
		target_link_libraries(devmock ${MOCK_LIBS})
		set(BENOIC_DEVICES ${BENOIC_DEVICES} devmock)
endif ()

option(WITH_ZWAVE "Build ZWave device" on)
if (WITH_ZWAVE)
		set(ZWAVE_LIB_SRC ${BENOIC_MODULES_SRC_PATH}/device-zwave.cpp)

		include(FindOpenZWAVE)
		find_package(OpenZWAVE REQUIRED)
		if (OPENZWAVE_FOUND)
				set(ZWAVE_LIBS ${OPENZWAVE_LIBRARIES} "-ldl -Wl,-R '-Wl,-R$$ORIGIN'")
				include_directories(${OPENZWAVE_INCLUDE_DIRS})
		endif ()

		add_library(devzwave MODULE ${ZWAVE_LIB_SRC})
		set_target_properties(devzwave PROPERTIES
						COMPILE_OPTIONS -Wextra)
		target_link_libraries(devzwave ${ZWAVE_LIBS})
		set(BENOIC_DEVICES ${BENOIC_DEVICES} devzwave)
endif ()

option(WITH_TAULAS "Build Taulas device" on)
if (WITH_TAULAS)
		set(TAULAS_LIB_SRC ${BENOIC_MODULES_SRC_PATH}/device-taulas.c)

		set(TAULAS_LIBS "-ldl")

		add_library(devtaulas MODULE ${TAULAS_LIB_SRC})
		set_target_properties(devtaulas PROPERTIES
						COMPILE_OPTIONS -Wextra)
		target_link_libraries(devtaulas ${TAULAS_LIBS})
		set(BENOIC_DEVICES ${BENOIC_DEVICES} devtaulas)
endif ()

# carleon services modules

option(WITH_MOCKSERVICE "Build Mock service" off)
if (WITH_MOCKSERVICE)
		set(MOCK_LIB_SRC ${CARLEON_MODULES_SRC_PATH}/service-mock.c)

		set(MOCK_LIBS "-ldl")

		add_library(servicemock MODULE ${MOCK_LIB_SRC})
		set_target_properties(servicemock PROPERTIES
						COMPILE_OPTIONS -Wextra)
		target_link_libraries(servicemock ${MOCK_LIBS})
		set(CARLEON_SERVICES ${CARLEON_SERVICES} servicemock)
endif ()

option(WITH_MOTION "Build Motion service" on)
if (WITH_MOTION)
		set(MOTION_LIB_SRC ${CARLEON_MODULES_SRC_PATH}/service-motion.c)

		set(MOTION_LIBS "-ldl")

		add_library(servicemotion MODULE ${MOTION_LIB_SRC})
		set_target_properties(servicemotion PROPERTIES
						COMPILE_OPTIONS -Wextra)
		target_link_libraries(servicemotion ${MOTION_LIBS})
		set(CARLEON_SERVICES ${CARLEON_SERVICES} servicemotion)
endif ()

option(WITH_LIQUIDSOAP "Build Liquidsoap service" on)
if (WITH_LIQUIDSOAP)
		set(LIQUIDSOAP_LIB_SRC ${CARLEON_MODULES_SRC_PATH}/service-liquidsoap.c)

		set(LIQUIDSOAP_LIBS "-ldl")

		add_library(serviceliquidsoap MODULE ${LIQUIDSOAP_LIB_SRC})
		set_target_properties(serviceliquidsoap PROPERTIES
						COMPILE_OPTIONS -Wextra)
		target_link_libraries(serviceliquidsoap ${LIQUIDSOAP_LIBS})
		set(CARLEON_SERVICES ${CARLEON_SERVICES} serviceliquidsoap)
endif ()

option(WITH_MPD "Build MPD service" on)
if (WITH_MPD)
		set(MPD_LIB_SRC ${CARLEON_MODULES_SRC_PATH}/service-mpd.c)

		include(FindLibMPDClient)
		find_package(LibMPDClient REQUIRED)
		if (LIBMPDCLIENT_FOUND)
				set(LIBMPDCLIENT_LIBRARIES ${LIBMPDCLIENT_LIBRARIES} "-ldl")
				include_directories(${LIBMPDCLIENT_INCLUDE_DIRS})
		endif ()

		add_library(servicempd MODULE ${MPD_LIB_SRC})
		set_target_properties(servicempd PROPERTIES
						COMPILE_OPTIONS -Wextra)
		target_link_libraries(servicempd ${LIBMPDCLIENT_LIBRARIES})
		set(CARLEON_SERVICES ${CARLEON_SERVICES} servicempd)
endif ()

# tests

option(BUILD_TESTING "Build the testing tree." OFF)

if (BUILD_TESTING)
		include(FindCheck)
		find_package(Check REQUIRED)
		if (CHECK_FOUND)
				include(FindSubunit)
				find_package(Subunit REQUIRED)

				enable_testing()

				set(CMAKE_CTEST_COMMAND ctest -V)

				set(TST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
				set(LIBS ${LIBS} ${CHECK_LIBRARIES} ulfius yder orcania)
				find_package(Threads REQUIRED)
				set(LIBS ${LIBS} ${SUBUNIT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} m rt)

				set(TESTS angharad_submodule
									angharad_script
									angharad_scheduler
									angharad_trigger
									angharad_scheduler_trigger_exec
									angharad_profile
									benoic_device
									benoic_device_element
									benoic_monitor
									carleon_service
									carleon_liquidsoap
									carleon_motion
									carleon_mpd
									gareth_alert_smtp
									gareth_alert_http
									gareth_filter
									gareth_message
									gareth_trigger)
						

				configure_file(
								"${CMAKE_MODULE_PATH}/CTestCustom.cmake.in"
								"${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake"
								@ONLY)

				foreach (t ${TESTS})
						add_executable(${t} EXCLUDE_FROM_ALL ${TST_DIR}/${t}.c ${TST_DIR}/unit-tests.c ${TST_DIR}/unit-tests.h)
						target_include_directories(${t} PUBLIC ${TST_DIR})
						target_link_libraries(${t} PUBLIC ${LIBS})
						add_test(NAME ${t}
										WORKING_DIRECTORY ${TST_DIR}
										COMMAND ${t})
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
						"${CMAKE_MODULE_PATH}/CMakeUninstall.cmake.in"
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
