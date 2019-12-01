find_file(SHADERC_PATH  NAMES shaderc  PATHS ${_VCPKG_ROOT_DIR}/packages/bgfx_${VCPKG_TARGET_TRIPLET}/bin NO_DEFAULT_PATH)
find_file(TEXTUREC_PATH NAMES texturec PATHS ${_VCPKG_ROOT_DIR}/packages/bgfx_${VCPKG_TARGET_TRIPLET}/bin NO_DEFAULT_PATH)

function(compile_shader SRC DEST TYPE SHADER_PLATFORM SHADER_PROFILE)
  if (${SHADER_PROFILE} STREQUAL "glsl")
    set(PROFILE_ARGS "")
  else()
    set(PROFILE_ARGS --profile ${SHADER_PROFILE})
  endif()
  add_custom_command(TARGET assets
                     COMMAND "${SHADERC_PATH}"
                     ARGS -f "${SRC}"
                          -o "${DEST}"
                          -i "${PROJECT_SOURCE_DIR}/assets/shaders"
                          --type ${TYPE}
                          --varyingdef "${PROJECT_SOURCE_DIR}/assets/shaders/varying.def.sc"
                          --platform ${SHADER_PLATFORM}
                          ${PROFILE_ARGS})
endfunction()

function(compile_texture SRC DEST)
  add_custom_command(TARGET assets
                     COMMAND "${TEXTUREC_PATH}"
                     ARGS -f "${SRC}"
                          -o "${DEST}")
endfunction()

add_custom_target(assets ALL)

file(GLOB FRAGMENT_SHADER_SOURCES "${PROJECT_SOURCE_DIR}/assets/shaders/fs_*.sc")
file(GLOB   VERTEX_SHADER_SOURCES "${PROJECT_SOURCE_DIR}/assets/shaders/vs_*.sc")
file(GLOB         TEXTURE_SOURCES "${PROJECT_SOURCE_DIR}/assets/textures/*.png")

file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/assets/textures/")
foreach(TEXTURE_SOURCE ${TEXTURE_SOURCES})
  get_filename_component(DESTNAME ${TEXTURE_SOURCE} NAME_WE)
  compile_texture(${TEXTURE_SOURCE} "${PROJECT_BINARY_DIR}/assets/textures/${DESTNAME}.ktx")
endforeach()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(SHADER_PLATFORM linux)
  set(SHADER_PROFILES glsl)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(SHADER_PLATFORM windows)
  set(SHADER_PROFILES glsl hlsl spirv)
else()
  message(FATAL_ERROR "Platform ${CMAKE_SYSTEM_NAME} unsupported")
endif()

foreach(SHADER_PROFILE ${SHADER_PROFILES})
  file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/assets/shaders/${SHADER_PROFILE}/")

  foreach(SHADER_SOURCE ${FRAGMENT_SHADER_SOURCES})
    get_filename_component(DESTNAME ${SHADER_SOURCE} NAME_WE)
    compile_shader(${SHADER_SOURCE} "${PROJECT_BINARY_DIR}/assets/shaders/${SHADER_PROFILE}/${DESTNAME}.bin" "f" ${SHADER_PLATFORM} ${SHADER_PROFILE})
  endforeach()

  foreach(SHADER_SOURCE ${VERTEX_SHADER_SOURCES})
    get_filename_component(DESTNAME ${SHADER_SOURCE} NAME_WE)
    compile_shader(${SHADER_SOURCE} "${PROJECT_BINARY_DIR}/assets/shaders/${SHADER_PROFILE}/${DESTNAME}.bin" "v" ${SHADER_PLATFORM} ${SHADER_PROFILE})
  endforeach()
endforeach()