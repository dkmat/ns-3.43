#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"
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

int
main(int argc, char *argv[])
{
    LogComponentEnable("Lab4Part1", LOG_LEVEL_INFO);
    double exponent = 2.0;
    CommandLine cmd(__FILE__);
    cmd.AddValue("exponent", "Path loss exponent for LogDistance model", exponent);
    cmd.Parse(argc, argv);

    NodeContainer wireStNode, apNode;
    wireStNode.Create(1);
    apNode.Create(1);

    SpectrumChannelHelper spectrumChannelHelper = SpectrumChannelHelper::Default();
    spectrumChannelHelper.AddPropagationLoss("ns3::LogDistancePropagationLossModel", "Exponent", DoubleValue(exponent));
    Ptr<SpectrumChannel> spectrumChannel = spectrumChannelHelper.Create();

    SpectrumWifiPhyHelper phy = SpectrumWifiPhyHelper::Default();
    phy.SetChannel(spectrumChannel);
    phy.Set("ChannelWidth", UintegerValue(20));
    phy.Set("TxPowerStart", DoubleValue(20.0));
    phy.Set("TxPowerEnd", DoubleValue(20.0));
}