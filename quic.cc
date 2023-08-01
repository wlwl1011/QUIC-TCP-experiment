#include <iostream>
#include <stdlib.h>
#include <memory>
#include <string>
#include <unistd.h>
#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"
#include "ns3/quic-module.h"
#include "ns3/nstime.h"

using namespace ns3;
using namespace quic;
using namespace std;
NS_LOG_COMPONENT_DEFINE("quic-main");
const uint32_t DEFAULT_PACKET_SIZE = 1500;
const uint32_t kBwUnit = 1000000;
class TriggerRandomLoss
{
public:
    TriggerRandomLoss() {}
    ~TriggerRandomLoss()
    {
        if (m_timer.IsRunning())
        {
            m_timer.Cancel();
        }
    }
    void RegisterDevice(Ptr<NetDevice> dev)
    {
        m_dev = dev;
    }
    void Start()
    {
        Time next = Seconds(2);
        m_timer = Simulator::Schedule(next, &TriggerRandomLoss::ConfigureRandomLoss, this);
    }
    void ConfigureRandomLoss()
    {
        if (m_timer.IsExpired())
        {
            std::string errorModelType = "ns3::RateErrorModel";
            ObjectFactory factory;
            factory.SetTypeId(errorModelType);
            Ptr<ErrorModel> em = factory.Create<ErrorModel>();
            m_dev->SetAttribute("ReceiveErrorModel", PointerValue(em));
            m_timer.Cancel();
        }
    }

private:
    Ptr<NetDevice> m_dev;
    EventId m_timer;
};
struct LinkProperty
{
    uint16_t nodes[2];
    uint32_t bandwidth;
    uint32_t propagation_ms;
};
uint32_t CalMaxRttInDumbbell(LinkProperty *topoinfo, int links)
{
    uint32_t rtt1 = 2 * (topoinfo[0].propagation_ms + topoinfo[1].propagation_ms + topoinfo[2].propagation_ms);
    uint32_t rtt2 = 2 * (topoinfo[1].propagation_ms + topoinfo[3].propagation_ms + topoinfo[4].propagation_ms);
    return std::max<uint32_t>(rtt1, rtt2);
}

/** Network topology
 *       n0            n1
 *        |            |
 *        | l0         | l2
 *        |            |
 *        n2---l1------n3
 *        |            |
 *        |  l3        | l4
 *        |            |
 *        n4           n5
 */

#define DEFAULT_PACKET_SIZE 1500
int ip = 1;
static NodeContainer BuildDumbbellTopo(LinkProperty *topoinfo, int links, int bottleneck_i,
                                       uint32_t buffer_ms, uint32_t delay_integer, TriggerRandomLoss *trigger = nullptr)
{
    int hosts = links + 1;
    NodeContainer topo;
    topo.Create(hosts);
    InternetStackHelper stack;
    stack.Install(topo);
    for (int i = 0; i < links; i++)
    {
        uint16_t src = topoinfo[i].nodes[0];
        uint16_t dst = topoinfo[i].nodes[1];
        uint32_t bps = topoinfo[i].bandwidth;

        NodeContainer nodes = NodeContainer(topo.Get(src), topo.Get(dst));
        auto bufSize = std::max<uint32_t>(DEFAULT_PACKET_SIZE, bps * buffer_ms / 8000);
        int packets = bufSize / DEFAULT_PACKET_SIZE;

        PointToPointHelper pointToPoint;
        pointToPoint.SetDeviceAttribute("DataRate", DataRateValue(DataRate(bps)));
        pointToPoint.SetChannelAttribute("Delay", TimeValue(MilliSeconds(delay_integer)));
        if (bottleneck_i == i)
        {
            pointToPoint.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue(std::to_string(20) + "p"));
        }
        else
        {
            pointToPoint.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue(std::to_string(packets) + "p"));
        }
        NetDeviceContainer devices = pointToPoint.Install(nodes);
        if (bottleneck_i == i)
        {
            TrafficControlHelper pfifoHelper;
            uint16_t handle = pfifoHelper.SetRootQueueDisc("ns3::FifoQueueDisc", "MaxSize", StringValue(std::to_string(packets) + "p"));
            pfifoHelper.AddInternalQueues(handle, 1, "ns3::DropTailQueue", "MaxSize", StringValue(std::to_string(packets) + "p"));
            pfifoHelper.Install(devices);
        }
        Ipv4AddressHelper address;
        std::string nodeip = "10.1." + std::to_string(ip) + ".0";
        ip++;
        address.SetBase(nodeip.c_str(), "255.255.255.0");
        address.Assign(devices);
        if (bottleneck_i == i && trigger)
        {
            trigger->RegisterDevice(devices.Get(1));
        }
    }
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    return topo;
}

