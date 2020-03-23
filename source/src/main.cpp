#include "videoutils.h"


#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>

EMSCRIPTEN_BINDINGS(videoutils) {
  emscripten::function("copyfile", &copyfile);

  emscripten::function("dumpMetaData", &dumpMetaData);
  emscripten::function("readMetaData", &readMetaData);
}

#endif
