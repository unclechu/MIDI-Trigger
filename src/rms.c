/**
	rms.c

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/unclechu/MIDI-Trigger/blob/master/LICENSE
*/

#include "rms.h"

inline float rms(float* buff, uint32_t size) {
	float sum = 0.0f;

	for (uint32_t i=0; i<size; ++i) {
		sum += powf(buff[i], 2);
	}

	return (1.0f / size) * sum;
}
