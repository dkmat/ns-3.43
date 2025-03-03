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

NS_LOG_COMPONENT_DEFINE("Lab2Part2");

static void
CwndChange(Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() <<"\t"<< newCwnd << std::endl;
}

int
main(int argc, char *argv[])
{
    LogComponentEnable("Lab2Part2", LOG_LEVEL_INFO);
    uint32_t nFlows = 1;
    std::string transport_prot = "TcpNewReno";
    double errorRate = 0.00001;
    uint32_t run = 1;
    CommandLine cmd(__FILE__);
    cmd.AddValue("nFlows", "Number of flows", nFlows);
    cmd.AddValue("transport_prot", "Transport protocol to use: TcpNewReno or TcpCubic", transport_prot);
    cmd.AddValue("run", "Run number", run);
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
    RngSeedManager::SetRun(run);
    Time::SetResolution(Time::NS);
    Ptr<UniformRandomVariable> startTime = CreateObject<UniformRandomVariable>();
    NodeContainer nodes;
    nodes.Create(6);

    InternetStackHelper stack;
    stack.Install(nodes);
    
    RateErrorModel error_model;
    startTime->SetStream(50);
    error_model.SetRandomVariable(startTime);
    error_model.SetUnit(RateErrorModel::ERROR_UNIT_BYTE);
    error_model.SetRate(errorRate);

    PointToPointHelper p2pSlow;
    p2pSlow.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pSlow.SetChannelAttribute("Delay", StringValue("100ms"));

    PointToPointHelper p2pFast;
    p2pFast.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pFast.SetChannelAttribute("Delay", StringValue("0.01ms"));

    PointToPointHelper bottleneck;
    bottleneck.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    bottleneck.SetChannelAttribute("Delay", StringValue("10ms"));
    bottleneck.SetDeviceAttribute("ReceiveErrorModel", PointerValue(&error_model));
    NetDeviceContainer devices;
    Ipv4AddressHelper address;

    devices = p2pFast.Install(nodes.Get(0), nodes.Get(2));
    address.SetBase("10.0.0.0", "255.255.255.0");
    Ipv4InterfaceContainer n0n2Interface = address.Assign(devices);

    devices = p2pFast.Install(nodes.Get(3), nodes.Get(5));
    address.SetBase("10.0.1.0", "255.255.255.0");
    Ipv4InterfaceContainer n3n5Interface = address.Assign(devices);

    devices = p2pSlow.Install(nodes.Get(1), nodes.Get(2));
    address.SetBase("10.0.2.0", "255.255.255.0");
    Ipv4InterfaceContainer n1n2Interface = address.Assign(devices);

    devices = p2pSlow.Install(nodes.Get(3), nodes.Get(4));
    address.SetBase("10.0.3.0", "255.255.255.0");
    Ipv4InterfaceContainer n3n4Interface = address.Assign(devices);

    devices = bottleneck.Install(nodes.Get(2), nodes.Get(3));
    address.SetBase("10.0.4.0", "255.255.255.0");
    Ipv4InterfaceContainer n2n3Interface = address.Assign(devices);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    std::vector<std::vector<Ptr<PacketSink>>> sinks(3);
    std::vector<std::vector<Time>> startTimes(3);
    for (uint32_t node = 0; node < 2; node++)
    {
        Ptr<Node> currentSource = nodes.Get(node);
        for (uint32_t i = 0; i < nFlows; i++)
        {
            uint16_t port = 8080 + node * 100 + i;
            uint32_t currentNode = (i % 2 == 0) ? 4 : 5;
            uint32_t assignSink = 0;
            if(node == 0)
            {
                assignSink = currentNode - 4;
            }
            else
            {
                assignSink = currentNode - 3;
            }
            Ptr<Node> currentDestination = nodes.Get(currentNode);
            Address sinkAddress(InetSocketAddress(currentDestination->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), port));
            PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
            ApplicationContainer sinkApp = sink.Install(currentDestination);
            sinkApp.Start(Seconds(0.0));
            sinkApp.Stop(Seconds(20.0));
            if(node + assignSink < 3)
            {
                sinks[node + assignSink].push_back(DynamicCast<PacketSink>(sinkApp.Get(0)));
            }
            else
            {
                sinks[0].push_back(DynamicCast<PacketSink>(sinkApp.Get(0)));
            }
            Ptr<BulkSendApplication> source = CreateObject<BulkSendApplication>();
            source->SetAttribute("Protocol", TypeIdValue(TcpSocketFactory::GetTypeId()));
            source->SetAttribute("Remote", AddressValue(sinkAddress));
            source->SetAttribute("MaxBytes", UintegerValue(0));
            currentSource->AddApplication(source);
            Time start = Seconds(startTime->GetValue(1.0, 1.5));
            source->SetStartTime(start);
            source->SetStopTime(Seconds(20.0));
            if(node + assignSink < 3)
            {
                startTimes[node + assignSink].push_back(start);
            }
            else
            {
                startTimes[0].push_back(start);
            }
            Simulator::Schedule(Seconds(1.5), [source, i, node, transport_prot](){
                Ptr<Socket> tcpSocket = source->GetSocket();
                AsciiTraceHelper asciiTraceHelper;
                std::string fileName = "cwnd-" + transport_prot +"-source" + std::to_string(node) + "-flow" + std::to_string(i) + ".txt";
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
    }
    Simulator::Stop(Seconds(20.0));
    Simulator::Run();
    float goodput = 0;
    for (uint32_t i = 0; i < 3; i++)
    {
        uint64_t totalBps = 0;
        
        for (uint32_t j = 0; j < sinks[i].size(); j++)
        {
            uint64_t Bytes = sinks[i][j]->GetTotalRx();
            double duration = 20.0 - startTimes[i][j].GetSeconds();
            totalBps += Bytes / duration;
        }
        if (i == 0)
        {
            goodput = (totalBps / nFlows) * 8.0 * 1e-6;
            std::cout << "Goodput for short-long flows: " << goodput << " Mbps" << std::endl;
        }
        else
        {
            goodput = (totalBps / (nFlows / 2.0)) * 8.0 * 1e-6;
            if(i == 1)
            {
                std::cout << "Goodput for short-short flows: " << goodput << " Mbps" << std::endl;
            }
            else
            {
                std::cout << "Goodput for long-long flows: " << goodput << " Mbps" << std::endl;
            }
        }
    }
    Simulator::Destroy();
}
