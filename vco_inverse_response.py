""" invert vco response """

from __future__ import print_function, division

import numpy as np
import matplotlib.pyplot as plt
from scipy import interpolate

dac_input = np.array([
196,
277,
392,
555,
785,
1110,
1570,
2220,
3140,
4440,
6279,
8880,
12558,
17760,
25116,
35519,
50232,
65535])
y= np.array([
4014598,
2750919,
1222319,
1008874,
779374,
652715,
441661,
327450,
224915,
160538,
119193,
86184,
62740,
46141,
34320,
26170,
20721,
20190
])

# y = np.array([
# 3944095,
# 2707080,
# 1859405,
# 1296525,
# 909791,
# 641332,
# 453949,
# 322248,
# 228849,
# 163509,
# 117600,
# 85165,
# 62109,
# 45745,
# 34079,
# 26021,
# 20667,
# 20194
# ])

f_measured = 1 / ( y / 64000000. ) # from 1/64 us ticks Hz

# MIDI note nr
highest_note = 104
note_nr = np.arange(12, highest_note, 1) 
print(note_nr)
# freq * 12 to at least have some resolution (8 hz -> 100)
f_synth = 8.1757989156 * 2**(note_nr/12.) # western scale - one semitone at a time.. starting at MIDI C0 = 8.1756 Hz
f_synth_ = f_synth.copy()

# prepping for c code. no vector operations..
f_synth = np.empty(highest_note-12)
i = 0
j = 0
for idx_octave in range(1,9):
    octave_scalar = 2.**idx_octave
    for idx_note_in_oct in range(12):
        if i == f_synth.size:
            break
        f_synth[i] = 8.1757989156 * octave_scalar * 2**(idx_note_in_oct/12.) 
        if f_synth[i] >= f_measured[j+1]:
            j += 1
        df = f_measured[j+1] - f_measured[j]
        ddac = dac_input[j+1] - dac_input[j]
        f_frac = (f_synth[i] - f_measured[j])/df
        dac_synth_ = dac_input[j] + f_frac*ddac
        print(i, f_synth.size, f_synth[i], int(dac_synth_+.5))
        i += 1

print(f_synth)

in1dfunc = interpolate.interp1d(f_measured, dac_input)
dac_synth = in1dfunc(f_synth)
#tpl = interpolate.splrep(f_measured, dac_input, s=0)
#dac_synth = interpolate.splev(f_synth, tpl, der=0)
print(f_synth, dac_synth)

plt.cla()
ax = plt.gca()
#ax.set_xscale('log')
#ax.set_yscale('log')
print(f_measured.shape, dac_input.shape, f_synth.shape, dac_synth.shape)
plt.plot(f_synth,dac_synth,'g.')
plt.plot(f_measured,dac_input,'b.')
plt.show()

f = open('libraries/vco/vcotable.h', 'w')

for i in range(12):
    f.write("%d," % (8.17579 * 12. * 2**(float(i)/12)))

for dac_value in dac_synth:
	f.write("%d," % int(dac_value+0.5))

f.write("65535") # dummy
f.close()
