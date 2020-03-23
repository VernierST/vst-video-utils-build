#!/usr/bin/env bash
# set -x

EMSDK="/Users/distiller/project/emsdk"

PATH+="/Users/distiller/project/emsdk"
PATH+="/Users/distiller/project/emsdk/clang/e1.38.30_64bit"
PATH+="/Users/distiller/project/emsdk/node/8.9.1_64bit/bin"
PATH+="/Users/distiller/project/emsdk/emscripten/1.38.30"

if [[ "$EMSDK" == "" ]]; then
  echo "EMSDK must be set"
  exit 1
fi

. $EMSDK/emsdk_env.sh


######
BUILD_FFMPEG=1
BUILD_MODULE=1
EXTRA_MAKE_ARGS=-j4

MODULE_BUILD_TESTS=OFF
MODULE_BUILD_TYPE=Release

######
BUILDDIR=build
FFMPEG_SRCDIR=${PWD}/ffmpeg-4.2.2
FFMPEG_BUILDDIR=${PWD}/${BUILDDIR}/ffmpeg

# prepare build directories
mkdir -p "$BUILDDIR" 2>/dev/null
mkdir -p "$FFMPEG_BUILDDIR" 2>/dev/null


####################
### Build ffmpeg ###
####################
if [[ $BUILD_FFMPEG -ne 0 ]]; then
  pushd "$FFMPEG_BUILDDIR" >/dev/null

  echo "**************************"
  echo "*** Configuring FFMPEG ***"
  echo "**************************"
  emconfigure "${FFMPEG_SRCDIR}/configure" \
              --prefix="${FFMPEG_BUILDDIR}" \
              --disable-x86asm \
              --disable-asm \
              --disable-inline-asm \
              --disable-network \
              --disable-doc \
              --ar=emar \
              --cc=emcc \
              --cxx=em++ \
              --objcc=emcc \
              --dep-cc=emcc \
              --enable-small \
              --disable-runtime-cpudetect \
              --disable-debug \
              --disable-programs \
              --enable-cross-compile \
              --disable-avdevice \
              --disable-swresample \
              --disable-swscale \
              --disable-postproc \
              --disable-avfilter \
              --disable-pthreads
  if [[ $? -ne 0 ]]; then
    echo "Configure FFMPEG failed"
    exit 1
  fi

  echo "***********************"
  echo "*** Building FFMPEG ***"
  echo "***********************"
  emmake make $EXTRA_MAKE_ARGS install
  if [[ $? -ne 0 ]]; then
    echo "Build FFMPEG failed"
    exit 1
  fi

  popd >/dev/null
fi


#########################
### Build WASM Module ###
#########################
if [[ $BUILD_MODULE -ne 0 ]]; then
  pushd "$BUILDDIR" >/dev/null

  echo "******************************"
  echo "*** Configuring VideoUtils ***"
  echo "******************************"
  emconfigure cmake .. -DCMAKE_BUILD_TYPE=${MODULE_BUILD_TYPE} -DINCLUDE_TESTS=${MODULE_BUILD_TESTS}

  echo "***************************"
  echo "*** Building VideoUtils ***"
  echo "***************************"
  emmake make $EXTRA_MAKE_ARGS install

  popd >/dev/null
fi

exit 0
