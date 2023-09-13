// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef QUICHE_SPDY_PLATFORM_API_SPDY_STRING_UTILS_H_
#define QUICHE_SPDY_PLATFORM_API_SPDY_STRING_UTILS_H_

#include <string>
#include <utility>

#include "absl/strings/string_view.h"
#include "platform/spdy_platform_impl/spdy_string_utils_impl.h"

namespace spdy {

inline char SpdyHexDigitToInt(char c) {
  return SpdyHexDigitToIntImpl(c);
}


inline bool SpdyHexDecodeToUInt32(absl::string_view data, uint32_t* out) {
  return SpdyHexDecodeToUInt32Impl(data, out);
}

inline std::string SpdyHexDump(absl::string_view data) {
  return SpdyHexDumpImpl(data);
}

}  // namespace spdy

#endif  // QUICHE_SPDY_PLATFORM_API_SPDY_STRING_UTILS_H_
