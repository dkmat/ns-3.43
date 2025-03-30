#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/config.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include <string>
#include <iostream>
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab4Part1");
double interval = 1.0;
double lastLinkRate = 0.0;
void
RateCallback(uint64_t oldRate, uint64_t newRate)
{
    std::cout << "Rate " << newRate / 1e6 << " Mbps" << std::endl;
    lastLinkRate = newRate / 1e6;
}

void moveStation(Vector pos, uint32_t step, Ptr<Node> stNode)
{
    Ptr<MobilityModel> mobility =stNode->GetObject<MobilityModel>();
    mobility->SetPosition(pos);

    Ptr<MobilityModel> apMobility = NodeList::GetNode(1)->GetObject<MobilityModel>();
    double distance = CalculateDistance(pos, apMobility->GetPosition());

    std::cout << "Time: " << Simulator::Now().GetSeconds() << "s, "
            << "Station Location: (" << pos.x << "," << pos.y << "," << pos.z << ") "
            << "Distance: " << distance << "m "
            << "Link Rate: " << lastLinkRate << " Mbps" << std::endl;

    Vector nextPos = pos;
    if (pos.y < 20)
    {
        nextPos.x += step;
        nextPos.y += step;

    }
    else if(pos.x < 65)
    {
        nextPos.x += step;
    }
    else
    {
        return;
    }
    Simulator::Schedule(Seconds(interval), &moveStation, nextPos, step, stNode);
}

int
main(int argc, char *argv[])
{
    LogComponentEnable("Lab4Part1", LOG_LEVEL_INFO);
    double exponent = 2.5;
    
    CommandLine cmd(__FILE__);
    cmd.AddValue("exponent", "Path loss exponent for LogDistance model", exponent);
    cmd.Parse(argc, argv);

    NodeContainer stNode, apNode;
    stNode.Create(1);
    apNode.Create(1);
    SpectrumWifiPhyHelper phy;
    SpectrumChannelHelper spectrumChannelHelper;
    spectrumChannelHelper.SetChannel("ns3::MultiModelSpectrumChannel");
    spectrumChannelHelper.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    spectrumChannelHelper.AddPropagationLoss("ns3::LogDistancePropagationLossModel", "Exponent", DoubleValue(exponent));
    Ptr<SpectrumChannel> spectrumChannel = spectrumChannelHelper.Create();
    
    phy.SetChannel(spectrumChannel);
    phy.SetErrorRateModel("ns3::NistErrorRateModel");
    phy.Set("TxPowerStart", DoubleValue(20.0));
    phy.Set("TxPowerEnd", DoubleValue(20.0));
    phy.Set("ChannelSettings",StringValue("{0, 20, BAND_UNSPECIFIED, 0}"));

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ac);
    wifi.SetRemoteStationManager("ns3::MinstrelHtWifiManager","RtsCtsThreshold", UintegerValue(65535));

    WifiMacHelper mac;
    Ssid ssid = Ssid("ap");
    NetDeviceContainer stDevice, apDevice, wifiDevice;
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
    stDevice.Add(wifi.Install(phy, mac, stNode.Get(0)));
    ssid = Ssid("ap");
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    apDevice.Add(wifi.Install(phy, mac, apNode.Get(0)));

    wifiDevice.Add(stDevice);
    wifiDevice.Add(apDevice);
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(stNode);
    mobility.Install(apNode);

    Ptr<MobilityModel> apMobility = apNode.Get(0)->GetObject<MobilityModel>();
    apMobility->SetPosition(Vector(0, 0, 4));
    Ptr<MobilityModel> stMobility = stNode.Get(0)->GetObject<MobilityModel>();
    stMobility->SetPosition(Vector(10, 0, 1));
    InternetStackHelper stack;
    stack.Install(stNode);
    stack.Install(apNode);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    // Ipv4InterfaceContainer stInterface = address.Assign(stDevice);
    // Ipv4InterfaceContainer apInterface = address.Assign(apDevice);
    Ipv4InterfaceContainer w = address.Assign(wifiDevice);
    Ipv4Address sinkAddress = w.GetAddress(0);

    uint16_t port = 9;
    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(sinkAddress, port));
    onoff.SetConstantRate(DataRate("400Mbps"), 1024);
    onoff.SetAttribute("StartTime", TimeValue(Seconds(0.1)));
    onoff.SetAttribute("StopTime", TimeValue(Seconds(30.0)));
    ApplicationContainer app = onoff.Install(apNode.Get(0));

    PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(sinkAddress, port));
    ApplicationContainer sinkApp = sink.Install(stNode.Get(0));
    sinkApp.Start(Seconds(0.1));
    sinkApp.Stop(Seconds(30.0));

    Config::ConnectWithoutContext(
        "/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/RemoteStationManager/$ns3::MinstrelHtWifiManager/Rate",
        MakeCallback(RateCallback));
    Simulator::Schedule(Seconds(0.5 + interval), &moveStation, Vector(10, 0, 1), 5, stNode.Get(0));
    Simulator::Stop(Seconds(30.0));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}