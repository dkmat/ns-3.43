import matplotlib.pyplot as plt

# File containing minSpeed (column 1) and ratio (column 2)
filename = "plot.txt"

# Initialize lists for storing minSpeed and ratio values
minSpeed = []
ratio = []

# Read the file and extract values
with open(filename, "r") as file:
    for line in file:
        values = line.split()  # Split by space
        minSpeed.append(float(values[0]))  # Convert minSpeed to float
        ratio.append(float(values[1]))  # Convert ratio to float

# Plot the data
plt.figure(figsize=(8, 5))
plt.plot(minSpeed, ratio, marker="o", linestyle="-", color="b", label="Ratio vs. MinSpeed")

# Labels and title
plt.xlabel("MinSpeed (m/s)")
plt.ylabel("Ratio")
plt.title("Ratio of Avg Final Speed to Avg Initial Speed vs. MinSpeed")
plt.grid(True)
plt.legend()

# Show the plot
plt.show()
