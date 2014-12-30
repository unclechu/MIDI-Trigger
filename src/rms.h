/**
	rms.h

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#ifndef RMS_H
#define RMS_H

#include <math.h>
#include <inttypes.h>

inline float rms(float** buff, uint32_t size);

#endif // RMS_H
