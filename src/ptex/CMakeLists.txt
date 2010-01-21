add_definitions(-DPATH_MAX=256)
include(FindThreads)
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})

set(SRCS 
	PtexCache.cpp 
	PtexFilters.cpp 
	PtexHalf.cpp 
	PtexReader.cpp 
	PtexSeparableFilter.cpp 
	PtexSeparableKernel.cpp 
	PtexTriangleFilter.cpp 
	PtexTriangleKernel.cpp 
	PtexUtils.cpp 
	PtexWriter.cpp)


add_library(Ptex ${SRCS})
target_link_libraries(Ptex ${CMAKE_THREAD_LIBS_INIT} ${ZLIB_LIBRARIES})
install(TARGETS Ptex DESTINATION bin)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/Ptexture.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/PtexHalf.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/PtexInt.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/PtexUtils.h"
  DESTINATION include)
