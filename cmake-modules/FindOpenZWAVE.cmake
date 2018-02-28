#.rst:
# FindOpenZWAVE
# -----------
#
# Find OpenZWAVE
#
# Find OpenZWAVE headers and libraries.
#
# ::
#
#   OPENZWAVE_FOUND          - True if OpenZWAVE found.
#   OPENZWAVE_INCLUDE_DIRS   - Where to find openzwave header files
#   OPENZWAVE_LIBRARIES      - List of libraries when using OpenZWAVE.

#=============================================================================
# Copyright 2018 Silvio Clecio <silvioprog@gmail.com>
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

# Sat Jan 20 12:32:26 -03 2018

find_package(PkgConfig QUIET)
pkg_check_modules(PC_OPENZWAVE QUIET openzwave)

find_path(OPENZWAVE_INCLUDE_DIR
        NAMES openzwave/Manager.h
        HINTS ${PC_OPENZWAVE_INCLUDEDIR} ${PC_OPENZWAVE_INCLUDE_DIRS})

find_library(OPENZWAVE_LIBRARY
        NAMES openzwave libopenzwave
        HINTS ${PC_OPENZWAVE_LIBDIR} ${PC_OPENZWAVE_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenZWAVE
        REQUIRED_VARS OPENZWAVE_LIBRARY OPENZWAVE_INCLUDE_DIR)

if (OPENZWAVE_FOUND)
    set(OPENZWAVE_LIBRARIES ${OPENZWAVE_LIBRARY})
    set(OPENZWAVE_INCLUDE_DIRS ${OPENZWAVE_INCLUDE_DIR}/openzwave/ ${OPENZWAVE_INCLUDE_DIR}/openzwave/command_classes ${OPENZWAVE_INCLUDE_DIR}/openzwave/platform ${OPENZWAVE_INCLUDE_DIR}/openzwave/value_classes)
endif ()

mark_as_advanced(OPENZWAVE_INCLUDE_DIR OPENZWAVE_LIBRARY)