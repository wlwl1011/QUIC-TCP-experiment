#include <string>
#include "gquiche/quic/core/quic_constants.h"
#include "gquiche/quic/core/quic_config.h"
#include "url/gurl.h"

#include "ns3-quic-client.h"
#include "ns3-quic-backend.h"
#include "ns3-quic-alarm-engine.h"
#include "ns3-client-network-helper.h"
#include "ns3-quic-connection-helper.h"
#include "ns3-quic-client-session.h"
namespace quic
{
  static std::string CongestionControlTypeToString(CongestionControlType cc_type)
  {
    switch (cc_type)
    {
    case kCubicBytes:
      return "CUBIC_BYTES";
    case kRenoBytes:
      return "RENO_BYTES";
    case kBBR:
      return "BBR";
    case kPCC:
      return "CUBIC_BYTES";
    case kGoogCC:
      return "BBR";
    case kBBRv2:
      return "BBRv2";
    default:
      return "???";
    }
  }
  Ns3QuicClient::Ns3QuicClient(QuicSession::Visitor *owner,
                               QuicSocketAddress server_address,
                               const QuicServerId &server_id,
                               const ParsedQuicVersionVector &supported_versions,
                               std::unique_ptr<ProofVerifier> proof_verifier,
                               Ns3QuicAlarmEngine *engine,
                               Ns3QuicBackendBase *backend,
                               Ns3QuicPollServer *poller,
                               CongestionControlType cc_type) : Ns3QuicClient(owner, server_address, server_id, supported_versions, QuicConfig(),
                                                                              std::move(proof_verifier), nullptr,
                                                                              engine, backend, poller, cc_type) {}

  Ns3QuicClient::Ns3QuicClient(QuicSession::Visitor *owner,
                               QuicSocketAddress server_address,
                               const QuicServerId &server_id,
                               const ParsedQuicVersionVector &supported_versions,
                               const QuicConfig &config,
                               std::unique_ptr<ProofVerifier> proof_verifier,
                               std::unique_ptr<SessionCache> session_cache,
                               Ns3QuicAlarmEngine *engine,
                               Ns3QuicBackendBase *backend,
                               Ns3QuicPollServer *poller,
                               CongestionControlType cc_type) : QuicClientBase(server_id, supported_versions, config,
                                                                               new Ns3QuicConnectionHelper(QuicAllocatorType::SIMPLE),
                                                                               new Ns3AlarmFactory(engine),
                                                                               std::make_unique<Ns3ClientNetworkHelper>(poller, this),
                                                                               std::move(proof_verifier),
                                                                               std::move(session_cache)),
                                                                backend_(backend),
                                                                owner_(owner),
                                                                cc_type_(cc_type)
  {
    set_server_address(server_address);
  }

  Ns3QuicClient::~Ns3QuicClient() = default;
  void Ns3QuicClient::AsynConnect()
  {
    if (!connected())
    {
      StartConnect();
    }
  }
  Ns3QuicClientSession *Ns3QuicClient::client_session()
  {
    return static_cast<Ns3QuicClientSession *>(QuicClientBase::session());
  }
  bool Ns3QuicClient::GetBandwidth(QuicBandwidth &bandwidth) const
  {
    bool valid = false;
    const QuicSession *session = QuicClientBase::session();
    if (session && session->connection())
    {
      const QuicSentPacketManager &manager = session->connection()->sent_packet_manager();
      bandwidth = manager.BandwidthEstimate();
      // std::cout << "bandwidth" << ' ' << bandwidth << std::endl;
      valid = true;
    }
    return valid;
  }
  bool Ns3QuicClient::GetCongestionWindowBytes(QuicByteCount &bytes) const
  {
    bool valid = false;
    const QuicSession *session = QuicClientBase::session();
    if (session && session->connection())
    {
      const QuicSentPacketManager &manager = session->connection()->sent_packet_manager();
      bytes = manager.GetCongestionWindowInBytes();
      valid = true;
    }
    return valid;
  }
  bool Ns3QuicClient::GetCongestionWindowPackets(int &packets) const
  {
    bool valid = false;
    const QuicSession *session = QuicClientBase::session();
    if (session && session->connection())
    {
      const QuicSentPacketManager &manager = session->connection()->sent_packet_manager();
      packets = manager.GetCongestionWindowInTcpMss();
      valid = true;
    }
    return valid;
  }
  bool Ns3QuicClient::GetInFlightBytes(QuicByteCount &bytes) const
  {
    bool valid = false;
    const QuicSession *session = QuicClientBase::session();
    if (session && session->connection())
    {
      const QuicSentPacketManager &manager = session->connection()->sent_packet_manager();
      bytes = manager.GetBytesInFlight();
      valid = true;
    }
    return valid;
  }
  bool Ns3QuicClient::GetInFlightPackets(int &packets) const
  {
    QuicByteCount bytes = 0;
    bool valid = false;
    valid = GetInFlightBytes(bytes);
    if (valid)
    {
      packets = (bytes + kDefaultTCPMSS - 1) / kDefaultTCPMSS;
    }
    return valid;
  }
  std::string Ns3QuicClient::GetCongestionTypeString() const
  {
    return CongestionControlTypeToString(cc_type_);
  }
  std::unique_ptr<QuicSession> Ns3QuicClient::CreateQuicClientSession(
      const quic::ParsedQuicVersionVector &supported_versions,
      QuicConnection *connection)
  {
    std::unique_ptr<QuicConnection> connection_ptr(connection);
    std::unique_ptr<Ns3QuicClientSession> session(new Ns3QuicClientSession(
        std::move(connection_ptr), owner_, *config(), supported_versions,
        server_id(), crypto_config(), backend_));
    session->connection()->sent_packet_manager().SetSendAlgorithm(cc_type_);
    return session;
  }
  int Ns3QuicClient::GetNumSentClientHellosFromSession()
  {
    return client_session()->GetNumSentClientHellos();
  }
  int Ns3QuicClient::GetNumReceivedServerConfigUpdatesFromSession()
  {
    return client_session()->GetNumReceivedServerConfigUpdates();
  }
  bool Ns3QuicClient::EarlyDataAccepted()
  {
    return client_session()->EarlyDataAccepted();
  }

  bool Ns3QuicClient::ReceivedInchoateReject()
  {
    return client_session()->ReceivedInchoateReject();
  }
  bool Ns3QuicClient::HasActiveRequests()
  {
    return client_session()->HasActiveRequestStreams();
  }

}
