#pragma once
#include <memory>
#include "gquiche/quic/core/quic_types.h"
#include "gquiche/quic/tools/quic_client_base.h"
#include "gquiche/quic/core/quic_bandwidth.h"
namespace quic{
class Ns3QuicBackendBase;
class Ns3QuicPollServer;
class Ns3QuicAlarmEngine;
class Ns3QuicClientSession;
class Ns3QuicClient:public QuicClientBase{
public:
    Ns3QuicClient(QuicSession::Visitor *owner,
                QuicSocketAddress server_address,
                const QuicServerId& server_id,
                const ParsedQuicVersionVector& supported_versions,
                std::unique_ptr<ProofVerifier> proof_verifier,
                Ns3QuicAlarmEngine *engine,
                Ns3QuicBackendBase *backend,
                Ns3QuicPollServer *poller,
                CongestionControlType cc_type);
    Ns3QuicClient(QuicSession::Visitor *owner,
                QuicSocketAddress server_address,
                const QuicServerId& server_id,
                const ParsedQuicVersionVector& supported_versions,
                const QuicConfig& config,
                std::unique_ptr<ProofVerifier> proof_verifier,
                std::unique_ptr<SessionCache> session_cache,
                Ns3QuicAlarmEngine *engine,
                Ns3QuicBackendBase *backend,
                Ns3QuicPollServer *poller,
                CongestionControlType cc_type);
    ~Ns3QuicClient() override;
    void AsynConnect();
    Ns3QuicClientSession* client_session();
    bool GetBandwidth(QuicBandwidth &bandwidth) const;
    bool GetCongestionWindowBytes(QuicByteCount &bytes) const;
    bool GetCongestionWindowPackets(int &packets) const;
    bool GetInFlightBytes(QuicByteCount& bytes) const;
    bool GetInFlightPackets(int & packets) const;
    std::string GetCongestionTypeString() const;
protected:
    //from QuicClientBase
    // Takes ownership of |connection|.
    std::unique_ptr<QuicSession> CreateQuicClientSession(
        const quic::ParsedQuicVersionVector& supported_versions,
        QuicConnection* connection) override;  

    int GetNumSentClientHellosFromSession() override;
    int GetNumReceivedServerConfigUpdatesFromSession() override;
    
    // This client does not resend saved data. This will be a no-op.
    void ResendSavedData() override{}
    
    // This client does not resend saved data. This will be a no-op.
    void ClearDataToResend() override{}
  
    bool EarlyDataAccepted() override;
    bool ReceivedInchoateReject() override;
    bool HasActiveRequests() override;
private:
    Ns3QuicBackendBase *backend_=nullptr;
    QuicSession::Visitor *owner_=nullptr;
    CongestionControlType cc_type_;
};
}
