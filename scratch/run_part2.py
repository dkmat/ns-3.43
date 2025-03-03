import os

# Range for delay and nFlows
runs = range(1, 11)  # From 50 to 300 in steps of 50
nFlows_list = [2, 4, 6, 8]

# Loop over delays and nFlows
for nFlows in nFlows_list:
    print(f"flow = {nFlows}")
    for run in runs:
        command = f"./ns3 run \"scratch/lab2-part2.cc --nFlows={nFlows} --run={run} --transport_prot=TcpCubic\""
        # print(f"Running: {command}")
        
        # Capture and display the result
        result = os.popen(command).read()
        # Print the output of the simulation
        print(f"{run}\n{result}")

