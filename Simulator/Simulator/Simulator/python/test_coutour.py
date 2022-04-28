import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import griddata

x_data = np.arange(0, 10)
y_data = np.arange(0, 10)


X = np.linspace(0, 10, 100)
Y = np.linspace(0, 10, 100)

X, Y = np.meshgrid(X, Y)

Z = np.arange(0, 10000).reshape((100, 100))

zi = griddata((x_data, y_data), c_data, (xi, yi), method='cubic')
print(X.shape)
print(Y.shape)
print(Z.shape)


plt.contourf(X, Y, Z)
plt.colorbar()
#ax.scatter(X, Y, color="green")
plt.title('Regular Grid, created by Meshgrid')
plt.xlabel('x')
plt.ylabel('y')
plt.show()