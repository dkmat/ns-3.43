#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/log.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/ssid.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/command-line.h"
#include "ns3/config.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab4Part1");

int
main(int argc, char *argv[])
{
    LogComponentEnable("Lab4Part1", LOG_LEVEL_INFO);
    
}