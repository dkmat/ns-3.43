#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-apps-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab1-part2");

int
main(int argc, char* argv[])
{
    uint32_t nCsma = 3;
    uint32_t interval = 5;
    std::string p2pDataRate = "5Mbps";
    std::string p2pDelay = "2ms";
    std::string csmaDataRate = "100Mbps";
    uint32_t csmaDelay = 6560;
    CommandLine cmd(__FILE__);
    cmd.AddValue("nCsma", "Number of extra CSMA nodes/devices", nCsma);
    cmd.AddValue("interval", "Interval between packets", interval);
    cmd.Parse(argc, argv);
    RngSeedManager::SetSeed(123456789);
    LogComponentEnable("Ping", LOG_LEVEL_INFO);
    NodeContainer relayNodes, pingNodes, lanNode;
    relayNodes.Create(3);
    pingNodes.Create(2);
    lanNode.Create(1);

    NodeContainer csmaNodes;
    csmaNodes.Add(lanNode);
    csmaNodes.Create(nCsma);

    InternetStackHelper stack;
    stack.Install(pingNodes);
    stack.Install(relayNodes);
    stack.Install(csmaNodes);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue(p2pDataRate));
    pointToPoint.SetChannelAttribute("Delay", StringValue(p2pDelay));

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue(csmaDataRate));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(csmaDelay)));

    NetDeviceContainer devices;
    Ipv4AddressHelper address;

    devices = pointToPoint.Install(NodeContainer(pingNodes.Get(0), relayNodes.Get(0)));
    address.SetBase("10.1.6.0","255.255.255.0");
    Ipv4InterfaceContainer p0Relay0Interface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(relayNodes.Get(0), relayNodes.Get(1)));
    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer relay0Relay1Interface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(relayNodes.Get(1), lanNode));
    address.SetBase("10.1.4.0","255.255.255.0");
    Ipv4InterfaceContainer relay1LanInterface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(pingNodes.Get(1), relayNodes.Get(2)));
    address.SetBase("10.1.3.0","255.255.255.0");
    Ipv4InterfaceContainer p1Relay2Interface = address.Assign(devices);

    devices = pointToPoint.Install(NodeContainer(relayNodes.Get(2), lanNode));
    address.SetBase("10.1.2.0","255.255.255.0");
    Ipv4InterfaceContainer relay2LanInterface = address.Assign(devices);

    devices = csma.Install(csmaNodes);
    address.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer csmaInterface = address.Assign(devices);

    Ptr<UniformRandomVariable> startTime = CreateObject<UniformRandomVariable>();
    PingHelper pingP0(csmaInterface.GetAddress(nCsma));
    pingP0.SetAttribute("Interval", TimeValue(Seconds(interval)));
    ApplicationContainer appsP0 = pingP0.Install(pingNodes.Get(0));
    appsP0.Start(Seconds(startTime->GetValue(2.0, 6.0)));
    appsP0.Stop(Seconds(20.0));

    PingHelper pingP1(csmaInterface.GetAddress(nCsma));
    pingP1.SetAttribute("Interval", TimeValue(Seconds(interval)));
    ApplicationContainer appsP1 = pingP1.Install(pingNodes.Get(1));
    appsP1.Start(Seconds(startTime->GetValue(2.0, 6.0)));
    appsP1.Stop(Seconds(20.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    pointToPoint.EnablePcapAll("lab1-part2");
    csma.EnablePcap("lab1-part2", devices.Get(nCsma), true);
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
