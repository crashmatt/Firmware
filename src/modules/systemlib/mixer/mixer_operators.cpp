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
#include <ctype.h>
#include <systemlib/err.h>

#include "mixer_operators.h"

#define debug(fmt, args...)	do { } while(0)
//#define debug(fmt, args...)	do { printf("[mixer] " fmt "\n", ##args); } while(0)


/****************************************************************************/

MixerAdd::MixerAdd(mixer_data_operator_s *mixdata)
	: MixerOperator(mixdata)
{
}

uint16_t
MixerAdd::mix(MixerRegisterGroups *reg_groups, mixer_register_types_e type)
{
	mixer_data_operator_s *mixdata = (mixer_data_operator_s *) _mixdata;
	float *dest = reg_groups->getFloatValue(mixdata->ref_out);
	float *left = reg_groups->getFloatValue(mixdata->ref_left);
	float *right = reg_groups->getFloatValue(mixdata->ref_right);

	*dest = *left + *right;
//    printf("dest=%f left=%f right=%f\n", (double) *dest, (double) *left, (double) *right);
	return 0;
}


/****************************************************************************/

MixerAddConst::MixerAddConst(mixer_data_const_operator_s *mixdata)
	: MixerConstOperator(mixdata)
{
}

uint16_t
MixerAddConst::mix(MixerRegisterGroups *reg_groups, mixer_register_types_e type)
{
	mixer_data_const_operator_s *mixdata = (mixer_data_const_operator_s *) _mixdata;
	float *dest = reg_groups->getFloatValue(mixdata->ref_out);
	float *value = reg_groups->getFloatValue(mixdata->ref_in);

	*dest = *value + mixdata->constval.floatval;
//    printf("dest=%f value=%f constval=%f\n", (double) *dest, (double) *value, mixdata->constval.floatval);
	return 0;
}


/****************************************************************************/

MixerCopy::MixerCopy(mixer_data_operator_s *mixdata)
	: MixerOperator(mixdata)
{
}

uint16_t
MixerCopy::mix(MixerRegisterGroups *reg_groups, mixer_register_types_e type)
{
	mixer_data_operator_s *mixdata = (mixer_data_operator_s *) _mixdata;
	float *dest = reg_groups->getFloatValue(mixdata->ref_out);
	float *right = reg_groups->getFloatValue(mixdata->ref_right);

	*dest = *right;
	return 0;
	return 0;
}


/****************************************************************************/

MixerMultiply::MixerMultiply(mixer_data_operator_s *mixdata)
	: MixerOperator(mixdata)
{
}

uint16_t
MixerMultiply::mix(MixerRegisterGroups *reg_groups, mixer_register_types_e type)
{
	mixer_data_operator_s *mixdata = (mixer_data_operator_s *) _mixdata;
	float *dest = reg_groups->getFloatValue(mixdata->ref_out);
	float *left = reg_groups->getFloatValue(mixdata->ref_left);
	float *right = reg_groups->getFloatValue(mixdata->ref_right);

	*dest = *left * *right;
	return 0;
}
