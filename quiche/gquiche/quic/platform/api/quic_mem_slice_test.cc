// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gquiche/quic/platform/api/quic_mem_slice.h"

#include <memory>
#include "gquiche/quic/core/quic_simple_buffer_allocator.h"
#include "gquiche/quic/platform/api/quic_test.h"

namespace quic {
namespace test {
namespace {

class QuicMemSliceTest : public QuicTest {
 public:
  QuicMemSliceTest() {
    size_t length = 1024;
    slice_ = QuicMemSlice(MakeUniqueBuffer(&allocator_, length), length);
    orig_data_ = slice_.data();
    orig_length_ = slice_.length();
  }

  SimpleBufferAllocator allocator_;
  QuicMemSlice slice_;
  const char* orig_data_;
  size_t orig_length_;
};

TEST_F(QuicMemSliceTest, MoveConstruct) {
  QuicMemSlice moved(std::move(slice_));
  EXPECT_EQ(moved.data(), orig_data_);
  EXPECT_EQ(moved.length(), orig_length_);
  EXPECT_EQ(nullptr, slice_.data());
  EXPECT_EQ(0u, slice_.length());
  EXPECT_TRUE(slice_.empty());
}

TEST_F(QuicMemSliceTest, MoveAssign) {
  QuicMemSlice moved;
  moved = std::move(slice_);
  EXPECT_EQ(moved.data(), orig_data_);
  EXPECT_EQ(moved.length(), orig_length_);
  EXPECT_EQ(nullptr, slice_.data());
  EXPECT_EQ(0u, slice_.length());
  EXPECT_TRUE(slice_.empty());
}

TEST_F(QuicMemSliceTest, SliceAllocatedOnHeap) {
  auto buffer = std::make_unique<char[]>(128);
  char* orig_data = buffer.get();
  size_t used_length = 105;
  QuicMemSlice slice = QuicMemSlice(std::move(buffer), used_length);
  QuicMemSlice moved = std::move(slice);
  EXPECT_EQ(moved.data(), orig_data);
  EXPECT_EQ(moved.length(), used_length);
}

}  // namespace
}  // namespace test
}  // namespace quic
