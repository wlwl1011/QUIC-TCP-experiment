#include "gquiche/quic/core/ns3-quic-session-base.h"
#include <iostream>
namespace quic{
Ns3QuicSessionBase::Ns3QuicSessionBase(QuicConnection* connection,
                      QuicSession::Visitor* owner,
                    const QuicConfig& config,
                    const ParsedQuicVersionVector& supported_versions,
                    QuicStreamCount num_expected_unidirectional_static_streams)
              :QuicSession(connection,owner,config,
                        supported_versions,num_expected_unidirectional_static_streams){}
void print_string_view(absl::string_view data){
    std::string message(data.data(),data.size());
    std::cout<<message<<std::endl;
}
void test_message(std::string &msg){
    absl::string_view view(msg.data(),msg.size());
    print_string_view(view);
}
}
