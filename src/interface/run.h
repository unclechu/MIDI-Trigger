/**
	run.h

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/unclechu/MIDI-Trigger/blob/master/LICENSE
*/

#ifndef RUN_H
#define RUN_H

#include <stdlib.h>
#include <inttypes.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/ext/atom/atom.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"

#include "../define.h"
#include "../types.h"
#include "../rms.h"

void run(LV2_Handle instance, uint32_t n_samples);

#endif // RUN_H
