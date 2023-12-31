// Copyright (c) 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gquiche/quic/core/crypto/tls_client_connection.h"

namespace quic {

TlsClientConnection::TlsClientConnection(SSL_CTX* ssl_ctx, Delegate* delegate)
    : TlsConnection(ssl_ctx, delegate->ConnectionDelegate()),
      delegate_(delegate) {}

// static
bssl::UniquePtr<SSL_CTX> TlsClientConnection::CreateSslCtx(
    bool enable_early_data) {
  bssl::UniquePtr<SSL_CTX> ssl_ctx =
      TlsConnection::CreateSslCtx(SSL_VERIFY_PEER);
  // Configure certificate verification.
  int reverify_on_resume_enabled = 1;
  SSL_CTX_set_reverify_on_resume(ssl_ctx.get(), reverify_on_resume_enabled);

  // Configure session caching.
  SSL_CTX_set_session_cache_mode(
      ssl_ctx.get(), SSL_SESS_CACHE_CLIENT | SSL_SESS_CACHE_NO_INTERNAL);
  SSL_CTX_sess_set_new_cb(ssl_ctx.get(), NewSessionCallback);

  SSL_CTX_set_early_data_enabled(ssl_ctx.get(), enable_early_data);
  return ssl_ctx;
}

// static
int TlsClientConnection::NewSessionCallback(SSL* ssl, SSL_SESSION* session) {
  static_cast<TlsClientConnection*>(ConnectionFromSsl(ssl))
      ->delegate_->InsertSession(bssl::UniquePtr<SSL_SESSION>(session));
  return 1;
}

}  // namespace quic
