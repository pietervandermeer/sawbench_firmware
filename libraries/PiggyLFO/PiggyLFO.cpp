using namespace std;

#include "PiggyLFO.h"
#include <avr/pgmspace.h>

const unsigned char sinewave[BUFFERSIZE] PROGMEM =
{
128,
131,
134,
137,
140,
143,
146,
149,
152,
155,
158,
162,
165,
167,
170,
173,
176,
179,
182,
185,
188,
190,
193,
196,
198,
201,
203,
206,
208,
211,
213,
215,
218,
220,
222,
224,
226,
228,
230,
232,
234,
235,
237,
238,
240,
241,
243,
244,
245,
246,
248,
249,
250,
250,
251,
252,
253,
253,
254,
254,
254,
255,
255,
255,
255,
255,
255,
255,
254,
254,
254,
253,
253,
252,
251,
250,
250,
249,
248,
246,
245,
244,
243,
241,
240,
238,
237,
235,
234,
232,
230,
228,
226,
224,
222,
220,
218,
215,
213,
211,
208,
206,
203,
201,
198,
196,
193,
190,
188,
185,
182,
179,
176,
173,
170,
167,
165,
162,
158,
155,
152,
149,
146,
143,
140,
137,
134,
131,
128,
124,
121,
118,
115,
112,
109,
106,
103,
100,
97,
93,
90,
88,
85,
82,
79,
76,
73,
70,
67,
65,
62,
59,
57,
54,
52,
49,
47,
44,
42,
40,
37,
35,
33,
31,
29,
27,
25,
23,
21,
20,
18,
17,
15,
14,
12,
11,
10,
9,
7,
6,
5,
5,
4,
3,
2,
2,
1,
1,
1,
0,
0,
0,
0,
0,
0,
0,
1,
1,
1,
2,
2,
3,
4,
5,
5,
6,
7,
9,
10,
11,
12,
14,
15,
17,
18,
20,
21,
23,
25,
27,
29,
31,
33,
35,
37,
40,
42,
44,
47,
49,
52,
54,
57,
59,
62,
65,
67,
70,
73,
76,
79,
82,
85,
88,
90,
93,
97,
100,
103,
106,
109,
112,
115,
118,
121,
124
};

const unsigned char sawwave[BUFFERSIZE] PROGMEM =
{
255,
254,
253,
252,
251,
250,
249,
248,
247,
246,
245,
244,
243,
242,
241,
240,
239,
238,
237,
236,
235,
234,
233,
232,
231,
230,
229,
228,
227,
226,
225,
224,
223,
222,
221,
220,
219,
218,
217,
216,
215,
214,
213,
212,
211,
210,
209,
208,
207,
206,
205,
204,
203,
202,
201,
200,
199,
198,
197,
196,
195,
194,
193,
192,
191,
190,
189,
188,
187,
186,
185,
184,
183,
182,
181,
180,
179,
178,
177,
176,
175,
174,
173,
172,
171,
170,
169,
168,
167,
166,
165,
164,
163,
162,
161,
160,
159,
158,
157,
156,
155,
154,
153,
152,
151,
150,
149,
148,
147,
146,
145,
144,
143,
142,
141,
140,
139,
138,
137,
136,
135,
134,
133,
132,
131,
130,
129,
128,
127,
126,
125,
124,
123,
122,
121,
120,
119,
118,
117,
116,
115,
114,
113,
112,
111,
110,
109,
108,
107,
106,
105,
104,
103,
102,
101,
100,
99,
98,
97,
96,
95,
94,
93,
92,
91,
90,
89,
88,
87,
86,
85,
84,
83,
82,
81,
80,
79,
78,
77,
76,
75,
74,
73,
72,
71,
70,
69,
68,
67,
66,
65,
64,
63,
62,
61,
60,
59,
58,
57,
56,
55,
54,
53,
52,
51,
50,
49,
48,
47,
46,
45,
44,
43,
42,
41,
40,
39,
38,
37,
36,
35,
34,
33,
32,
31,
30,
29,
28,
27,
26,
25,
24,
23,
22,
21,
20,
19,
18,
17,
16,
15,
14,
13,
12,
11,
10,
9,
8,
7,
6,
5,
4,
3,
2,
1,
0,
};

const unsigned char squarewave[BUFFERSIZE] PROGMEM =
{
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
255,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
};

//Constructor & Destructor
//======================================================
LFO::LFO(){

}

LFO::~LFO(){
}

//LFO output (main functie)
//======================================================
int LFO::LFOout(int wave, int index){
		switch(wave)
			{
			case 0:
			//Sample and hold				
				Xrand = (Xrand>>(Xrand&31)) | (Xrand<<(32-(Xrand&31)));
				Xrand += 7;

	
				output = Xrand % BUFFERSIZE;
				break;	
			case 1:
			//sine
				output = pgm_read_byte(sinewave + (index % BUFFERSIZE));
				break;
				
			case 2:
			//saw
				output = pgm_read_byte(sawwave + (index % BUFFERSIZE));
				break;	
			case 3:
			//square
				output = pgm_read_byte(squarewave + (index % BUFFERSIZE));

				break;
				
			}
		return output;
}





