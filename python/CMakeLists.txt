find_package(ROOT REQUIRED COMPONENTS RIO)
include(${ROOT_USE_FILE})
include_directories(../include)
REFLEX_GENERATE_DICTIONARY(G__TTpy ../include/tricktrack/TTPoint.h SELECTION selection.xml) #LINKDEF LinkDef.h)
#---Create a shared library with geneated dictionary
add_library(TTpy SHARED TTPoint.cpp G__TTpy.cxx)
target_link_libraries(TTpy ${ROOT_LIBRARIES})



