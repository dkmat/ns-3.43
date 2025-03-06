import os

minSpeed = range(10, -1, -2)

for speed in minSpeed:
    command = f"./ns3 run \"scratch/lab3-part2.cc --minSpeed={speed}\""
    result = os.popen(command).read()
    print(f"minSpeed: {speed}")
    print(result)
