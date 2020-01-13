file(GLOB resources "${CMAKE_CURRENT_SOURCE_DIR}/materials/*" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/rtss/*")

foreach(RESOURCE ${resources})
  get_filename_component(FNAME ${RESOURCE} NAME)
  configure_file(${RESOURCE} "${CMAKE_CURRENT_BINARY_DIR}/${FNAME}" COPYONLY)
endforeach()

file(GLOB resources_flattened "${CMAKE_CURRENT_BINARY_DIR}/*.glsl" "${CMAKE_CURRENT_BINARY_DIR}/*.material")

add_custom_target(resources ALL
  COMMAND ${CMAKE_COMMAND} -E tar "cfv" "resources.zip" --format=zip ${resources_flattened}
  WORKING_DIRECTORY "${CMAKE_PROJECT_BINARY_DIR}")
