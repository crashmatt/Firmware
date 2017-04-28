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

//#include <px4_config.h>

#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "mixer.h"
#include "mixer_factory.h"

#define debug(fmt, args...)	do { } while(0)
//#define debug(fmt, args...)	do { printf("[mixer] " fmt "\n", ##args); } while(0)
//#include <debug.h>
//#define debug(fmt, args...)	syslog(fmt "\n", ##args)

MixerGroup::MixerGroup()
	: _first(nullptr)
	, _reg_groups()
	, _parameters()
	, _variables()
{
}

MixerGroup::~MixerGroup()
{
	reset();
}

void
MixerGroup::append_mixer(Mixer *mixer)
{
	if (mixer == nullptr) {
		return;
	}

	Mixer **mpp;

	mpp = &_first;

	while (*mpp != nullptr) {
		mpp = &((*mpp)->_next);
	}

	*mpp = mixer;
	mixer->_next = nullptr;
}


int
MixerGroup::check_mixers_valid()
{
	Mixer *mix = _first;
	int index = 0;

	while (mix != nullptr) {
		if (mix->mixerValid(&_reg_groups) == false) {
			return -1 - index ;
		}

		mix = mix->_next;
		index++;
	}

	return 0;
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
		mixer->mix(&_reg_groups);
		mixer = mixer->_next;
	}

	return 0;
}

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

#if !defined(MIXER_REMOTE)
int16_t
MixerGroup::group_param_count()
{
//	Mixer	*mixer = _first;
	int16_t param_count = 0;

//	while ((mixer != nullptr)) {
//		if (mixer->getBaseType() == Mixer::MIXER_BASE_TYPE_OBJECT) {
//			MixerObject *mixobj = (MixerObject *) mixer;
//			param_count += mixobj->parameter_count();
//		}

//		mixer = mixer->_next;
//	}

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
