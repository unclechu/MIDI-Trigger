/**
	define.h

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#ifndef DEFINE_H
#define DEFINE_H

#include <math.h>

#define PLUGIN_URI "https://github.com/metachronica/audio-dsp-midi-trigger"

#define DETECTOR_BUFFER_MAX 200 // ms
#define DETECTOR_GAP_MAX 2000 // ms

/** Define a macro for converting a gain in dB to a coefficient */
#define DB_CO(g) ((g) > -90.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)
#define CO_DB(c) (log10f(c) / 0.05f)

#define SAMPLES_IN_MS(MS, SR) (round((SR / 1000.0f) * MS))
#define LEN(MIN, MAX) (MAX - MIN)

#endif // DEFINE_H
