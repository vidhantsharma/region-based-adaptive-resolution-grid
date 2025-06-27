import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.colors as mcolors
import matplotlib.cm as cm
import os
import sys

# File path
csv_file = "grid_output_2d.csv"
if not os.path.exists(csv_file):
    raise FileNotFoundError(f"{csv_file} not found. Make sure to run the C++ program first.")

# Load CSV
df = pd.read_csv(csv_file)

# Extract dimension columns
dimension_cols = [col for col in df.columns if col.startswith("dim")]
num_dims = len(dimension_cols)

print(f"Detected {num_dims} dimensions from columns: {dimension_cols}")
if num_dims > 3:
    sys.exit("❌ Cannot visualize more than 3 dimensions. Please reduce dimensions or project the data.")

# Normalize values for color mapping
norm = mcolors.Normalize(vmin=df['value'].min(), vmax=df['value'].max())
cmap = cm.viridis

# Plot setup
fig = plt.figure(figsize=(12, 10))

# 1D Plot
if num_dims == 1:
    ax = fig.add_subplot(111)
    for _, row in df.iterrows():
        x = row[dimension_cols[0]]
        res = row['resolution']
        val = row['value']
        color = cmap(norm(val))
        ax.plot(x, val, 'o', markersize=res * 20, color=color)
        ax.text(x, val, f"{val:.1f}", ha='center', va='bottom', fontsize=7, color='black')

    ax.set_xlabel("Position")
    ax.set_ylabel("Value")
    ax.set_title("1D Grid Visualization")

# 2D Plot
elif num_dims == 2:
    ax = fig.add_subplot(111)
    for _, row in df.iterrows():
        x, y = row[dimension_cols[0]], row[dimension_cols[1]]
        res = row['resolution']
        val = row['value']
        radius = res * 0.5
        color = cmap(norm(val))
        circle = plt.Circle((x, y), radius, facecolor=color, edgecolor='black', linewidth=0.4)
        ax.add_patch(circle)
        ax.text(x, y, f"{val:.1f}", ha='center', va='center', fontsize=7, color='white')

    padding = 1.0
    ax.set_xlim(df[dimension_cols[0]].min() - padding, df[dimension_cols[0]].max() + padding)
    ax.set_ylim(df[dimension_cols[1]].min() - padding, df[dimension_cols[1]].max() + padding)
    ax.set_aspect('equal')
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_title("2D Grid Visualization")
    ax.grid(True)

# 3D Plot
elif num_dims == 3:
    from mpl_toolkits.mplot3d import Axes3D  # noqa: F401
    ax = fig.add_subplot(111, projection='3d')
    for _, row in df.iterrows():
        x, y, z = row[dimension_cols[0]], row[dimension_cols[1]], row[dimension_cols[2]]
        res = row['resolution']
        val = row['value']
        color = cmap(norm(val))
        ax.scatter(x, y, z, s=res * 200, color=color)
        ax.text(x, y, z, f"{val:.1f}", fontsize=7, color='black')

    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")
    ax.set_title("3D Grid Visualization")

# Colorbar
sm = cm.ScalarMappable(norm=norm, cmap=cmap)
sm.set_array([])
cbar = plt.colorbar(sm, ax=ax, shrink=0.75)
cbar.set_label("Cell Value")

plt.tight_layout()
plt.show()
