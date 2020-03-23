#ifndef __VST_FFMPEG_UTILS_H__
#define __VST_FFMPEG_UTILS_H__

extern "C" {
#include <libavformat/avformat.h>
}

struct VideoMetaData
{
  double avgFrameRate = 0;
  double realFrameRate = 0; // may be 0
  int numFrames = 0; // may be 0
  double duration = 0;
  int rotation = 0; // rotation angle in degrees
  int vidWidth = 0;
  int vidHeight = 0;
};

// result must be freed with: avformat_close_input()
// may return: NULL
AVFormatContext* CreateFormatContextFromBuffer(const uint8_t *buf, int size, int &outErrCode);

// the returned stream is owned by the AVFormatContext
AVStream* GetFirstStreamForType(AVFormatContext *cxt, AVMediaType type);

bool GetVideoMetaData(AVFormatContext *cxt, VideoMetaData &meta);

#endif
