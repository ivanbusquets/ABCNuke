include_directories ( ${NUKE_INCLUDE_DIR}
					  ${Boost_INCLUDE_DIRS}
					  ${ALEMBIC_INCLUDE_DIR}	  
					  ${OPENEXR_INCLUDE_PATHS}
					  ${ALEMBIC_ILMBASE_INCLUDE_DIRECTORY}
					 )

link_directories    ( ${NUKE_LIBRARY_DIR}
				      ${ALEMBIC_LIBRARY_DIR}
				      #${OPENEXR_LIB_PATH}
				     )

add_library 		( ABCReadGeo SHARED
			  ABCNuke_ArchiveHelper.cpp
			  ABCNuke_Interpolation.cpp
			  ABCNuke_MatrixHelper.cpp
			  ABCNuke_GeoHelper.cpp
		          ABCReadGeo.cpp
				   	 )


set_target_properties ( ABCReadGeo
			PROPERTIES
			PREFIX ""
		        OUTPUT_NAME "ABCReadGeo"
			COMPILE_FLAGS "-g -c -Wall -DUSE_GLEW -fPIC -O3"
			  		   )

target_link_libraries ( ABCReadGeo 
		        ${DDIMAGE_LIBRARY}
		        GLEW
			Iex
			Half
			Imath
                        pthread
			${ALEMBIC_LIBRARIES}
			${HDF5_HL_LIBRARIES}
			${HDF5_LIBRARIES}
                      )


install ( TARGETS ABCReadGeo
          DESTINATION ${CMAKE_BINARY_DIR}/plugins/${NUKE_MAJOR_VERSION}.${NUKE_MINOR_VERSION})
