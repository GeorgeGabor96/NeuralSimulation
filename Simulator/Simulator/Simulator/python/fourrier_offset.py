import numpy as np
import matplotlib.pyplot as plt

n = 1000
Lx = 1000

omg = 2.0 * np.pi / Lx

x = np.linspace(0, Lx, n)
y = np.sin(10.0 * omg * x) + np.sin(20.0 * omg * x)
y1 = np.sin(10.0 * omg * x)
y2 = np.sin(20.0 * omg * x)
plt.plot(x, y1)
plt.plot(x, y2)
plt.plot(x, y)


plt.show()