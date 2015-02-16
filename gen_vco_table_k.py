""" sawbench vco calibration for "K" unit. thing seems to have an upward trend in octaves 0,+1,+2 for some reason. """

from __future__ import print_function, division

import numpy as np
import matplotlib.pyplot as plt

def gauss(x, *p):
    A, mu, sigma = p
    return A*np.exp(-(x-mu)**2/(2.*sigma**2))

def sinestep(x, *p):
    A, mu, sigma = p
    y = 0.5*A*( 1 - np.cos((x-mu)*2*np.pi/sigma) )
    idx = (x < mu) 
    y[idx] = 0
    idx = ((x-mu) > sigma/2)
    y[idx] = A
    return y

#
# Factory-trimmable constants
#

Rtrim = 3300. # Ohm

#
# Analog constants
#

Vsupply = 5. # V

#peak-to-peak voltage of the saw wave. as present on the cap
#Vpp = 3.1 # ideal 
Vpp = 2.8 # measured on scope.. weird

C = 117*1e-9 # 100 nF, capacitance.. there's 10% tolerance here, but this can be trimmed by Rtrim
Voff = 0.006 # V, offset voltage present at the trimpot ground

Toff = 60*1e-6 # 80 us, capacitor discharge time
#Toff = 250*1e-6 # 80 us, capacitor discharge time

highest_note = 110

# MIDI note nr
note_nr = np.arange(0,highest_note,1)

# a, mu sigma
#p = [0.8, 60, 10]
#note_nr_ = note_nr - gauss(note_nr, *p) 
p = [0.8, 48, 34]
note_nr_ = note_nr - sinestep(note_nr, *p) 

# plt.cla()
# plt.plot(note_nr_)
# plt.show()
f = 8.1757989156 * 2**(note_nr_/12.) # western scale - one semitone at a time.. starting at MIDI C0 = 8.1756 Hz

#Toff = 50*1e-6 + (60*1e-6 * f/4400) # variable discharge time. because the cap is charging also when it is discharging! 
Vcontrol = (C*Vpp*Rtrim / (1./f - Toff)) - Voff
DAC_value = (65536/Vsupply) * Vcontrol

#
# write all this to file
#

f = open('libraries/vco/vcotable.h', 'w')

for i in range(highest_note):
	if DAC_value[i] > 65535.5:
		break

	#f.write("%f %fV %d," % (f[i] , Vcontrol[i], int(DAC_value[i]+0.5)))
	f.write("%d," % int(DAC_value[i]+0.5))

	i += 1 # next note number

f.write("65535") # dummy
