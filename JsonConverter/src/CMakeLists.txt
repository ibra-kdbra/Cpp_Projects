# header files
set(header_path "${ccdJsonpp_SOURCE_DIR}/include/ccdJsonpp")
set(header 
	${header_path}/JsonPP.h
	${header_path}/Value.h
	)

# source fiels
set(src
	JsonPP.cpp
	Value.cpp
	)

# library target
add_library(ccdJsonpp
	${header}
	${src}
        )

target_include_directories(ccdJsonpp PUBLIC 
	$<BUILD_INTERFACE:${ccdJsonpp_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${include_dest}>
	)

install(EXPORT ccdJsonpp
	DESTINATION ${cmake_dest})

install(TARGETS ccdJsonpp 
	EXPORT ccdJsonpp
	DESTINATION ${lib_dest})

install(FILES ${header} DESTINATION ${include_dest})

