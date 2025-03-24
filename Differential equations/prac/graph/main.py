import numpy as np
import matplotlib.pyplot as plt

theta = np.linspace(0, 4 * np.pi, 500) 
r = np.exp(-0.1 * theta) 

xi = r * np.cos(theta)
eta = r * np.sin(theta)

# Plot the graph
plt.figure(figsize=(6, 6))
plt.plot(xi, eta, label="Spiral trajectory")
plt.axhline(0, color='black', linewidth=0.8)  # x-axis
plt.axvline(0, color='black', linewidth=0.8)  # y-axis
plt.scatter(0, 0, color="black", label="Focus O1", zorder=5)  # Mark the focus point

# Add labels and customize
plt.title("Phase Portrait: Focus")
plt.xlabel(r"$\xi$")
plt.ylabel(r"$\eta$")
plt.grid(True, linestyle="--", alpha=1)
plt.gca().set_aspect('equal', adjustable='box')  # Equal scaling
plt.legend()
plt.show()
