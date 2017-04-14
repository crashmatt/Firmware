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
 * @file mixer.cpp
 *
 * Generic mixer library.
 */

#include "mixer_config.h"
#include "mixer.h"

#include <stdint.h>
#include <stdlib.h>
//#include <string.h>

#define debug(fmt, args...)	do { } while(0)
//#define debug(fmt, args...)	do { printf("[mixer] " fmt "\n", ##args); } while(0)

/****************************************************************************/

Mixer::Mixer(mixer_base_header_s *mixdata)
	: _mixdata(mixdata)
{
}

/****************************************************************************/

MixerOperator::MixerOperator(mixer_data_operator_s *mixdata)
	: Mixer((mixer_base_header_s *) mixdata)
{
}

MixerOperator::~MixerOperator()
{
	if (_mixdata != nullptr) {
		free((mixer_data_operator_s *) _mixdata);
	}
}

bool
MixerOperator::mixerValid(MixerRegisterGroups *reg_groups)
{
	mixer_data_operator_s *oppdata = (mixer_data_operator_s *) _mixdata;

	if (reg_groups->validRegister(&oppdata->ref_left, true) == false) {
		return false;
	}

	if (reg_groups->validRegister(&oppdata->ref_right, true) == false) {
		return false;
	}

	if (reg_groups->validRegister(&oppdata->ref_out, false) == false) {
		return false;
	}

	return true;
}


/****************************************************************************/

MixerConstOperator::MixerConstOperator(mixer_data_const_operator_s *mixdata)
	: Mixer((mixer_base_header_s *) mixdata)
{
}

MixerConstOperator::~MixerConstOperator()
{
	if (_mixdata != nullptr) {
		free((mixer_data_const_operator_s *) _mixdata);
	}
}

bool
MixerConstOperator::mixerValid(MixerRegisterGroups *reg_groups)
{
	mixer_data_const_operator_s *oppdata = (mixer_data_const_operator_s *) _mixdata;

	if (reg_groups->validRegister(&oppdata->ref_in, true) == false) {
		return false;
	}

	if (reg_groups->validRegister(&oppdata->ref_out, false) == false) {
		return false;
	}

	return true;
}


/****************************************************************************/

MixerFunction::MixerFunction(mixer_data_function_s *mixdata)
	: Mixer((mixer_base_header_s *) mixdata)
{
}

MixerFunction::~MixerFunction()
{
	if (_mixdata != nullptr) {
		free((mixer_data_function_s *) _mixdata);
	}
}

bool
MixerFunction::mixerValid(MixerRegisterGroups *reg_groups)
{
	return false;
}


/****************************************************************************/


MixerObject::MixerObject(mixer_data_object_s *mixdata)
	: Mixer((mixer_base_header_s *) mixdata)
{
}
