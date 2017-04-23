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
 * @file mixer_operators.cpp
 *
 * Generic mixer library.
 */

#include "mixer_operators.h"

#include <sys/types.h>
#include <stdint.h>
//#include <stdio.h>
#include <math.h>
#include <string.h>


#define debug(fmt, args...)	do { } while(0)
//#define debug(fmt, args...)	do { printf("[mixer] " fmt "\n", ##args); } while(0)


/****************************************************************************/

MixerAdd::MixerAdd(mixer_register_ref_s left, mixer_register_ref_s right, mixer_register_ref_s out)
	: Mixer()
	, _left(left)
	, _right(right)
	, _out(out)
{

}


MixerAdd::MixerAdd(mixer_data_operator_s *mixdata)
	: Mixer()
	, _left(mixdata->ref_left)
	, _right(mixdata->ref_right)
	, _out(mixdata->ref_out)
{
}

uint16_t
MixerAdd::mix(MixerRegisterGroups *reg_groups)
{
	float *dest = reg_groups->getFloatValue(_out);
	float *left = reg_groups->getFloatValue(_left);
	float *right = reg_groups->getFloatValue(_right);

	*dest = *left + *right;
	return 0;
}

bool
MixerAdd::mixerValid(MixerRegisterGroups *reg_groups)
{
	//Check input and output references
	if (!reg_groups->validRegister(&_left, true)) {
		return false;
	}

	if (!reg_groups->validRegister(&_right, true)) {
		return false;
	}

	if (!reg_groups->validRegister(&_out, true)) {
		return false;
	}

	return true;
}

uint16_t
MixerAdd::getMixerData(uint8_t *buff, int len)
{
	if (len < sizeof(mixer_data_operator_s)) {
		return -1;
	}

	memset(buff, 0, sizeof(mixer_data_operator_s));
	mixer_data_operator_s *data = (mixer_data_operator_s *) buff;
	data->header.mixer_type = MIXER_TYPES_ADD;
	data->header.data_size = sizeof(mixer_data_operator_s);
	data->ref_left = _left;
	data->ref_right = _right;
	data->ref_out = _out;
	return sizeof(mixer_data_operator_s);
}


/****************************************************************************/

MixerAddConst::MixerAddConst(mixer_register_ref_s in, mixer_register_val_u constval, mixer_register_ref_s out)
	: Mixer()
	, _in(in)
	, _constval(constval)
	, _out(out)
{
}

MixerAddConst::MixerAddConst(mixer_data_operator_s *mixdata)
	: Mixer()
	, _in(mixdata->ref_right)
	, _constval(mixdata->constval)
	, _out(mixdata->ref_out)
{
}

uint16_t
MixerAddConst::mix(MixerRegisterGroups *reg_groups)
{
	float *dest = reg_groups->getFloatValue(_out);
	float *value = reg_groups->getFloatValue(_in);

	*dest = *value + _constval.floatval;
	return 0;
}

uint16_t
MixerAddConst::getMixerData(uint8_t *buff, int len)
{
	if (len < sizeof(mixer_data_operator_s)) {
		return -1;
	}

	memset(buff, 0, sizeof(mixer_data_operator_s));
	mixer_data_operator_s *data = (mixer_data_operator_s *) buff;
	data->header.mixer_type = MIXER_TYPES_ADD_CONST;
	data->header.data_size = sizeof(mixer_data_operator_s);
	data->ref_right = _in;
	data->constval = _constval;
	data->ref_out = _out;
	return sizeof(mixer_data_operator_s);
}

bool
MixerAddConst::mixerValid(MixerRegisterGroups *reg_groups)
{
	//Check input and output references
	if (!reg_groups->validRegister(&_in, true)) {
		return false;
	}

	if (!reg_groups->validRegister(&_out, true)) {
		return false;
	}

	return true;
}


/****************************************************************************/

MixerCopy::MixerCopy(mixer_register_ref_s in, mixer_register_ref_s out)
	: Mixer()
	, _in(in)
	, _out(out)
{
}

MixerCopy::MixerCopy(mixer_data_operator_s *mixdata)
	: Mixer()
	, _in(mixdata->ref_right)
	, _out(mixdata->ref_out)
{
}

uint16_t
MixerCopy::mix(MixerRegisterGroups *reg_groups)
{
	float *dest = reg_groups->getFloatValue(_out);
	float *source = reg_groups->getFloatValue(_in);

	*dest = *source;
	return 0;
	return 0;
}


uint16_t
MixerCopy::getMixerData(uint8_t *buff, int len)
{
	if (len < sizeof(mixer_data_operator_s)) {
		return -1;
	}

	memset(buff, 0, sizeof(mixer_data_operator_s));
	mixer_data_operator_s *data = (mixer_data_operator_s *) buff;
	data->header.mixer_type = MIXER_TYPES_COPY;
	data->header.data_size = sizeof(mixer_data_operator_s);
	data->ref_right = _in;
	data->ref_out = _out;
	return sizeof(mixer_data_operator_s);
}

bool
MixerCopy::mixerValid(MixerRegisterGroups *reg_groups)
{
	//Check input and output references
	if (!reg_groups->validRegister(&_in, true)) {
		return false;
	}

	if (!reg_groups->validRegister(&_out, true)) {
		return false;
	}

	return true;
}

/****************************************************************************/

MixerMultiply::MixerMultiply(mixer_register_ref_s left, mixer_register_ref_s right, mixer_register_ref_s out)
	: Mixer()
	, _left(left)
	, _right(right)
	, _out(out)
{

}

MixerMultiply::MixerMultiply(mixer_data_operator_s *mixdata)
	: Mixer()
	, _left(mixdata->ref_left)
	, _right(mixdata->ref_right)
	, _out(mixdata->ref_out)
{
}

uint16_t
MixerMultiply::mix(MixerRegisterGroups *reg_groups)
{
	float *dest = reg_groups->getFloatValue(_out);
	float *left = reg_groups->getFloatValue(_left);
	float *right = reg_groups->getFloatValue(_right);

	*dest = *left * *right;
	return 0;
}

uint16_t
MixerMultiply::getMixerData(uint8_t *buff, int len)
{
	if (len < sizeof(mixer_data_operator_s)) {
		return -1;
	}

	memset(buff, 0, sizeof(mixer_data_operator_s));
	mixer_data_operator_s *data = (mixer_data_operator_s *) buff;
	data->header.mixer_type = MIXER_TYPES_MULTIPLY;
	data->header.data_size = sizeof(mixer_data_operator_s);
	data->ref_left = _left;
	data->ref_right = _right;
	data->ref_out = _out;
	return sizeof(mixer_data_operator_s);
}

bool
MixerMultiply::mixerValid(MixerRegisterGroups *reg_groups)
{
	//Check input and output references
	if (!reg_groups->validRegister(&_left, true)) {
		return false;
	}

	if (!reg_groups->validRegister(&_right, true)) {
		return false;
	}

	if (!reg_groups->validRegister(&_out, true)) {
		return false;
	}

	return true;
}
