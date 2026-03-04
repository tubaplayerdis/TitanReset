import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image

file_name = input("Name of file to analyize: ")
field_type = int(input("Match Field (0), Skills Field (1)?: "))

# Load lemlib data
data = pd.read_csv(
    file_name,
    header=None,
    names=["x", "y", "theta"]
)

field_img = None;

if field_type == 1:
    field_img = Image.open("SkillsField.png")
else:
    field_img = Image.open("MatchField.png")

FIELD_IN = 144  # VEX field size in inches

plt.figure(figsize=(6, 6))

# Draw field
plt.imshow(
    field_img,
    extent=[-FIELD_IN/13.8, FIELD_IN/13.8, -FIELD_IN/13.8, FIELD_IN/13.8]
)

# Plot path
plt.plot(data["x"], data["y"], "r", linewidth=2)
plt.scatter(data["x"], data["y"], s=4, c="r")

# Heading arrows (lemlib theta is degrees)
theta = np.deg2rad(data["theta"])
plt.quiver(
    data["x"][::10],
    data["y"][::10],
    np.cos(theta[::10]),
    np.sin(theta[::10]),
    scale=25,
    width=0.004,
    color="blue"
)

plt.axis("equal")
plt.xlabel("X (in)")
plt.ylabel("Y (in)")
plt.title("TitanReset debug data")
plt.show()
