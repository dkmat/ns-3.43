#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include <string>
#include <iostream>
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab3Part1");

int
main(int argc, char *argv[])
{
    LogComponentEnable("Lab3Part1", LOG_LEVEL_INFO);
    std::string mobilityType = "walk";
    uint32_t nNodes = 5;
    uint32_t duration = 400;
    uint32_t minSpeed = 2;
    uint32_t maxSpeed = 6;
    uint32_t pause = 2;
    CommandLine cmd(__FILE__);
    cmd.AddValue("mobilityType", "Type of mobility model: walk or way or direction", mobilityType);
    cmd.AddValue("nNodes", "Number of nodes", nNodes);
    cmd.AddValue("duration", "Duration of simulation", duration);
    cmd.AddValue("minSpeed", "Minimum speed of nodes", minSpeed);
    cmd.AddValue("maxSpeed", "Maximum speed of nodes", maxSpeed);
    cmd.AddValue("pause", "Pause time of nodes", pause);
    cmd.Parse(argc, argv);

    NodeContainer nodes;
    nodes.Create(nNodes);
    InternetStackHelper stack;
    stack.Install(nodes);

    MobilityHelper mobility;
    
}