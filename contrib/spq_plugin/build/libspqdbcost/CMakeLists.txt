# Copyright (c) 2015, Pivotal Software, Inc. All Rights Reserved.

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libspqopt/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libnaucrates/include)
# for the generated config.h file.
include_directories(${PROJECT_BINARY_DIR}/libspqos/include/)

file(GLOB_RECURSE hdrs ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Add headers to make them visible in some IDEs (Clion, VS, Xcode)
list(APPEND srcs ${hdrs})

add_library(spqdbcost ${srcs})

target_link_libraries(spqdbcost
                      ${CMAKE_DL_LIBS}
                      spqos
                      ${XERCES_LIBRARIES})

# Prevent linker errors from undefined symbols on OSX.
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(spqdbcost PROPERTIES
                        LINK_FLAGS "-undefined dynamic_lookup")
endif()
