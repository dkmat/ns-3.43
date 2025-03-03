#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/tcp-congestion-ops.h"
#include "ns3/event-id.h"
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab2Part1");

static void
CwndChange(Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() <<"\t"<< newCwnd << std::endl;
}

int
main(int argc, char *argv[])
{
    LogComponentEnable("Lab2Part1", LOG_LEVEL_INFO);
    std::string dataRate = "1Mbps";
    std::string delay = "20ms";
    uint32_t nFlows = 1;
    std::string transport_prot = "TcpNewReno";
    double errorRate = 0.00001;
    CommandLine cmd(__FILE__);
    cmd.AddValue("dataRate", "Rate at which packets are sent", dataRate);
    cmd.AddValue("delay","Propagation delay between links", delay);
    cmd.AddValue("errorRate", "Packet error rate", errorRate);
    cmd.AddValue("nFlows", "Number of flows", nFlows);
    cmd.AddValue("transport_prot", "Transport protocol to use: TcpNewReno or TcpCubic", transport_prot);
    cmd.Parse(argc, argv);
    
    Config::SetDefault("ns3::TcpSocket::InitialCwnd", UintegerValue(1));
    if(transport_prot == "TcpNewReno")
    {
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpNewReno"));
    }
    else if(transport_prot == "TcpCubic")
    {
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpCubic"));
    }
    else
    {
        NS_LOG_ERROR("Invalid transport protocol");
        return 1;
    }

    RngSeedManager::SetSeed(123456789);
    Time::SetResolution(Time::NS);
    Ptr<UniformRandomVariable> startTime = CreateObject<UniformRandomVariable>();
    NodeContainer nodes;
    nodes.Create(4);

    InternetStackHelper stack;
    stack.Install(nodes);
    
    RateErrorModel error_model;
    startTime->SetStream(50);
    error_model.SetRandomVariable(startTime);
    error_model.SetUnit(RateErrorModel::ERROR_UNIT_BYTE);
    error_model.SetRate(errorRate);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("0.01ms"));

    PointToPointHelper bottleneck;
    bottleneck.SetDeviceAttribute("DataRate", StringValue(dataRate));
    bottleneck.SetChannelAttribute("Delay", StringValue(delay));
    bottleneck.SetDeviceAttribute("ReceiveErrorModel", PointerValue(&error_model));
    NetDeviceContainer devices;
    Ipv4AddressHelper address;

    devices = pointToPoint.Install(nodes.Get(0), nodes.Get(1));
    address.SetBase("10.0.0.0","255.255.255.0");
    Ipv4InterfaceContainer n0n1Interface = address.Assign(devices);

    devices = pointToPoint.Install(nodes.Get(2), nodes.Get(3));
    address.SetBase("10.0.2.0","255.255.255.0");
    Ipv4InterfaceContainer n2n3Interface = address.Assign(devices);
    
    

    devices = bottleneck.Install(nodes.Get(1), nodes.Get(2));
    address.SetBase("10.0.1.0", "255.255.255.0");
    Ipv4InterfaceContainer n1n2Interface = address.Assign(devices);
    std::vector<Ptr<PacketSink>> sinks;
    std::vector<Time> startTimes;
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    for (uint32_t i = 0; i < nFlows; i++)
    {
        uint16_t port = 8080 + i;
        Address sinkAddress(InetSocketAddress(n2n3Interface.GetAddress(1), port));
        PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        ApplicationContainer sinkApp = sink.Install(nodes.Get(3));
        sinkApp.Start(Seconds(0.0));
        sinkApp.Stop(Seconds(20.0));
        sinks.push_back(DynamicCast<PacketSink>(sinkApp.Get(0)));
        Ptr<BulkSendApplication> source = CreateObject<BulkSendApplication>();
        source->SetAttribute("Protocol", TypeIdValue(TcpSocketFactory::GetTypeId()));
        source->SetAttribute("Remote", AddressValue(InetSocketAddress(n2n3Interface.GetAddress(1), port)));
        source->SetAttribute("MaxBytes", UintegerValue(0));
        nodes.Get(0)->AddApplication(source);
        Time start = Seconds(startTime->GetValue(1.0, 1.5));
        startTimes.push_back(start);
        source->SetStartTime(start);
        source->SetStopTime(Seconds(20.0));
        
        

        Simulator::Schedule(Seconds(1.5), [source, i, transport_prot](){
            Ptr<Socket> tcpSocket = source->GetSocket();
            AsciiTraceHelper asciiTraceHelper;
            std::string fileName = "cwnd-" + transport_prot + "-flow" + std::to_string(i) + ".txt";
            Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream(fileName);
            if(tcpSocket)
            {
                tcpSocket->TraceConnectWithoutContext("CongestionWindow", MakeBoundCallback(&CwndChange, stream));
            }
            else
            {
                NS_LOG_INFO("Socket not found " << i);
            }
        });
    }
    
    Simulator::Stop(Seconds(20.0));
    Simulator::Run();

    uint64_t totalBps = 0;
    for (uint32_t i = 0; i < sinks.size(); i++)
    {
        uint64_t totalBytes = sinks[i]->GetTotalRx();
        double duration = 20.0 - startTimes[i].GetSeconds();
        totalBps += totalBytes / duration;
    }
    float goodput = totalBps * 8.0 * 1e-6;
    std::cout << "Goodput: "<< std::fixed << std::setprecision(2) << goodput << " Mbps" << std::endl;
    Simulator::Destroy();
    return 0;
}