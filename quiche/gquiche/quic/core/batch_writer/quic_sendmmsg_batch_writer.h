// Copyright (c) 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef QUICHE_QUIC_PLATFORM_IMPL_QUIC_SENDMMSG_BATCH_WRITER_H_
#define QUICHE_QUIC_PLATFORM_IMPL_QUIC_SENDMMSG_BATCH_WRITER_H_

#include "gquiche/quic/core/batch_writer/quic_batch_writer_base.h"
#include "gquiche/quic/core/quic_linux_socket_utils.h"

namespace quic {

class QUIC_EXPORT_PRIVATE QuicSendmmsgBatchWriter : public QuicUdpBatchWriter {
 public:
  QuicSendmmsgBatchWriter(std::unique_ptr<QuicBatchWriterBuffer> batch_buffer,
                          int fd);

  CanBatchResult CanBatch(const char* buffer,
                          size_t buf_len,
                          const QuicIpAddress& self_address,
                          const QuicSocketAddress& peer_address,
                          const PerPacketOptions* options,
                          uint64_t release_time) const override;

  FlushImplResult FlushImpl() override;

 protected:
  using CmsgBuilder = QuicMMsgHdr::ControlBufferInitializer;
  FlushImplResult InternalFlushImpl(size_t cmsg_space,
                                    const CmsgBuilder& cmsg_builder);
};

}  // namespace quic

#endif  // QUICHE_QUIC_PLATFORM_IMPL_QUIC_SENDMMSG_BATCH_WRITER_H_
