import os

# Range for delay and nFlows
delay = 1   # From 50 to 300 in steps of 50
nFlows_list = [1, 2, 4]
errors = [0.00001, 0.00005, 0.0001, 0.0005, 0.001]
# Loop over delays and nFlows
for nFlows in nFlows_list:
    print(f"flow = {nFlows}")
    for error in errors:
        command = f"./ns3 run \"scratch/lab2-part1.cc --delay={delay}ms --nFlows={nFlows} --errorRate={error} --transport_prot=TcpCubic\""
        # print(f"Running: {command}")
        
        # Capture and display the result
        result = os.popen(command).read()
        result = result.replace("Goodput:","")
        result = result.replace("Mbps","")
        result = result.strip()
        # Print the output of the simulation
        print(f"{error} {result}")
