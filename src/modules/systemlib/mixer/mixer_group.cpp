/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file mixer_group.cpp
 *
 * Mixer collection.
 */

#include <px4_config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "mixer.h"
#include "mixer_factory.h"

#define debug(fmt, args...)	do { } while(0)
//#define debug(fmt, args...)	do { printf("[mixer] " fmt "\n", ##args); } while(0)
//#include <debug.h>
//#define debug(fmt, args...)	syslog(fmt "\n", ##args)

MixerGroup::MixerGroup(MixerRegisterGroups *reg_groups)
	: _first(nullptr)
	, _reg_groups(reg_groups)
{
}

MixerGroup::~MixerGroup()
{
	reset();
}

int
MixerGroup::append_mixer(Mixer *mixer)
{
	if (mixer == nullptr) {
		return -1;
	}

	Mixer **mpp;

	mpp = &_first;

	while (*mpp != nullptr) {
		mpp = &((*mpp)->_next);
	}

	*mpp = mixer;
	mixer->_next = nullptr;
	return 0;
}



int
MixerGroup::from_buffer(uint8_t *mixbuff, int bufflen)
{
	mixer_base_header_s *mixdata;
	Mixer *new_mixer;

	int remaining = bufflen;

	while (remaining > 0) {
		mixdata = (mixer_base_header_s *) &mixbuff[bufflen - remaining];

		if (mixdata->data_size == 0) {
			return remaining;
		}

		if (mixdata->mixer_type == MIXER_TYPES_NONE) {
			return remaining;
		}

		new_mixer = MixerFactory::factory(mixdata);

		if (new_mixer == nullptr) {
			return remaining;
		}

		append_mixer(new_mixer);

		remaining -= mixdata->data_size;
	}

	return remaining;
}


int
MixerGroup::to_buffer(uint8_t *mixbuff, int bufflen)
{
	int data_size;
	int buffpos = 0;
	Mixer *mix = _first;
	mixer_base_header_s *mixheader;

	while (mix != nullptr) {
		mixheader = mix->getMixerData();
		data_size = mixheader->data_size;

		//Check for buffer overflow
		if ((buffpos + data_size) >= bufflen) {
			return -1;
		}

		memcpy(&mixbuff[buffpos], mixheader, data_size);
		buffpos += data_size;
		mix = mix->_next;
	}

	return buffpos;
}


void
MixerGroup::reset()
{
	Mixer *mixer;
	Mixer *next = _first;

	/* flag mixer as invalid */
	_first = nullptr;

	/* discard sub-mixers */
	while (next != nullptr) {
		mixer = next;
		next = mixer->_next;
		delete mixer;
		mixer = nullptr;
	}
}

uint16_t
MixerGroup::mix_group()
{
	Mixer	*mixer = _first;

	while (mixer != nullptr) {
		mixer->mix(_reg_groups);
		mixer = mixer->_next;
	}

	return 0;
}

/*
 * set_trims() has no effect except for the SimpleMixer implementation for which set_trim()
 * always returns the value one.
 * The only other existing implementation is MultirotorMixer, which ignores the trim value
 * and returns _rotor_count.
 */
//unsigned
//MixerGroup::set_trims(int16_t *values, unsigned n)
//{
//	Mixer	*mixer = _first;
//	unsigned index = 0;

//	while ((mixer != nullptr) && (index < n)) {
//		/* convert from integer to float */
//		float offset = (float)values[index] / 10000;

//		/* to be safe, clamp offset to range of [-100, 100] usec */
//		if (offset < -0.2f) { offset = -0.2f; }

//		if (offset >  0.2f) { offset =  0.2f; }

//		debug("set trim: %d, offset: %5.3f", values[index], (double)offset);
//		index += mixer->set_trim(offset);
//		mixer = mixer->_next;
//	}

//	return index;
//}

//void
//MixerGroup::set_thrust_factor(float val)
//{
//	Mixer	*mixer = _first;

//	while (mixer != nullptr) {
//		mixer->set_thrust_factor(val);
//		mixer = mixer->_next;
//	}

//}

//uint16_t
//MixerGroup::get_saturation_status()
//{
//	Mixer	*mixer = _first;
//	uint16_t sat = 0;

//	while (mixer != nullptr) {
//		sat |= mixer->get_saturation_status();
//		mixer = mixer->_next;
//	}

//	return sat;
//}

unsigned
MixerGroup::count()
{
	Mixer	*mixer = _first;
	unsigned index = 0;

	while (mixer != nullptr) {
		mixer = mixer->_next;
		index++;
	}

	return index;
}

