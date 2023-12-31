#include "gquiche/http2/adapter/nghttp2_session.h"

#include "gquiche/common/platform/api/quiche_logging.h"

namespace http2 {
namespace adapter {
namespace {

void DeleteSession(nghttp2_session* session) {
  nghttp2_session_del(session);
}

void DeleteOptions(nghttp2_option* options) {
  nghttp2_option_del(options);
}

}  // namespace

NgHttp2Session::NgHttp2Session(Perspective perspective,
                               nghttp2_session_callbacks* callbacks,
                               nghttp2_option* options,
                               void* userdata)
    : session_(nullptr, DeleteSession),
      options_(options, DeleteOptions),
      perspective_(perspective) {
  nghttp2_session* session;
  switch (perspective) {
    case Perspective::kClient:
      nghttp2_session_client_new2(&session, callbacks, userdata,
                                  options_.get());
      break;
    case Perspective::kServer:
      nghttp2_session_server_new2(&session, callbacks, userdata,
                                  options_.get());
      break;
  }
  nghttp2_session_callbacks_del(callbacks);
  session_.reset(session);
}

NgHttp2Session::~NgHttp2Session() {
  // Can't invoke want_read() or want_write(), as they are virtual methods.
  const bool pending_reads = nghttp2_session_want_read(session_.get()) != 0;
  const bool pending_writes = nghttp2_session_want_write(session_.get()) != 0;
  QUICHE_LOG_IF(WARNING, pending_reads || pending_writes)
      << "Shutting down connection with pending reads: " << pending_reads
      << " or pending writes: " << pending_writes;
}

ssize_t NgHttp2Session::ProcessBytes(absl::string_view bytes) {
  return nghttp2_session_mem_recv(
      session_.get(), reinterpret_cast<const uint8_t*>(bytes.data()),
      bytes.size());
}

int NgHttp2Session::Consume(Http2StreamId stream_id, size_t num_bytes) {
  return nghttp2_session_consume(session_.get(), stream_id, num_bytes);
}

bool NgHttp2Session::want_read() const {
  return nghttp2_session_want_read(session_.get()) != 0;
}

bool NgHttp2Session::want_write() const {
  return nghttp2_session_want_write(session_.get()) != 0;
}

int NgHttp2Session::GetRemoteWindowSize() const {
  return nghttp2_session_get_remote_window_size(session_.get());
}

}  // namespace adapter
}  // namespace http2
