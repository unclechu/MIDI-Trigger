/**
	define.h

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/unclechu/MIDI-Trigger/blob/master/LICENSE
*/

#ifndef DEFINE_H
#define DEFINE_H

#include <math.h>

#define PLUGIN_URI "https://github.com/unclechu/MIDI-Trigger"

/** Define a macro for converting a gain in dB to a coefficient */
#define DB_CO(g) ((g) > -90.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)
#define CO_DB(c) (log10f(c) / 0.05f)

#define SAMPLES_IN_MS(MS, SR) (round((SR / 1000.0f) * MS))
#define LEN(MIN, MAX) (MAX - MIN)

#endif // DEFINE_H
