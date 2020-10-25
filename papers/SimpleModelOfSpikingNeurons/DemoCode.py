import numpy as np
import matplotlib.pyplot as plt
import matplotlib


Ne = 800 # number of excitatory neurons
re = np.random.uniform(0.0, 1.0, (Ne, 1))

Ni = 200 # number of inhibitory neurons
ri = np.random.uniform(0.0, 1.0, (Ni, 1))

a_e = 0.02 * np.ones((Ne, 1), dtype=np.float)
a_i = 0.02 + 0.08 * ri
a = np.vstack((a_e, a_i))

b_e = 0.2 * np.ones((Ne, 1), dtype=np.float)
b_i = 0.25 - 0.05 * ri
b = np.vstack((b_e, b_i))

c_e = -65.0 + 15.0 * (re ** 2)
c_i = -65 * np.ones((Ni, 1), dtype=np.float)
c = np.vstack((c_e, c_i))

d_e = 8.0 - 6.0 * (re ** 2)
d_i = 2 * np.ones((Ni, 1), dtype=np.float)
d = np.vstack((d_e, d_i))

S_e = 0.5 * np.random.rand(Ne + Ni, Ne)
S_i = - np.random.rand(Ne + Ni, Ni)
S = np.hstack((S_e, S_i))

v = -65 * np.ones((Ne + Ni, 1), dtype=np.float)     # Initial values of v
u = np.multiply(b, v)                               # Initial values of u
firings = np.zeros((0, 2))                                      # splide timigs
each_neuron = [[v[i]] for i in range(10)]

# Debug
print('Ne:', Ne)
print('Ni:', Ni)
print('a:', a.shape)
print('b:', b.shape)
print('c:', c.shape)
print('d:', d.shape)
print('S:', S.shape)
print('v:', v.shape)
print('u:', u.shape)

for t in range(1, 1000):                            # simulation of 1000 ms
    I_e = 5.0 * np.random.normal(size=(Ne, 1))
    I_i = 2.0 * np.random.normal(size=(Ni, 1))
    I = np.vstack((I_e, I_i))                       # thalamic input

    fired_mask = v >= 30
    fired = np.nonzero(fired_mask)[0]
    firings_new = np.hstack((np.expand_dims(t + 0 * fired, axis=1), np.expand_dims(fired, axis=1)))
    firings_new = firings_new.reshape(firings_new.shape[0], 2)
    firings = np.vstack((firings, firings_new))

    v[fired_mask] = c[fired_mask]
    u[fired_mask] = u[fired_mask] + d[fired_mask]
    I = I + np.expand_dims(np.sum(S[:, fired_mask[:, 0]], axis=1), axis=1)

    # step 2 times 0.5 ms for numerical stability
    v = v + 0.5 * (0.04 * (v ** 2) + 5 * v + 140 - u + I)
    v = v + 0.5 * (0.04 * (v ** 2) + 5 * v + 140 - u + I)
    u = u + a * (b * v - u)
    
    for i in range(len(each_neuron)):
        each_neuron[i].append(v[i])

plt.scatter(firings[:, 0], firings[:, 1], s=1.7)
plt.xlabel('time, ms')
plt.ylabel('neuron number')
plt.title('All neurons')
plt.savefig('neurons.png', dpi=200)
plt.close()

for i in range(len(each_neuron)):
    fig = plt.figure(figsize=(15, 4))
    ax = fig.add_subplot(111)
    ax.plot(np.arange(1000), np.array(each_neuron[i]))
    plt.xlabel('time, ms')
    plt.ylabel('mV')
    plt.title('Neuron %s activity' % (str(i)))
    plt.savefig('neuron_%s.png' % (str(i)))