//void
//MixerGroup::groups_required(uint32_t &groups)
//{
//	Mixer	*mixer = _first;

//	while (mixer != nullptr) {
//		mixer->groups_required(groups);
//		mixer = mixer->_next;
//	}
//}

//int
//MixerGroup::load_from_buf(const char *buf, unsigned &buflen)
//{
//    int ret = -1;
//	const char *end = buf + buflen;

//	/*
//	 * Loop until either we have emptied the buffer, or we have failed to
//	 * allocate something when we expected to.
//	 */
//	while (buflen > 0) {
//		Mixer *m = nullptr;
//		const char *p = end - buflen;
//		unsigned resid = buflen;

//		/*
//		 * Use the next character as a hint to decide which mixer class to construct.
//		 */
//		switch (*p) {
//		case 'Z':
//			m = NullMixer::from_text(p, resid);
//			break;

//		case 'M':
//			m = SimpleMixer::from_text(_control_cb, _cb_handle, p, resid);
//			break;

//		case 'R':
//			m = MultirotorMixer::from_text(_control_cb, _cb_handle, p, resid);
//			break;

//		case 'H':
//			m = HelicopterMixer::from_text(_control_cb, _cb_handle, p, resid);
//			break;

//		default:
//			/* it's probably junk or whitespace, skip a byte and retry */
//			buflen--;
//			continue;
//		}

//		/*
//		 * If we constructed something, add it to the group.
//		 */
//		if (m != nullptr) {
//			add_mixer(m);

//			/* we constructed something */
//			ret = 0;

//			/* only adjust buflen if parsing was successful */
//			buflen = resid;
//			debug("SUCCESS - buflen: %d", buflen);

//		} else {

//			/*
//			 * There is data in the buffer that we expected to parse, but it didn't,
//			 * so give up for now.
//			 */
//			break;
//		}
//	}
//	/* nothing more in the buffer for us now */
//	return ret;
//}

//void MixerGroup::set_max_delta_out_once(float delta_out_max)
//{
//	Mixer	*mixer = _first;

//	while (mixer != nullptr) {
//		mixer->set_max_delta_out_once(delta_out_max);
//		mixer = mixer->_next;
//	}
//}

#if !defined(MIXER_REMOTE)
int16_t
MixerGroup::group_param_count()
{
	Mixer	*mixer = _first;
	int16_t param_count = 0;

	while ((mixer != nullptr)) {
		if (mixer->getBaseType() == Mixer::MIXER_BASE_TYPE_OBJECT) {
			MixerObject *mixobj = (MixerObject *) mixer;
			param_count += mixobj->parameter_count();
		}

		mixer = mixer->_next;
	}

	return param_count;
}


int16_t
MixerGroup::group_get_param(mixer_param_s *param)
{
//	Mixer	 *mixer = _first;
//	uint16_t remaining = param->index;
//	uint16_t mix_param_count;
//	param->mix_sub_index = -1;
//	param->mix_type = MIXER_TYPES_NONE;
//	strcpy(param->name, "NONE");
//	param->mix_index = 0;

//	while ((mixer != nullptr)) {
//		mix_param_count = mixer->parameter_count();

//		if (remaining < mix_param_count) {
//			param->param_index = remaining;
//			return mixer->get_parameter(param);
//		}

//		remaining -= mix_param_count;
//		param->mix_index++;
//		mixer = mixer->_next;
//	}

	return -1;
}


int16_t
MixerGroup::group_set_param(mixer_param_s *param)
{
//	Mixer	 *mixer = _first;
//	uint16_t remaining = param->index;
//	uint16_t mix_param_count;

//	param->mix_index = 0;

//	while ((mixer != nullptr)) {
//		mix_param_count = mixer->parameter_count();

//		if (remaining < mix_param_count) {
//			param->param_index = remaining;
//			return mixer->set_parameter(param);
//		}

//		remaining -= mix_param_count;
//		param->mix_index++;
//		mixer = mixer->_next;
//	}

	return -1;
}

#endif //MIXER_REMOTE

int16_t
MixerGroup::group_set_param_value(int16_t index, int16_t arrayIndex, float value)
{
//	Mixer	 *mixer = _first;
//	uint16_t remaining = index;
//	uint16_t mix_param_count;

//	while ((mixer != nullptr)) {
//		mix_param_count = mixer->parameter_count();

//		if (remaining < mix_param_count) {
//			return mixer->set_param_value(remaining, arrayIndex, value);
//		}

//		remaining -= mix_param_count;
//		mixer = mixer->_next;
//	}

	return -1;
}
