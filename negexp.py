from __future__ import print_function, division

import matplotlib.pyplot as plt
import numpy as np

x = np.linspace(0,7,128)
x0 = 0
yexp = np.exp(-x-x0)

x = np.linspace(1,7,128) #FW1.1
x2 = np.linspace(1,7*1.2599*1.2599,128) #FW1.2 (*2^.667)
ycube = x**(-3)
ycube2 = x2**(-3)

plt.cla()
plt.plot(x, yexp, 'b-', x, ycube, 'g-', x, ycube2*4, 'r-')
plt.show()

i=0
for x_ in x:
#	print("0x%04X," % int(ycube[i]*65535 + 0.5)) #FW1.1
	print("0x%04X," % int(ycube2[i]*65535 + 0.5)) #FW1.2
	i+=1
