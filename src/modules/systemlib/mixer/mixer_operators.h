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
 * @file mixer_operators.h
 *
 * Generic, programmable, procedural control signal mixers.
 *
 * This library implements a generic mixer interface that can be used
 * by any driver or subsytem that wants to combine control signals
 * into one or more outputs.
 *
 * Terminology
 * ===========
 *
 * control value
 *	A mixer input value, typically provided by some controlling
 *	component of the system.
 *
 * control group
 * 	A collection of controls provided by a single controlling component.
 *
 * actuator
 *	The mixer output value.
 *
 *
 * Mixing basics
 * =============
 *
 *
 * Scaling
 * -------
 *
 *
 * Mixing
 * ------
 *
 * Mixing behaviour varies based on the specific mixer class; each
 * mixer class describes its behaviour in more detail.
 *
 *
 * Controls
 * --------
 *
 * The precise assignment of controls may vary depending on the
 * application, but the following assignments should be used
 * when appropriate.  Some mixer classes have specific assumptions
 * about the assignment of controls.
 *
 * control | standard meaning
 * --------+-----------------------
 *     0   | roll
 *     1   | pitch
 *     2   | yaw
 *     3   | primary thrust
 */


#ifndef _SYSTEMLIB_MIXER_OPERATORS_H
#define _SYSTEMLIB_MIXER_OPERATORS_H value

#include "mixer.h"

/****************************************************************************/

/**
 * Mixer for doing add operations
 */
class __EXPORT MixerAdd : public Mixer
{
public:
	MixerAdd(mixer_register_ref_s left, mixer_register_ref_s right, mixer_register_ref_s out);
	MixerAdd(mixer_data_operator_s *mixdata);

	uint16_t		mix(MixerRegisterGroups *reg_groups);
	uint16_t        getMixerData(uint8_t *buff, int len);
	bool            mixerValid(MixerRegisterGroups *reg_groups);
protected:
	mixer_register_ref_s _left;
	mixer_register_ref_s _right;
	mixer_register_ref_s _out;

private:
};


/****************************************************************************/

/**
 * Mixer for add constant to reference value
 */
class __EXPORT MixerAddConst : public Mixer
{
public:
	MixerAddConst(mixer_data_operator_s *mixdata);
	MixerAddConst(mixer_register_ref_s in, mixer_register_val_u constval, mixer_register_ref_s out);

	uint16_t		mix(MixerRegisterGroups *reg_groups);
	uint16_t        getMixerData(uint8_t *buff, int len);
	bool            mixerValid(MixerRegisterGroups *reg_groups);
protected:
	mixer_register_ref_s _in;
	mixer_register_val_u _constval;
	mixer_register_ref_s _out;
private:
};


/****************************************************************************/

/**
 * Mixer for copy values
 */
class __EXPORT MixerCopy : public Mixer
{
public:
	MixerCopy(mixer_register_ref_s in, mixer_register_ref_s out);
	MixerCopy(mixer_data_operator_s *mixdata);

	uint16_t		mix(MixerRegisterGroups *reg_groups);
	uint16_t        getMixerData(uint8_t *buff, int len);
	bool            mixerValid(MixerRegisterGroups *reg_groups);
protected:
	mixer_register_ref_s _in;
	mixer_register_ref_s _out;
private:
};


/****************************************************************************/

/**
 * Mixer for copy values
 */
class __EXPORT MixerMultiply : public Mixer
{
public:
	MixerMultiply(mixer_register_ref_s left, mixer_register_ref_s right, mixer_register_ref_s out);
	MixerMultiply(mixer_data_operator_s *mixdata);

	uint16_t		mix(MixerRegisterGroups *reg_groups);
	uint16_t        getMixerData(uint8_t *buff, int len);
	bool            mixerValid(MixerRegisterGroups *reg_groups);
protected:
	mixer_register_ref_s _left;
	mixer_register_ref_s _right;
	mixer_register_ref_s _out;
private:
};


/****************************************************************************/


#endif
