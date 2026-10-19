add_library(hash_map_lib INTERFACE)
add_library(hash_map::lib ALIAS hash_map_lib)

target_include_directories(hash_map_lib INTERFACE
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>	# for users
)

target_compile_features(hash_map_lib INTERFACE cxx_std_20)

