import numpy as np
import matplotlib.pyplot as plt
import math

values = []

value = 0.0

for i in range(100):
    if i == 30:
        value = 1.0
    values.append(value)
    value = value * math.exp(- 1 / 10)


plt.plot(np.arange(100), values)
plt.show()