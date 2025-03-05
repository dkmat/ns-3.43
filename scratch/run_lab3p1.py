import os
seeds = [13, 26, 4, 8, 130, 16, 4616, 30, 39, 2016]
mobilityTypes = [ "walk", "way", "direction"]
for mobility in mobilityTypes:
    print(f"MobilityType = {mobility}")
    for seed in seeds:
        command = f"./ns3 run \"scratch/lab3-part1b.cc --seed={seed} --mobilityType={mobility}\"" 
        # command = f"./ns3 run \"scratch/lab3-part1b.cc --seed={seed} --mobilityType={mobility} --minSpeed=20.0 --maxSpeed=22.0 --pause=60.0\""
        result = os.popen(command).read()
        print(result)