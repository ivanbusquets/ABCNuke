# NVIDIA Texture Tools 2.0 is licensed under the MIT license.

# Copyright (c) 2007 NVIDIA Corporation

# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:

# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.

###############################################################
# Try to find OpenEXR's libraries, and include path.
# Once done this will define:
#
# OPENEXR_FOUND = OpenEXR found. 
# OPENEXR_INCLUDE_PATHS = OpenEXR include directories.
# OPENEXR_LIBRARIES = libraries that are needed to use OpenEXR.
###############################################################

INCLUDE(FindZLIB)


IF(ZLIB_FOUND)

	SET(LIBRARY_PATHS 
		/usr/lib
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		$ENV{PROGRAM_FILES}/OpenEXR/lib/static)

	FIND_PATH(OPENEXR_INCLUDE_PATH ImfRgbaFile.h
		PATH_SUFFIXES OpenEXR
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include)

	FIND_LIBRARY(OPENEXR_HALF_LIBRARY 
		NAMES Half
		PATHS ${LIBRARY_PATHS})
  
	FIND_LIBRARY(OPENEXR_IEX_LIBRARY 
		NAMES Iex
		PATHS ${LIBRARY_PATHS})
 
	FIND_LIBRARY(OPENEXR_IMATH_LIBRARY
		NAMES Imath
		PATHS ${LIBRARY_PATHS})
  
	FIND_LIBRARY(OPENEXR_ILMIMF_LIBRARY
		NAMES IlmImf
		PATHS ${LIBRARY_PATHS})

	FIND_LIBRARY(OPENEXR_ILMTHREAD_LIBRARY
		NAMES IlmThread
		PATHS ${LIBRARY_PATHS})

ENDIF(ZLIB_FOUND)

#MESSAGE(STATUS ${OPENEXR_IMATH_LIBRARY} ${OPENEXR_ILMIMF_LIBRARY} ${OPENEXR_IEX_LIBRARY} ${OPENEXR_HALF_LIBRARY} ${OPENEXR_ILMTHREAD_LIBRARY} ${ZLIB_LIBRARY})

IF (OPENEXR_INCLUDE_PATH AND OPENEXR_IMATH_LIBRARY AND OPENEXR_ILMIMF_LIBRARY AND OPENEXR_IEX_LIBRARY AND OPENEXR_HALF_LIBRARY)
	SET(OPENEXR_FOUND TRUE)
	SET(OPENEXR_INCLUDE_PATHS ${OPENEXR_INCLUDE_PATH} CACHE STRING "The include paths needed to use OpenEXR")
	SET(OPENEXR_LIBRARIES ${OPENEXR_IMATH_LIBRARY} ${OPENEXR_ILMIMF_LIBRARY} ${OPENEXR_IEX_LIBRARY} ${OPENEXR_HALF_LIBRARY} ${OPENEXR_ILMTHREAD_LIBRARY} ${ZLIB_LIBRARY} CACHE STRING "The libraries needed to use OpenEXR")
ENDIF (OPENEXR_INCLUDE_PATH AND OPENEXR_IMATH_LIBRARY AND OPENEXR_ILMIMF_LIBRARY AND OPENEXR_IEX_LIBRARY AND OPENEXR_HALF_LIBRARY)

get_filename_component( OPENEXR_LIB_PATH ${OPENEXR_HALF_LIBRARY} PATH )

IF(OPENEXR_FOUND)
	IF(NOT OPENEXR_FIND_QUIETLY)
		MESSAGE(STATUS "Found OpenEXR: ${OPENEXR_ILMIMF_LIBRARY}")
	ENDIF(NOT OPENEXR_FIND_QUIETLY)
ELSE(OPENEXR_FOUND)
	IF(OPENEXR_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find OpenEXR library")
	ENDIF(OPENEXR_FIND_REQUIRED)
ENDIF(OPENEXR_FOUND)

MARK_AS_ADVANCED(
	OPENEXR_INCLUDE_PATHS
	OPENEXR_LIB_PATH
	OPENEXR_LIBRARIES
	OPENEXR_ILMIMF_LIBRARY
	OPENEXR_IMATH_LIBRARY
	OPENEXR_IEX_LIBRARY
	OPENEXR_HALF_LIBRARY)