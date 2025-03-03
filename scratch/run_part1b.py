import os

# Range for delay and nFlows
delays = range(50, 301, 50)  # From 50 to 300 in steps of 50
nFlows_list = [1, 2, 4]

# Loop over delays and nFlows
for nFlows in nFlows_list:
    print(f"flow = {nFlows}")
    for delay in delays:
        command = f"./ns3 run \"scratch/lab2-part1.cc --delay={delay}ms --nFlows={nFlows} --transport_prot=TcpCubic\""
        # print(f"Running: {command}")
        
        # Capture and display the result
        result = os.popen(command).read()
        result = result.replace("Goodput:","")
        result = result.replace("Mbps","")
        result = result.strip()
        # Print the output of the simulation
        print(f"{delay} {result}")
