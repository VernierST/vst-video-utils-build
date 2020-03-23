#include "ffmpegutils.h"
#include <cstdlib>


namespace
{
  struct buffer_data {
    const uint8_t *ptr;
    size_t size; ///< size left in the buffer
  };


  int read_packet(void *opaque, uint8_t *buf, int buf_size)
  {
    struct buffer_data *bd = (struct buffer_data *)opaque;
    buf_size = FFMIN(buf_size, bd->size);

    // copy internal buffer data to buf
    memcpy(buf, bd->ptr, buf_size);
    bd->ptr  += buf_size;
    bd->size -= buf_size;
    return buf_size;
  }
}


AVFormatContext* CreateFormatContextFromBuffer(const uint8_t *buf, int size, int &outErrCode)
{
  bool success = false;
  AVFormatContext *fmt_ctx = NULL;
  AVIOContext *avio_ctx = NULL;
  uint8_t *buffer = NULL, *avio_ctx_buffer = NULL;
  size_t buffer_size, avio_ctx_buffer_size = 4096;
  int ret = 0;
  struct buffer_data bd = { 0 };

  // fill opaque structure used by the AVIOContext read callback
  bd.ptr  = buf;
  bd.size = size;
  if (!(fmt_ctx = avformat_alloc_context())) {
    ret = AVERROR(ENOMEM);
    goto end;
  }
  avio_ctx_buffer = (unsigned char*)av_malloc(avio_ctx_buffer_size);
  if (!avio_ctx_buffer) {
    ret = AVERROR(ENOMEM);
    goto end;
  }
  avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
                                  0, &bd, &read_packet, NULL, NULL);
  if (!avio_ctx) {
    ret = AVERROR(ENOMEM);
    goto end;
  }
  fmt_ctx->pb = avio_ctx;
  ret = avformat_open_input(&fmt_ctx, NULL, NULL, NULL);
  if (ret < 0) {
    fprintf(stderr, "Could not open input\n");
    goto end;
  }
  ret = avformat_find_stream_info(fmt_ctx, NULL);
  if (ret < 0) {
    fprintf(stderr, "Could not find stream information\n");
    goto end;
  }

  fmt_ctx->pb = nullptr;

end:
  if (ret < 0)
    avformat_close_input(&fmt_ctx);
  // note: the internal buffer could have changed, and be != avio_ctx_buffer
  av_freep(&avio_ctx->buffer);
  av_freep(&avio_ctx);
  if (ret < 0) {
    fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
  }

  outErrCode = ret;

  if (ret < 0) {
    fmt_ctx = nullptr;
  }

  return fmt_ctx;
}


AVStream* GetFirstStreamForType(AVFormatContext *cxt, AVMediaType type)
{
  AVStream *stream = nullptr;

  for (int i = 0; !stream && i < cxt->nb_streams; ++i)
  {
    AVStream *st = cxt->streams[i];
    if (st->codecpar->codec_type == type)
      stream = st;
  }

  return stream;
}

bool GetVideoMetaData(AVFormatContext *cxt, VideoMetaData &meta)
{
  bool valid = false;
  auto *st = GetFirstStreamForType(cxt, AVMEDIA_TYPE_VIDEO);

  if (st)
  {
    int rotation = 0;

    AVDictionaryEntry *entry = nullptr;
    while ( ( entry = av_dict_get(st->metadata, "", entry, AV_DICT_IGNORE_SUFFIX)) )
    {
      if (!strcmp(entry->key, "rotate")) {
        rotation = std::atoi(entry->value);
      }
    }
    // the rotation matrix can also be obtained from:
    // st->side_data[i].type == AV_PKT_DATA_DISPLAYMATRIX, av_display_rotation_get((int32_t *)sd.data)

    meta.avgFrameRate  = av_q2d(st->avg_frame_rate);
    meta.realFrameRate = av_q2d(st->r_frame_rate);
    meta.numFrames     = st->nb_frames;
    meta.duration      = (double)cxt->duration / AV_TIME_BASE;
    meta.rotation      = rotation;
    meta.vidWidth      = st->codecpar->width;
    meta.vidHeight     = st->codecpar->height;
    valid = true;
  }

  return valid;
}
