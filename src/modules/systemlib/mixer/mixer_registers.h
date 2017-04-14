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
 * @file mixer.h
 *
 * Structure for holding resgisters which a mixer uses for input and output.
 * Holds structure information on regsiters so a mixer can validate correct registers
 * Supports mixer parameters for reporting register metadata
 */


#ifndef _SYSTEMLIB_MIXER_REGISTERS_H
#define _SYSTEMLIB_MIXER_REGISTERS_H value

#include <stdint.h>
#include "mixer_data.h"

/**
 * Class containing a reference to a mixer register array and a size for the array
 *
 */
class __EXPORT MixerRegisterGroup
{
public:
	MixerRegisterGroup();

	uint16_t groupSize() {return _group_size;}

	void setGroup(unsigned group_size, mixer_register_val_u *group_data, bool read_only);

	mixer_register_val_u   *groupData() {return _group_data;}

	bool validRegister(uint16_t reg_index, bool read_only = true);

	bool                group_required;

protected:
	//* groupSize enables checks for access beyond the array maximum*/
	uint16_t             _group_size;
	mixer_register_val_u *_group_data;
	bool                 _read_only;
};

#define MIXER_REG_GROUP_NAMES {"CNTRLS_PRIMARY", "CONTROL1", "CONTROL2", "CONTROL3", "OUTPUTS", "PARAMETERS", "VARIABLES", "STACK"}

/**
 * Class containing groups of mixer registers
 *
 */
class __EXPORT MixerRegisterGroups
{
public:
	typedef enum {
		REGS_CONTROL_0     = 0,
		REGS_CONTROL_1,
		REGS_CONTROL_2,
		REGS_CONTROL_3,
		REGS_OUTPUTS,
		REGS_PARAMS,
		REGS_VARIABLES,
		REGS_STACK,
		MIXER_REGISTER_GROUPS_MAX,
	} MIXER_REG_GROUPS;

	MixerRegisterGroups();

	float *getFloatValue(mixer_register_ref_s regref) {return &(register_groups[regref.group].groupData()[regref.index].floatval);}

	bool validRegister(mixer_register_ref_s *regref, bool read_only = false);


	MixerRegisterGroup  register_groups[MIXER_REGISTER_GROUPS_MAX];

#if !defined(MIXER_REMOTE)
	const char *getGroupName(int index);
#endif //MIXER_REMOTE


protected:
};


#endif