void test_arg_config(int argc, char *argv[])
{
    const char *usage = "./quic-main --test_value=1234";
    parser_flags(usage, argc, argv);
    quic::print_test_value();
}
typedef struct
{
    const char *cc_name;
    float start;
    float stop;
} client_config_t;

void test_app_on_dumbbell(std::string &instance, float app_start, float app_stop,
                          uint8_t client_log_flag, uint8_t server_log_flag,
                          quic::BackendType type, TriggerRandomLoss *trigger_loss,
                          const std::string &cc1, const std::string &cc2, uint32_t delay_integer, int index)
{
    uint32_t non_bottleneck_bw = 100 * kBwUnit;
    uint32_t bottleneck_bw = 10 * kBwUnit;
    uint32_t links = 5;
    int bottleneck_i = 1;
    LinkProperty topoinfo1[] = {
        [0] = {0, 2, 0, 10},
        [1] = {2, 3, 0, 10},
        [2] = {3, 1, 0, 10},
        [3] = {2, 4, 0, 10},
        [4] = {3, 5, 0, 10},
    };
    {
        LinkProperty *info_ptr = topoinfo1;
        for (int i = 0; i < links; i++)
        {
            if (bottleneck_i == i)
            {
                info_ptr[i].bandwidth = bottleneck_bw;
            }
            else
            {
                info_ptr[i].bandwidth = non_bottleneck_bw;
            }
        }
    }

    LinkProperty *topoinfo_ptr = nullptr;
    uint32_t buffer_ms = 0;

    if (0 == instance.compare("1"))
    {
        topoinfo_ptr = topoinfo1;
        uint32_t rtt = CalMaxRttInDumbbell(topoinfo_ptr, links);
        buffer_ms = rtt;
    }
    else if (0 == instance.compare("2"))
    {
        topoinfo_ptr = topoinfo1;
        uint32_t rtt = CalMaxRttInDumbbell(topoinfo_ptr, links);
        buffer_ms = 3 * rtt / 2;
    }
    else if (0 == instance.compare("3"))
    {
        topoinfo_ptr = topoinfo1;
        uint32_t rtt = CalMaxRttInDumbbell(topoinfo_ptr, links);
        buffer_ms = 4 * rtt / 2;
    }
    else if (0 == instance.compare("4"))
    {
        topoinfo_ptr = topoinfo1;
        uint32_t rtt = CalMaxRttInDumbbell(topoinfo_ptr, links);
        buffer_ms = 6 * rtt / 2;
    }
    else
    {
        topoinfo_ptr = topoinfo1;
        uint32_t rtt = CalMaxRttInDumbbell(topoinfo_ptr, links);
        buffer_ms = 4 * rtt / 2;
    }
    NodeContainer topo = BuildDumbbellTopo(topoinfo_ptr, links, bottleneck_i, buffer_ms, delay_integer, trigger_loss);

    std::vector<Ns3QuicClientTrace *> client_traces;
    Ns3QuicServerTraceDispatcher *server_trace = nullptr;
    if (server_log_flag & E_QS_ALL)
    {
        server_trace = new Ns3QuicServerTraceDispatcher();
        server_trace->LogEnable(instance, server_log_flag);
    }

    uint16_t server_port = 1234;
    InetSocketAddress server_addr1(server_port);
    InetSocketAddress server_addr2(server_port);

    // install server on h1
    {
        Ptr<Node> host = topo.Get(1);
        Ptr<QuicServerApp> server_app = CreateObject<QuicServerApp>(type);
        host->AddApplication(server_app);
        server_app->Bind(server_port);
        server_addr1 = server_app->GetLocalAddress();
        server_app->SetStartTime(Seconds(app_start));
        server_app->SetStopTime(Seconds(app_stop) + Seconds(10));
        if (server_trace)
        {
            server_app->set_trace(server_trace);
        }
    }

    // install server on h5
    {
        Ptr<Node> host = topo.Get(5);
        Ptr<QuicServerApp> server_app = CreateObject<QuicServerApp>(type);
        host->AddApplication(server_app);
        server_app->Bind(server_port);
        server_addr2 = server_app->GetLocalAddress();
        server_app->SetStartTime(Seconds(app_start));
        server_app->SetStopTime(Seconds(app_stop) + Seconds(10));
        if (server_trace)
        {
            server_app->set_trace(server_trace);
        }
    }
    // client_config_t config1[100];
    // client_config_t config2[100];
    // for (int i = 0; i < index; i++)
    // {
    //     config1[i] = {.cc_name = cc1.c_str(), .start = app_start, .stop = app_stop};
    //     config2[i] = {.cc_name = cc1.c_str(), .start = app_start, .stop = app_stop};
    // }
    // client_config_t config2[100] = {
    //     [0] = {.cc_name = cc2.c_str(), .start = app_start, .stop = app_stop},
    //     [1] = {.cc_name = cc2.c_str(), .start = app_start, .stop = app_stop},
    // };
    Ptr<Node> h0 = topo.Get(0);
    Ptr<Node> h4 = topo.Get(4);
    for (int i = 0; i < index; i++)
    {
        // install client
        Ptr<QuicClientApp> client_app = CreateObject<QuicClientApp>(type, cc1.c_str());
        h0->AddApplication(client_app);
        client_app->Bind();
        InetSocketAddress client_addr = client_app->GetLocalAddress();
        client_app->set_peer(server_addr1.GetIpv4(), server_addr1.GetPort());
        client_app->SetStartTime(Seconds(app_start));
        client_app->SetStopTime(Seconds(app_stop));
        Ns3QuicAddressPair addr_pair(client_addr, server_addr1);
        if (client_log_flag & E_QC_ALL)
        {
            std::string prefix = instance + "_" + Ns3QuicAddressPair2String(addr_pair);
            Ns3QuicClientTrace *trace = new Ns3QuicClientTrace();
            trace->LogEnable(prefix, client_log_flag);
            if (client_log_flag & E_QC_IN_FLIGHT)
            {
                client_app->SetInFlightTraceFun(
                    MakeCallback(&Ns3QuicClientTrace::OnInFlight, trace));
            }
            if (client_log_flag & E_QC_SEND_RATE)
            {
                client_app->SetRateTraceFuc(MakeCallback(&Ns3QuicClientTrace::OnSendRate, trace));
            }
            if (client_log_flag & E_QC_CWND)
            {
                client_app->SetCwndTraceFun(MakeCallback(&Ns3QuicClientTrace::OnCwnd, trace));
            }
            client_traces.push_back(trace);
        }
        if (server_log_flag & E_QS_ALL)
        {
            server_trace->AddMonitorAddressPair(addr_pair);
        }
    }
    for (int i = 0; i < index; i++)
    {
        // install client
        Ptr<QuicClientApp> client_app = CreateObject<QuicClientApp>(type, cc1.c_str());
        h4->AddApplication(client_app);
        client_app->Bind();
        InetSocketAddress client_addr = client_app->GetLocalAddress();
        client_app->set_peer(server_addr2.GetIpv4(), server_addr2.GetPort());
        client_app->SetStartTime(Seconds(app_start));
        client_app->SetStopTime(Seconds(app_stop));
        Ns3QuicAddressPair addr_pair(client_addr, server_addr2);
        if (client_log_flag & E_QC_ALL)
        {
            std::string prefix = instance + "_" + Ns3QuicAddressPair2String(addr_pair);
            Ns3QuicClientTrace *trace = new Ns3QuicClientTrace();
            trace->LogEnable(prefix, client_log_flag);
            if (client_log_flag & E_QC_IN_FLIGHT)
            {
                client_app->SetInFlightTraceFun(
                    MakeCallback(&Ns3QuicClientTrace::OnInFlight, trace));
            }
            if (client_log_flag & E_QC_SEND_RATE)
            {
                client_app->SetRateTraceFuc(MakeCallback(&Ns3QuicClientTrace::OnSendRate, trace));
            }
            if (client_log_flag & E_QC_CWND)
            {
                client_app->SetCwndTraceFun(MakeCallback(&Ns3QuicClientTrace::OnCwnd, trace));
            }
            client_traces.push_back(trace);
        }
        if (server_log_flag & E_QS_ALL)
        {
            server_trace->AddMonitorAddressPair(addr_pair);
        }
    }
    int last_time = WallTimeMillis();
    Simulator::Stop(Seconds(app_stop) + Seconds(20));
    Simulator::Run();
    Simulator::Destroy();
    for (auto it = client_traces.begin(); it != client_traces.end(); it++)
    {
        Ns3QuicClientTrace *ptr = (*it);
        delete ptr;
    }
    client_traces.clear();
    if (server_trace)
    {
        Time last_stamp = server_trace->GetLastReceiptTime();
        Time start = Seconds(app_start);
        int64_t channnel_bite = 0;
        if (last_stamp > start)
        {
            Time duration = last_stamp - start;
            channnel_bite = bottleneck_bw * (duration.GetMilliSeconds() / 1000);
            server_trace->CalculateUtil(channnel_bite);
        }
        delete server_trace;
    }
    int delta = WallTimeMillis() - last_time;
    std::cout << "run time ms: " << delta << std::endl;
}
static const float startTime = 0.001;
static const float simDuration = 200.0;
// ./waf --run "scratch/quic-main --cc1=bbr --cc2=bbr --folder=no"
//./waf --run "scratch/quic-main --topo=dumbbell --cc1=bbr --cc2=bbr --folder=dumbbell"
int main(int argc, char *argv[])
{
    // LogComponentEnable("Ns3QuicBackendBase",LOG_LEVEL_ALL);
    // LogComponentEnable("Ns3QuicAlarmEngine",LOG_LEVEL_ALL);
    // LogComponentEnable("Ns3QuicChannelBase",LOG_LEVEL_ALL);

    LogComponentEnable("QuicClientApp", LOG_LEVEL_ALL);
    // LogComponentEnable("QuicServerApp",LOG_LEVEL_ALL);
    std::string topo("p2p");
    std::string index = std::string("20");
    std::string instance = std::string("1");
    std::string loss_str("0"); // config random loss
    std::string link_delay_str("0");
    std::string cc1("cubic");
    std::string cc2("cubic");
    std::string data_folder("no-one");
    CommandLine cmd;
    cmd.AddValue("topo", "topology", topo);
    cmd.AddValue("it", "instacne", instance);
    cmd.AddValue("i", "index", index);
    cmd.AddValue("folder", "folder name to collect data", data_folder);
    cmd.AddValue("lo", "loss", loss_str); // 10 means the dev will introduce 10/1000 % random loss
    cmd.AddValue("de", "link_delay", link_delay_str);
    cmd.AddValue("cc1", "congestion algorithm1", cc1);
    cmd.AddValue("cc2", "congestion algorithm2", cc2);
    cmd.Parse(argc, argv);
    int loss_integer = std::stoi(loss_str);
    int delay_integer = std::stoi(link_delay_str);
    int index_integer = std::stoi(index);
    double random_loss = loss_integer * 1.0 / 100;
    std::unique_ptr<TriggerRandomLoss> triggerloss = nullptr;
    if (loss_integer > 0)
    {
        Config::SetDefault("ns3::RateErrorModel::ErrorRate", DoubleValue(random_loss));
        Config::SetDefault("ns3::RateErrorModel::ErrorUnit", StringValue("ERROR_UNIT_PACKET"));
        Config::SetDefault("ns3::BurstErrorModel::ErrorRate", DoubleValue(random_loss));
        Config::SetDefault("ns3::BurstErrorModel::BurstSize", StringValue("ns3::UniformRandomVariable[Min=1|Max=3]"));
        triggerloss.reset(new TriggerRandomLoss());
        triggerloss->Start();
    }
    if (0 == cc1.compare("reno") || 0 == cc1.compare("cubic") ||
        0 == cc1.compare("bbr") || 0 == cc1.compare("bbrv2") ||
        0 == cc1.compare("copa") || 0 == cc1.compare("vegas"))
    {
    }
    else
    {
        NS_ASSERT_MSG(0, "please input correct cc1");
    }
    if (0 == cc2.compare("reno") || 0 == cc2.compare("cubic") ||
        0 == cc2.compare("bbr") || 0 == cc2.compare("bbrv2") ||
        0 == cc2.compare("copa") || 0 == cc2.compare("vegas"))
    {
    }
    else
    {
        NS_ASSERT_MSG(0, "please input correct cc2");
    }
    const char *envKey = "QUICHE_SRC_DIR";
    char *envValue = getenv(envKey);
    if (envValue)
    {
        std::cout << envValue << std::endl;
    }
    char buffer[128] = {0};
    if (getcwd(buffer, sizeof(buffer)) != buffer)
    {
        NS_LOG_ERROR("path error");
        return -1;
    }
    std::string ns3_path(buffer, ::strlen(buffer));
    if ('/' != ns3_path.back())
    {
        ns3_path.push_back('/');
    }
    std::string trace_folder = ns3_path + "traces/";
    if (!MakePath(trace_folder))
    {
        std::cout << trace_folder << " is not right" << std::endl;
        return -1;
    }

    std::string log_path = trace_folder + data_folder + "/";
    set_quic_trace_folder(log_path);
    if (!MakePath(log_path))
    {
        std::cout << log_path << " is not right" << std::endl;
        return -1;
    }

    set_quic_log_error_level(); // print quiche lib error
    quic::ContentInit('a');

    std::string quic_cert_path = std::string(envValue) + "utils/data/quic-cert/";
    if (ns3::set_quic_cert_key(quic_cert_path))
    {
        // HELLO_UNIDI
        // HELLO_BIDI
        // BANDWIDTH

        quic::BackendType type = quic::BackendType::BANDWIDTH;
        QuicClientTraceType client_log_flag = E_QC_ALL;
        QuicServerTraceType server_log_flag = E_QS_ALL;
        RegisterExternalCongestionFactory();
        if (0 == topo.compare("dumbbell"))
        {
            test_app_on_dumbbell(instance, startTime, simDuration,
                                 client_log_flag, server_log_flag,
                                 type, triggerloss.get(),
                                 cc1, cc2, delay_integer, index_integer);
        }
    }
    return 0;
}
