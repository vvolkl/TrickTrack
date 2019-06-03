



add_executable(profiling_kdtree profiling_kdtree.cpp)
target_link_libraries(profiling_kdtree tricktrack)

add_executable(profiling_cm profiling_cm.cpp)
target_link_libraries(profiling_cm tricktrack)

add_executable(profiling_fitter profiling_fitter.cpp)
target_link_libraries(profiling_fitter tricktrack)



