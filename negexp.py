from __future__ import print_function, division

import matplotlib.pyplot as plt
import numpy as np

x = np.linspace(0,7,128)
x0 = 0
yexp = np.exp(-x-x0)

x = np.linspace(1,7,128)
ycube = x**(-3)

plt.cla()
plt.plot(x, yexp, 'b-', x, ycube, 'g-')
plt.show()

i=0
for x_ in x:
	print("0x%04X," % int(ycube[i]*65535 + 0.5))
	i+=1
