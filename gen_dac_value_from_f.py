from __future__ import print_function, division

# Input
f = 3900. # Hz

# Factory trimmable
Rtrim = 3300. # Ohm

# Analog constants
Vsupply = 5. # V
Vpp = 2.8 # V
C = 100*1e-9 # 100 nF
Toff = 80*1e-6 # 80 us
Voff = 0.006 # V

Vcontrol = (C*Vpp*Rtrim / (1./f - Toff)) - Voff
DAC_value = (65536/Vsupply) * Vcontrol
print(DAC_value)
