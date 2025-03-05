import matplotlib.pyplot as plt

# Read node positions from the file
positions = []
with open("empty-cells-direction-4616.txt", "r") as file:
    for line in file:
        x, y = map(float, line.strip().split())
        positions.append((x, y))

# Extract x and y coordinates
x_coords, y_coords = zip(*positions)

# Create the plot
plt.figure(figsize=(8, 8))
plt.scatter(x_coords, y_coords, c="blue", marker="o", label="Nodes")
plt.xlim(-50, 50)
plt.ylim(-50, 50)
plt.axhline(0, color="gray", linestyle="--", linewidth=0.5)  # Center lines
plt.axvline(0, color="gray", linestyle="--", linewidth=0.5)
plt.grid(True, linestyle="--", linewidth=0.5)
plt.xlabel("X Position")
plt.ylabel("Y Position")
plt.title("Final Positions of 400 Nodes")
plt.legend()
plt.show()
