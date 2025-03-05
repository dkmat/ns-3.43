#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab3Part1");

int
main(int argc, char *argv[])
{
    LogComponentEnable("Lab3Part1", LOG_LEVEL_INFO);
    std::string mobilityType = "walk";
    uint32_t nNodes = 400;
    double duration = 400.0;
    double minSpeed = 2.0;
    double maxSpeed = 6.0;
    double pause = 2.0;
    uint32_t seed = 0;
    CommandLine cmd(__FILE__);
    cmd.AddValue("mobilityType", "Type of mobility model: walk or way or direction", mobilityType);
    cmd.AddValue("nNodes", "Number of nodes", nNodes);
    cmd.AddValue("duration", "Duration of simulation", duration);
    cmd.AddValue("minSpeed", "Minimum speed of nodes", minSpeed);
    cmd.AddValue("maxSpeed", "Maximum speed of nodes", maxSpeed);
    cmd.AddValue("pause", "Pause time of nodes", pause);
    cmd.AddValue("seed", "Random number seed", seed);
    cmd.Parse(argc, argv);

    NodeContainer nodes;
    nodes.Create(nNodes);
    InternetStackHelper stack;
    stack.Install(nodes);
    RngSeedManager::SetSeed(seed);
    MobilityHelper mobility;
    ObjectFactory pos;
    pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
    pos.Set("X", StringValue("ns3::UniformRandomVariable[Min=-50.0|Max=50.0]"));
    pos.Set("Y", StringValue("ns3::UniformRandomVariable[Min=-50.0|Max=50.0]"));
    Ptr<PositionAllocator> posAlloc = pos.Create()->GetObject<PositionAllocator>();

    if(mobilityType == "walk")
    {
        mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                    "Mode", StringValue("Distance"),
                                  "Bounds", RectangleValue(Rectangle(-50, 50, -50, 50)),
                                  "Speed", StringValue("ns3::UniformRandomVariable[Min=" + std::to_string(minSpeed) + "|Max=" + std::to_string(maxSpeed) + "]"),
                                  "Distance", DoubleValue(10.0));
                                  
    }
    else if(mobilityType == "way")
    {
        mobility.SetMobilityModel("ns3::RandomWaypointMobilityModel",
                                    "Speed", StringValue("ns3::UniformRandomVariable[Min=" + std::to_string(minSpeed) + "|Max=" + std::to_string(maxSpeed) + "]"),
                                  "Pause", StringValue("ns3::ConstantRandomVariable[Constant=" + std::to_string(pause) + "]"),
                                  "PositionAllocator", PointerValue(posAlloc));
    }
    else if(mobilityType == "direction")
    {
        mobility.SetMobilityModel("ns3::RandomDirection2dMobilityModel",
                                    "Bounds", RectangleValue(Rectangle(-50, 50, -50, 50)),
                                  "Speed", StringValue("ns3::UniformRandomVariable[Min=" + std::to_string(minSpeed) + "|Max=" + std::to_string(maxSpeed) + "]"),
                                  "Pause", StringValue("ns3::ConstantRandomVariable[Constant=" + std::to_string(pause) + "]"));
                                  
    }
    else
    {
        NS_LOG_ERROR("Invalid mobility type, type walk, way or direction");
        return 1;
    }
    mobility.SetPositionAllocator(posAlloc);
    mobility.Install(nodes);

    Simulator::Stop(Seconds(duration));
    Simulator::Run();

    double cellSize = 5.0;
    uint32_t gridRows = 100.0 / cellSize;
    uint32_t gridCols = 100.0 / cellSize;
    std::set<std::pair<int, int>> occupiedCells;
    std::ofstream outputFile("empty-cells-" + mobilityType + "-" + std::to_string(seed) + ".txt");
    for (uint32_t i = 0; i < nNodes; i++)
    {

        Ptr<MobilityModel> mobilityMod = nodes.Get(i)->GetObject<MobilityModel>();
        Vector pos = mobilityMod->GetPosition();
        int cellX = static_cast<int>((pos.x + 50.0) / cellSize);
        int cellY = static_cast<int>((pos.y + 50.0) / cellSize);
        occupiedCells.insert({cellX, cellY});
        outputFile << pos.x << " " << pos.y << std::endl;
    }
    outputFile.close();
    uint32_t totalCells = gridRows * gridCols;
    uint32_t emptyCells = totalCells - occupiedCells.size();

    NS_LOG_INFO("Empty cells: " << emptyCells);
    Simulator::Destroy();
    return 0;
}