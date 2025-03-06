#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab3Part2");

int
main(int argc, char *argv[])
{
    LogComponentEnable("Lab3Part2", LOG_LEVEL_INFO);
    uint32_t nNodes = 200;
    double duration = 400.0;
    double minSpeed = 0.0;
    double maxSpeed = 20.0;
    double pause = 2.0;
    CommandLine cmd(__FILE__);
    cmd.AddValue("minSpeed", "Minimum speed of nodes", minSpeed);
    cmd.Parse(argc, argv);
    NodeContainer nodes;
    nodes.Create(nNodes);
    InternetStackHelper stack;
    stack.Install(nodes);

    MobilityHelper mobility;
    ObjectFactory pos;
    pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
    pos.Set("X", StringValue("ns3::UniformRandomVariable[Min=-50.0|Max=50.0]"));
    pos.Set("Y", StringValue("ns3::UniformRandomVariable[Min=-50.0|Max=50.0]"));
    Ptr<PositionAllocator> posAlloc = pos.Create()->GetObject<PositionAllocator>();
    mobility.SetMobilityModel("ns3::RandomWaypointMobilityModel",
        "Speed", StringValue("ns3::UniformRandomVariable[Min=" + std::to_string(minSpeed) + "|Max=" + std::to_string(maxSpeed) + "]"),
      "Pause", StringValue("ns3::ConstantRandomVariable[Constant=" + std::to_string(pause) + "]"),
      "PositionAllocator", PointerValue(posAlloc));

    mobility.SetPositionAllocator(posAlloc);
    mobility.Install(nodes);

    Simulator::Stop(Seconds(duration));
    Simulator::Run();

    double totalSpeed = 0.0;
    uint32_t movingNodes = 0;

    std::ofstream outputFile("node-minSpeed="+ std::to_string(static_cast<int>(minSpeed)) +".txt");

    for (uint32_t i = 0; i < nNodes; i++)
    {
        Ptr<MobilityModel> mobilityMod = nodes.Get(i)->GetObject<MobilityModel>();
        Vector pos = mobilityMod->GetPosition();
        Vector velocity = mobilityMod->GetVelocity();
        double speed = velocity.x * velocity.x + velocity.y * velocity.y;

        outputFile << speed << " " << pos.x << " " << pos.y << std::endl;

        if (speed > 0.0)
        {
            totalSpeed += speed;
            movingNodes++;
        }
    }
    outputFile.close();
    double avgSpeed = (movingNodes > 0) ? (totalSpeed / movingNodes) : 0.0;

    NS_LOG_INFO("Average final node speed: " << avgSpeed);

    Simulator::Destroy();
    return 0;
}