cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 11)

add_library(videoutils STATIC
            videoutils.cpp
            ffmpegutils.cpp
            indexeddb.cpp)

set(FFMPEG_LIBS
  avformat
  avcodec
  avutil
)

add_executable(vstvideoutils main.cpp)
target_link_libraries(vstvideoutils videoutils ${FFMPEG_LIBS})


set(DEBUG False)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(DEBUG True)
endif()


if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
  set(WASM 1)

  set(CMAKE_EXECUTABLE_SUFFIX ".js")

  set(POST_JS ${CMAKE_CURRENT_SOURCE_DIR}/messaging.js)

  set(WASM_LINK_FLAGS
    --post-js ${POST_JS}
    --bind
    -s ASSERTIONS
    -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall,cwrap']
    -s SAFE_HEAP=1
    -s WASM=1
    -s NO_EXIT_RUNTIME=1
    -s ALLOW_MEMORY_GROWTH=1
    -s BUILD_AS_WORKER=1
    -s NO_FILESYSTEM=1
    -lm
  )

  if (DEBUG)
    list(APPEND WASM_LINK_FLAGS
      --emrun
      -g
    )
  endif()

  string(REPLACE ";" " " WASM_LINK_FLAGS "${WASM_LINK_FLAGS}")
  set_target_properties(vstvideoutils PROPERTIES LINK_FLAGS "${WASM_LINK_FLAGS}")

  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/VideoUtils.js DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


endif()

