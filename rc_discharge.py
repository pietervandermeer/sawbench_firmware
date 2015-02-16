""" 
shows that there's a nonlinearity in the discharge time. 
cap is charging (linearly) even when it's discharging (exponentially).
the plots show that the discharge time is not a linear function of frequency..
"""

from __future__ import print_function, division

import numpy as np
import matplotlib.pyplot as plt

# discharge resistors (ohm)
r1 = 100
r2 = 100
# capacitors (farad)
c1 = 100*1e-9
c2 = 80*1e-9
# rc constants
rc1 = r1*c1
rc2 = r2*c2

# time (s)
t = np.linspace(0,1e-4,100)
# charging current (ampere)
i = 4e-4 
# discharging + charging voltage of the capacitors
# v = q/v = i*t/v (in the case of linear charging)
# v = exp(-t/rc) (in the case of normal passive discharge)
v1 = np.exp(-t/rc1) + (i*t/c1)
v2 = np.exp(-t/rc2) + (i*t/c2)

plt.cla()
plt.plot(t,v1,'b-',t,v2,'g-')
plt.plot([0,1e-4],[0.1,0.1],'k-')
plt.show()
