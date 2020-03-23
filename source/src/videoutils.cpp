#include "videoutils.h"
#include "ffmpegutils.h"
#include "indexeddb.h"

#include <functional>

#include <cstdio>
#include <vector>

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/parseutils.h>
#include <libavutil/avutil.h>
#include <libavutil/dict.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/file.h>
}


#define sendError(reqId,errmsg) do { \
    EM_ASM({ \
      sendError($0,{ \
        error: errmsg \
      }); \
    }, reqId); \
} while(0)


struct CopyFileCxt
{
  int reqId = 0;
  std::string db;
  std::string src;
  std::string dst;
  std::vector<uint8_t> data;
};


struct ReadFileMetaCtx
{
  int reqId = 0;
  std::string db;
  std::string filename;

  std::function<void(AVFormatContext *cxt)> onSuccess;
  std::function<void()> onError;
};


namespace
{
  void sendResponse(int id)
  {
    EM_ASM({ sendResult($0) }, id);
  }


  void sendResponse(int id, const VideoMetaData &meta)
  {
      EM_ASM({
        sendResult($0, {
          avgFrameRate: $1,
          realFrameRate: $2,
          numFrames: $3,
          duration: $4,
          rotation: $5,
          vidWidth: $6,
          vidHeight: $7,
        });
      },
        id,
        meta.avgFrameRate,
        meta.realFrameRate,
        meta.numFrames,
        meta.duration,
        meta.rotation,
        meta.vidWidth,
        meta.vidHeight
      );
  }


  void onerror(void *userdata)
  {
    auto *cxt = (CopyFileCxt*)userdata;

    sendError(cxt->reqId, 'Failed to read file');

    delete cxt;
  };



  void onload(void *userdata, void *buf, int size)
  {
    auto *cxt = (CopyFileCxt*)userdata;

    cxt->data.assign((uint8_t*)buf, (uint8_t*)buf + size);

    IDBStoreAsync(cxt->db, cxt->dst,
                  cxt->data.data(), cxt->data.size(),
                  [=]() {
                    printf("COPY SUCCESS!\n");
                    EM_ASM({ sendResult($0, {}); }, cxt->reqId);
                    delete cxt;
                  },
                  [=]() {
                    printf("COPY FAILED!\n");
                    sendError(cxt->reqId, 'Failed to write file');
                    delete cxt;
                  });
  }
}


int copyfile(int reqId, std::string db, std::string src, std::string dst)
{
  auto *cxt = new CopyFileCxt;
  cxt->reqId = reqId;
  cxt->db = db;
  cxt->src = src;
  cxt->dst = dst;

  emscripten_idb_async_load (db.c_str(), src.c_str(), (void*)cxt, &onload, &onerror);

  return 0;
}


void dumpMetaData(int reqId, std::string db, std::string filename)
{
  auto onSuccess = [=](const uint8_t *buf, size_t size)
  {
    int result = 0;
    AVFormatContext *ic = CreateFormatContextFromBuffer((const uint8_t*)buf, size, result);

    if (0 == result && ic)
    {
      auto name = db + ':' + filename;
      av_dump_format(ic, 0, name.c_str(), 0);
      avformat_close_input(&ic);
      sendResponse(reqId);
    }
    else
      sendError(reqId, 'Failed to read video file');
  };

  auto onError = [=]()
  {
    sendError(reqId, 'Failed to load file');
  };

  IDBLoadAsync(db, filename, onSuccess, onError);
}



void readMetaData(int reqId, std::string db, std::string filename)
{
  auto onSuccess = [=](const uint8_t *buf, size_t size)
  {
    int result = 0;
    AVFormatContext *ic = CreateFormatContextFromBuffer((const uint8_t*)buf, size, result);

    if (0 == result && ic)
    {
      VideoMetaData meta;
      bool success = GetVideoMetaData(ic, meta);
      if (success)
        sendResponse(reqId, meta);
      else
        sendError(reqId, 'Failed to read file metadata');

      avformat_close_input(&ic);
    }
    else
      sendError(reqId, 'Failed to read video file');
  };

  auto onError = [=]()
  {
    sendError(reqId, 'Failed to load file');
  };

  IDBLoadAsync(db, filename, onSuccess, onError);
}
