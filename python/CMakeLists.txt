find_package(ROOT REQUIRED COMPONENTS RIO)
include(${ROOT_USE_FILE})
include_directories(../include)
REFLEX_GENERATE_DICTIONARY(G__TTpy ../include/tricktrack/TTPoint.h SELECTION selection.xml) #LINKDEF LinkDef.h)
#---Create a shared library with geneated dictionary
add_library(TTpy SHARED TTPoint.cpp G__TTpy.cxx)
target_link_libraries(TTpy ${ROOT_LIBRARIES})




find_package(pybind11)

# use git submodules
if(NOT pybind11_FOUND) 
  add_subdirectory(pybind11)
endif()
include_directories(${CMAKE_SOURCE_DIR}/include)
pybind11_add_module(pyTT pyTT.cpp)
install(TARGETS pyTT DESTINATION ${CMAKE_INSTALL_LIBDIR})
