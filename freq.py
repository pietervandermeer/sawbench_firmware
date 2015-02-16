""" exponential stuff """

from __future__ import print_function, division

import numpy as np
import matplotlib.pyplot as plt

highest_note = 120

semitones_per_octave = 6

# MIDI note nr
note_nr = np.arange(0, highest_note, semitones_per_octave) 

# plt.cla()
# plt.plot(note_nr_)
# plt.show()
f = 8.1757989156 * 2**(note_nr/12.) # western scale - one semitone at a time.. starting at MIDI C0 = 8.1756 Hz

DAC_value = f * 12

for i in range(highest_note):
	if DAC_value[i] > 65535.5:
		break

	#print("%f %fV %d," % (f[i] , Vcontrol[i], int(DAC_value[i]+0.5)))
	print("%d," % int(DAC_value[i]+0.5))

	i += 1 # next note number

print("65535") # dummy
