import numpy as np
import matplotlib.pyplot as plt

# Constants
L0 = 46.6777  # Path loss at d0 in dB
d0 = 1.0      # Reference distance in meters
Pn = 4.01e-13    
Pt = 20.0     # Transmit power in dBm

# Path loss exponents
n_values = [2.5, 3.0, 3.5]

# Distances (from 1 meter to 100 meters)
distances = np.array([10.4403,16.0935, 22.561, 29.3087, 36.1801, 40.4228, 44.8219, 49.3356, 53.9351, 58.6003, 63.3167, 68.0735])

# Theoretical Received Power Calculation
def measured_received_power(n, distance):
    # Path loss calculation
    PL = L0 + 10 * n * np.log10(distance / d0)
    # Received power in dBm
    Pr = Pt - PL
    return Pr
def dB_to_linear(dB):
    return 10**(dB / 10)

def theoretical_received_power(SNR, Pn):
    SNR_linear = dB_to_linear(SNR)
    Pr_watts = SNR_linear * Pn
    Pr_dBm = 10 * np.log10(Pr_watts * 1000)
    return Pr_dBm
# Generate theoretical received power for each path loss exponent
plt.figure(figsize=(10, 6))

for n in n_values:
    measured_powers = measured_received_power(n, distances)
    plt.scatter(distances, measured_powers, label=f'Path Loss Exponent = {n}', marker = 'o')

SNR_values = {
    2.5: [25,29,29,29,29,29,25,25,25,25,20,20],
    3.0: [25,29,29,25,18,15,15,15,15,11,11,11],
    3.5: [25,29,25,15,11,9,2,2,2,2,2,2]
}

for n in n_values:
    theoretical_powers = [theoretical_received_power(SNR, Pn) for SNR in SNR_values[n]]
    plt.plot(distances, theoretical_powers, label=f'Theoretical SNR received power (n={n})')
# Labels and legend
plt.title('Received Power vs. Distance (Log-Distance Path Loss Model)')
plt.xlabel('Distance (m)')
plt.ylabel('Received Power (dBm)')
plt.legend()

# Display the plot
plt.grid(True)
plt.show()
