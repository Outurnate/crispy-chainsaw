file(GLOB materials "${CMAKE_CURRENT_SOURCE_DIR}/*.material")

add_custom_target(resources ALL COMMAND ${CMAKE_COMMAND} -E tar "cfv" "resources.zip" --format=zip ${materials})