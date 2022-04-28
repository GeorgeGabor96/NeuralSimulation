import numpy as np
import matplotlib.pyplot as plt

n = 20
alpha = 0.1
sigma = 4.0

def get_gamma_value(alpha, betta):
    pass



def get_gaussion_value():
    y1 = np.random.uniform(0, 1)
    y2 = np.random.uniform(0, 1)

    return np.cos(2 * 3.14 * y2) * np.sqrt(-2.0 * np.log(y1))

values = []
for i in range(100000):
    values.append(np.random.gamma(0.1, 4.0))

values = np.array(values)
print(np.mean(values))
print(np.std(values))

plt.hist(values, bins='auto')
plt.show()
