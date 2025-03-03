/*
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
//
//   c2                                             c1
//    |                                             |
//    | 10.1.6.0                                    | 10.1.3.0
//    |   10.1.5.0       10.1.4.0        10.1.2.0   |
//  relay -------- relay -------- server -------- relay
//                                  |
//                                  | 10.1.1.0
//                                  |
//                                  c0
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab1Part1a");

int
main(int argc, char* argv[])
{
    uint32_t nPackets = 1;
    uint32_t interval = 1;
    uint32_t packetSize = 1024;
    std::string dataRate = "5Mbps";
    std::string delay = "2ms";
    CommandLine cmd(__FILE__);
    cmd.AddValue("nPackets", "Number of packets to echo", nPackets);
    cmd.AddValue("interval", "Interval between packets", interval);
    cmd.AddValue("packetSize", "Size of packet in bytes", packetSize);
    cmd.AddValue("dataRate", "Rate at which packets are sent", dataRate);
    cmd.AddValue("delay","Propagation delay between links", delay);
    cmd.Parse(argc, argv);
    RngSeedManager::SetSeed(123456789);
    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer clientNodes, relayNodes, serverNode;
    clientNodes.Create(3);
    relayNodes.Create(3);
    serverNode.Create(1);

    InternetStackHelper stack;
    stack.Install(clientNodes);
    stack.Install(relayNodes);
    stack.Install(serverNode);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue(dataRate));
    pointToPoint.SetChannelAttribute("Delay", StringValue(delay));

    NetDeviceContainer devices;
    Ipv4AddressHelper address;

    devices = pointToPoint.Install(NodeContainer(clientNodes.Get(0), serverNode));
    address.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer c0ServerInterface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(clientNodes.Get(1), relayNodes.Get(0)));
    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer c1Relay0Interface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(relayNodes.Get(0), serverNode));
    address.SetBase("10.1.2.0","255.255.255.0");
    Ipv4InterfaceContainer relay0ServerInterface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(clientNodes.Get(2), relayNodes.Get(2)));
    address.SetBase("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer c2Relay2Interface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(relayNodes.Get(2), relayNodes.Get(1)));
    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer relay2Relay1Interface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(relayNodes.Get(1), serverNode));
    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer relay1ServerInterface = address.Assign(devices);

    UdpEchoServerHelper echoServer(9);
    echoServer.SetAttribute("Port", UintegerValue(45));
    ApplicationContainer serverApps = echoServer.Install(serverNode);
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(20.0));

    Ptr<UniformRandomVariable> startTime = CreateObject<UniformRandomVariable>();

    UdpEchoClientHelper echoClientC0(c0ServerInterface.GetAddress(1), 45);
    echoClientC0.SetAttribute("MaxPackets", UintegerValue(nPackets));
    echoClientC0.SetAttribute("Interval", TimeValue(Seconds(interval)));
    echoClientC0.SetAttribute("PacketSize", UintegerValue(packetSize));
    
    ApplicationContainer clientAppsC0 = echoClientC0.Install(clientNodes.Get(0));
    clientAppsC0.Start(Seconds(startTime->GetValue(2.0, 6.0)));
    clientAppsC0.Stop(Seconds(20.0));

    UdpEchoClientHelper echoClientC1(relay0ServerInterface.GetAddress(1), 45);
    echoClientC1.SetAttribute("MaxPackets", UintegerValue(nPackets));
    echoClientC1.SetAttribute("Interval", TimeValue(Seconds(interval)));
    echoClientC1.SetAttribute("PacketSize", UintegerValue(packetSize));

    ApplicationContainer clientAppsC1 = echoClientC1.Install(clientNodes.Get(1));
    clientAppsC1.Start(Seconds(startTime->GetValue(2.0, 6.0)));
    clientAppsC1.Stop(Seconds(20.0));

    UdpEchoClientHelper echoClientC2(relay1ServerInterface.GetAddress(1), 45);
    echoClientC2.SetAttribute("MaxPackets", UintegerValue(nPackets));
    echoClientC2.SetAttribute("Interval", TimeValue(Seconds(interval)));
    echoClientC2.SetAttribute("PacketSize", UintegerValue(packetSize));

    ApplicationContainer clientAppsC2 = echoClientC2.Install(clientNodes.Get(2));
    clientAppsC2.Start(Seconds(startTime->GetValue(2.0, 6.0)));
    clientAppsC2.Stop(Seconds(20.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(20.0));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
