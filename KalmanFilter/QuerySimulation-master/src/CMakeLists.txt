add_library(global STATIC global.cpp global.h)
add_library(walkinggraph STATIC walkinggraph.cpp walkinggraph.h)
add_library(particle STATIC particle.cpp particle.h)
add_library(simulator STATIC simulator.cpp simulator.h)

add_library(range_query STATIC range_query.cpp range_query.h)
add_library(nearest_neighbor STATIC nearest_neighbor.cpp nearest_neighbor.h)

add_library(query range_query nearest_neighbor)

add_executable(main main.cpp)

target_link_libraries(main query)
target_link_libraries(main simulator)
target_link_libraries(main particle)
target_link_libraries(main walkinggraph)
target_link_libraries(main global)

target_link_libraries(main CGAL CGAL_Core)
target_link_libraries(main gmp)
target_link_libraries(main boost_system)
target_link_libraries(main boost_random)
target_link_libraries(main boost_thread)
