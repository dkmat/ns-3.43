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

int
main(int argc, char *argv[])
{
    LogComponentEnable("Lab4Part1", LOG_LEVEL_INFO);
    double exponent = 2.0;
    
    CommandLine cmd(__FILE__);
    cmd.AddValue("exponent", "Path loss exponent for LogDistance model", exponent);
    cmd.Parse(argc, argv);

    NodeContainer stNode, apNode;
    stNode.Create(1);
    apNode.Create(1);

    SpectrumChannelHelper spectrumChannelHelper;
    spectrumChannelHelper.SetChannel("ns3::MultiModelSpectrumChannel");
    spectrumChannelHelper.AddPropagationLoss("ns3::LogDistancePropagationLossModel", "Exponent", DoubleValue(exponent));
    Ptr<SpectrumChannel> spectrumChannel = spectrumChannelHelper.Create();

    SpectrumWifiPhyHelper phy;
    phy.SetChannel(spectrumChannel);
    phy.Set("ChannelSettings",StringValue("{0, 20, BAND_UNSPECIFIED, 0}"));
    phy.Set("TxPowerStart", DoubleValue(20.0));
    phy.Set("TxPowerEnd", DoubleValue(20.0));

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ac);
    wifi.SetRemoteStationManager("ns3::MinstrelHtWifiManager");

    WifiMacHelper mac;
    Ssid ssid = Ssid("ns3-80211ac");
    NetDeviceContainer stDevice, apDevice;
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
    stDevice = wifi.Install(phy, mac, stNode);

    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    apDevice = wifi.Install(phy, mac, apNode);

    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(0.0), "MinY", DoubleValue(0.0));
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(stNode);
    mobility.Install(apNode);

    InternetStackHelper stack;
    stack.Install(stNode);
    stack.Install(apNode);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer stInterface = address.Assign(stDevice);
    Ipv4InterfaceContainer apInterface = address.Assign(apDevice);

    uint16_t port = 9;
    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(stInterface.GetAddress(0), port));
    onoff.SetConstantRate(DataRate("400Mbps"), 1024);
    ApplicationContainer app = onoff.Install(apNode.Get(0));
    app.Start(Seconds(1.0));
    app.Stop(Seconds(10.0));

    PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp = sink.Install(stNode.Get(0));
    sinkApp.Start(Seconds(1.0));
    sinkApp.Stop(Seconds(10.0));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}