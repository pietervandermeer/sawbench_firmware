from __future__ import print_function, division

import matplotlib.pyplot as plt
import numpy as np

x = np.linspace(0,+8,128)
x0=0
y = 1./(1+np.exp(x-x0))

# plt.cla()
# plt.plot(x, y)
# plt.show()

i=0
for x_ in x:
	print("0x%04X," % int(y[i]*65535*2 + 0.5))
	i+=1
