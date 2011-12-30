#=============================================================================
# Copyright 2011, Ivan Busquets.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
#     * Neither the name of Ivan Busquets nor the names of any
#       other contributors to this software may be used to endorse or
#       promote products derived from this software without specific prior
#       written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#=============================================================================
#
# The module defines the following variables:
#	NUKE_INCLUDE_DIR - path to Nuke headers
#	NUKE_LIBRARY_DIR - path to Nuke libs
#       NUKE_FOUND       - true if the Nuke install directory was found
#
# Example usage:
#   find_package(NUKE)
#   if(NUKE_FOUND)
#     message("Nuke found: ${NUKE_LIBRARY_DIR}")
#   endif()
#
#  Search paths:
#  To help the module find a suitable Nuke install, you must either set a
#  NUKE_NDK_DIR environment variable, or set NUKE_DIR in the main CMakeLists.txt.     
# 
#  You can also pass the NUKE_DIR variable when running cmake.
#  Example: 
#   > cmake -D NUKE_DIR=/usr/local/Nuke6.3v4
#
#=============================================================================

message("-- Searching Nuke libraries.......")

# Find Nuke lib dir
find_library ( DDIMAGE_LIBRARY DDImage
              ${NUKE_DIR}
  	      $ENV{NUKE_NDK_DIR}
  	     )

get_filename_component ( NUKE_LIBRARY_DIR ${DDIMAGE_LIBRARY} PATH )

# Find Nuke include dir
find_path ( NUKE_INCLUDE_DIR DDImage/Op.h
            ${NUKE_LIBRARY_DIR}/include
          )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( "Nuke" DEFAULT_MSG
				  DDIMAGE_LIBRARY
                                  NUKE_LIBRARY_DIR
				  NUKE_INCLUDE_DIR								  
				  )

# Get API version numbers
file ( STRINGS ${NUKE_INCLUDE_DIR}/DDImage/ddImageVersionNumbers.h NUKE_VERSION_NUMBERS )
string ( REGEX REPLACE ".*#define kDDImageVersionMajorNum ([0-9]+).*"  "\\1" NUKE_MAJOR_VERSION ${NUKE_VERSION_NUMBERS} )
string ( REGEX REPLACE ".*#define kDDImageVersionMinorNum ([0-9]+).*"  "\\1" NUKE_MINOR_VERSION ${NUKE_VERSION_NUMBERS} )

message(STATUS "Using Nuke API version ${NUKE_MAJOR_VERSION}.${NUKE_MINOR_VERSION}")


if(NOT NUKE_FOUND)
    message(FATAL_ERROR "Try using the -D NUKE_DIR flag =... or set the NUKE_NDK_DIR env variable")
endif()
								