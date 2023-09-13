// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef QUICHE_HTTP2_PLATFORM_API_HTTP2_BUG_TRACKER_H_
#define QUICHE_HTTP2_PLATFORM_API_HTTP2_BUG_TRACKER_H_

#include "platform/http2_platform_impl/http2_bug_tracker_impl.h"

#define HTTP2_BUG(x) HTTP2_BUG_IMPL(x)
#define HTTP2_BUG_IF HTTP2_BUG_IF_IMPL

// V2 macros are the same as all the HTTP2_BUG flavor above, but they take a
// bug_id parameter.
#define HTTP2_BUG_V2 HTTP2_BUG_V2_IMPL
#define HTTP2_BUG_IF_V2 HTTP2_BUG_IF_V2_IMPL

#define FLAGS_http2_always_log_bugs_for_tests \
  FLAGS_http2_always_log_bugs_for_tests_IMPL

#endif  // QUICHE_HTTP2_PLATFORM_API_HTTP2_BUG_TRACKER_H_
