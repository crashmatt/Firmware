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
 * @file mixer_functions.cpp
 *
 * Generic mixer library.
 */

#include "mixer_functions.h"

#include <sys/types.h>
#include <stdint.h>
//#include <stdio.h>
#include <math.h>


#define debug(fmt, args...)	do { } while(0)
//#define debug(fmt, args...)	do { printf("[mixer_functions] " fmt "\n", ##args); } while(0)


/****************************************************************************/

MixerMultipoint::MixerMultipoint(mixer_data_multipoint_s *mixdata)
	: Mixer()
	, _count(mixdata->count)
	, _in(mixdata->ref_in)
	, _in_vals(mixdata->in_vals)
	, _out_vals(mixdata->out_vals)
	, _out(mixdata->ref_out)
{
}

uint16_t
MixerMultipoint::mix(MixerRegisterGroups *reg_groups)
{
	float input = *reg_groups->getFloatValue(_in);
	float *invals = reg_groups->getFloatValue(_in_vals);
	float *outvals = reg_groups->getFloatValue(_out_vals);
	float *output = reg_groups->getFloatValue(_out);

	if (input <= invals[0]) {
		*output = outvals[0];
	}

	for (int i = 0; i < (_count - 1); i++) {
		if ((input >= invals[i]) && (input <= invals[i + 1])) {
			float inrange = invals[i + 1] - invals[i];

			// Prevent divide by zero or reversed input values
			if (inrange <= 0) {
				*output = outvals[i];
				return 0;
			}

			float outrange = outvals[i + 1] - outvals[i];
			*output = ((input - invals[i]) * outrange / inrange) + outvals[i];
			return 0;
		}
	}

	*output = outvals[_count - 1];
	return 0;
}

bool
MixerMultipoint::mixerValid(MixerRegisterGroups *reg_groups)
{
	//Check input and output references
	if (!reg_groups->validRegister(&_in, true)) {
		return false;
	}

	if (!reg_groups->validRegister(&_out, true)) {
		return false;
	}

	//Check start and end of input registers. Pressume inbetween is ok.
	if (!reg_groups->validRegister(&_in_vals, true)) {
		return false;
	}

	if (!reg_groups->validRegister(&_out_vals, true)) {
		return false;
	}

	mixer_register_ref_s ipref = _in_vals;
	mixer_register_ref_s opref = _out_vals;
	ipref.index += _count;
	opref.index += _count;

	if (!reg_groups->validRegister(&ipref, true)) {
		return false;
	}

	if (!reg_groups->validRegister(&opref, true)) {
		return false;
	}

	return true;
}
