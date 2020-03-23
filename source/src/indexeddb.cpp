#include "indexeddb.h"
#include <emscripten.h>


void IDBLoadAsync(const std::string &db,
                  const std::string &filename,
                  IDBLoadFunc onSuccess,
                  IDBErrorFunc onError)
{
  struct IDBLoadContext
  {
    IDBLoadFunc onSuccess;
    IDBErrorFunc onError;
  };

  static auto onSuccessCB = [](void *userdata, void *buf, int size)
  {
    auto *cxt = (IDBLoadContext*)userdata;
    cxt->onSuccess(static_cast<const uint8_t*>(buf), static_cast<size_t>(size));
    delete cxt;
  };

  static auto onErrorCB = [](void *userdata)
  {
    auto *cxt = (IDBLoadContext*)userdata;
    cxt->onError();
    delete cxt;
  };

  auto *cxt = new IDBLoadContext { onSuccess, onError };
  emscripten_idb_async_load (db.c_str(), filename.c_str(), (void*)cxt, onSuccessCB, onErrorCB);
}


void IDBStoreAsync(const std::string &db,
                   const std::string &filename,
                   const uint8_t *buf,
                   size_t size,
                   IDBStoreFunc onSuccess,
                   IDBErrorFunc onError)
{
  struct IDBStoreContext
  {
    IDBStoreFunc onSuccess;
    IDBErrorFunc onError;
  };

  static auto onSuccessCB = [](void *userdata)
  {
    auto *cxt = (IDBStoreContext*)userdata;
    cxt->onSuccess();
    delete cxt;
  };

  static auto onErrorCB = [](void *userdata)
  {
    auto *cxt = (IDBStoreContext*)userdata;
    cxt->onError();
    delete cxt;
  };

  auto *cxt = new IDBStoreContext { onSuccess, onError };
  emscripten_idb_async_store(db.c_str(), filename.c_str(), (void*)buf, (int)size, (void*)cxt, onSuccessCB, onErrorCB);
}

