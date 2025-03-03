#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
//
//   p2                                             p1
//    |                                             |
//    | 10.1.6.0                                    | 10.1.3.0
//    |   10.1.5.0       10.1.4.0        10.1.2.0   |
//  relay -------- relay -------- server -------- relay
//                                  |
//                                  | 10.1.1.0
//                                  |
//                                  p0
using namespace ns3;

int
main(int argc, char* argv[])
{
    uint32_t interval = 1;
    std::string dataRate = "5Mbps";
    std::string delay = "2ms";
    CommandLine cmd(__FILE__);
    cmd.AddValue("interval", "Interval between pings", interval);
    cmd.AddValue("dataRate", "Rate at which pings are sent", dataRate);
    cmd.AddValue("delay","Propagation delay between links", delay);
    cmd.Parse(argc, argv);
    RngSeedManager::SetSeed(123456789);
    NodeContainer pingNodes, relayNodes, hostNode;
    pingNodes.Create(3);
    relayNodes.Create(3);
    hostNode.Create(1);
    InternetStackHelper stack;
    stack.Install(pingNodes);
    stack.Install(relayNodes);
    stack.Install(hostNode);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue(dataRate));
    pointToPoint.SetChannelAttribute("Delay", StringValue(delay));

    NetDeviceContainer devices;
    Ipv4AddressHelper address;

    devices = pointToPoint.Install(NodeContainer(pingNodes.Get(0), hostNode));
    address.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer p0HostInterface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(pingNodes.Get(1), relayNodes.Get(0)));
    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer p1Relay0Interface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(relayNodes.Get(0), hostNode));
    address.SetBase("10.1.2.0","255.255.255.0");
    Ipv4InterfaceContainer relay0HostInterface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(pingNodes.Get(2), relayNodes.Get(2)));
    address.SetBase("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer p2Relay2Interface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(relayNodes.Get(2), relayNodes.Get(1)));
    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer relay2Relay1Interface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(relayNodes.Get(1), hostNode));
    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer relay1HostInterface = address.Assign(devices);

    Ptr<UniformRandomVariable> startTime = CreateObject<UniformRandomVariable>();
    PingHelper pingP0(p0HostInterface.GetAddress(1));
    pingP0.SetAttribute("Interval", TimeValue(Seconds(interval)));
    ApplicationContainer appsP0 = pingP0.Install(pingNodes.Get(0));
    appsP0.Start(Seconds(startTime->GetValue(2.0, 6.0)));
    appsP0.Stop(Seconds(20.0));

    PingHelper pingP1(relay0HostInterface.GetAddress(1));
    pingP1.SetAttribute("Interval", TimeValue(Seconds(interval)));
    ApplicationContainer appsP1 = pingP1.Install(pingNodes.Get(1));
    appsP1.Start(Seconds(startTime->GetValue(2.0, 6.0)));
    appsP1.Stop(Seconds(20.0));

    PingHelper pingP2(relay1HostInterface.GetAddress(1));
    pingP2.SetAttribute("Interval", TimeValue(Seconds(interval)));
    ApplicationContainer appsP2 = pingP2.Install(pingNodes.Get(2));
    appsP2.Start(Seconds(startTime->GetValue(2.0, 6.0)));
    appsP2.Stop(Seconds(20.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    pointToPoint.EnablePcapAll("lab1-part1b");
    Simulator::Stop(Seconds(20.0));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}

