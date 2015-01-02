/**
	connect_port.c

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#include "connect_port.h"

void connect_port(LV2_Handle instance, uint32_t port, void* data) {
	Plugin *plugin = (Plugin* )instance;

	switch((PortIndex)port) {
		case input_audio:
			plugin->channels.input_audio = (const float *)data;
			break;
		case output_midi:
			plugin->channels.output_midi = (LV2_Atom_Sequence *)data;
			break;
		case input_gain:
			plugin->channels.input_gain = (float *)data;
			break;
		case detector_buffer:
			plugin->channels.detector_buffer = (float *)data;
			break;
		case detector_gap:
			plugin->channels.detector_gap = (float *)data;
			break;
		case threshold:
			plugin->channels.threshold = (float *)data;
			break;
		case midi_note:
			plugin->channels.midi_note = (float *)data;
			break;
		case note_off_mode:
			plugin->channels.note_off_mode = (float *)data;
			break;
		case note_off_delay:
			plugin->channels.note_off_delay = (float *)data;
			break;
		case velocity_floor:
			plugin->channels.velocity_floor = (float *)data;
			break;
		case velocity_ceiling:
			plugin->channels.velocity_ceiling = (float *)data;
			break;
		case limit_rms_velocity_ceiling:
			plugin->channels.limit_rms_velocity_ceiling = (float *)data;
			break;
	}
}
