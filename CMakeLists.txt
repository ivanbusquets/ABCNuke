cmake_minimum_required(VERSION 2.8)
project(ABCNuke)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_CXX_FLAGS "-g -Wall")

####################################
# ---------- Find NUKE ----------- #  
####################################

find_package(NUKE REQUIRED)

####################################
# ---------- Find Boost ---------- #  
####################################

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
find_package(Boost REQUIRED)

####################################
# ----------- Find HDF5 ---------- #  
####################################

set(HDF5_USE_STATIC_LIBRARIES    ON)
set( HDF5_COMPONENTS 
    CXX
)

find_package(HDF5 REQUIRED)

# Original Find module doesn't seem
# to find hdf5_hl sometimes

if(NOT HDF5_HL_LIBRARIES)
	
	# Is there a better way to find a static lib?
	find_library  (HDF5_HL_LIBRARIES libhdf5_hl.a
		    	   #HINTS ${HDF5_LIBRARY_DIRS}
		    		/usr/lib
		    		/usr/local/lib
					/opt/local/lib
		    		)
endif()

message(${HDF5_HL_LIBRARIES})
					    
####################################
# --------- Find Alembic --------- #
####################################

find_package(ALEMBIC)

####################################
# --------- Find OpenEXR --------- #
####################################

find_package(OpenEXR REQUIRED)

#--------------------------------------------#

# Add our src directory if all packages found

add_subdirectory("src")




