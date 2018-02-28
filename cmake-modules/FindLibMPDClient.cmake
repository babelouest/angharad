#.rst:
# FindLibMPDClient
# -----------
#
# Find LibMPDClient
#
# Find LibMPDClient headers and libraries.
#
# ::
#
#   LIBMPDCLIENT_FOUND          - True if LibMPDClient found.
#   LIBMPDCLIENT_INCLUDE_DIRS   - Where to find mpd/version.h.
#   LIBMPDCLIENT_LIBRARIES      - List of libraries when using LibMPDClient.

#=============================================================================
# Copyright 2018 Nicolas Mora <mail@babelouest.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation;
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU GENERAL PUBLIC LICENSE for more details.
#
# You should have received a copy of the GNU General Public
# License along with this library.	If not, see <http://www.gnu.org/licenses/>.
#=============================================================================

find_package(PkgConfig QUIET)
pkg_check_modules(PC_LIBMPDCLIENT QUIET libmpdclient)

find_path(LIBMPDCLIENT_INCLUDE_DIR
        NAMES mpd/version.h
        HINTS ${PC_LIBMPDCLIENT_INCLUDEDIR} ${PC_LIBMPDCLIENT_INCLUDE_DIRS})

find_library(LIBMPDCLIENT_LIBRARY
        NAMES mpdclient libmpdclient
        HINTS ${PC_LIBMPDCLIENT_LIBDIR} ${PC_LIBMPDCLIENT_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibMPDClient
        REQUIRED_VARS LIBMPDCLIENT_LIBRARY LIBMPDCLIENT_INCLUDE_DIR)

if (LIBMPDCLIENT_FOUND)
    set(LIBMPDCLIENT_LIBRARIES ${LIBMPDCLIENT_LIBRARY})
    set(LIBMPDCLIENT_INCLUDE_DIRS ${LIBMPDCLIENT_INCLUDE_DIR}/mpd)
endif ()

mark_as_advanced(LIBMPDCLIENT_INCLUDE_DIR LIBMPDCLIENT_LIBRARY)