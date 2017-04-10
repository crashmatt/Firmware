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

#include <px4_config.h>
#include "drivers/drv_mixer.h"

typedef enum {
	MIXER_REGISTER_TYPE_NONE = 0,
	MIXER_REGISTER_TYPE_FLOAT,
	MIXER_REGISTER_TYPE_UINT32,
	MIXER_REGISTER_TYPE_INT32,
	MIXER_REGISTER_TYPE_BOOL,
} mixer_register_types_e;

typedef union {
	float       floatval;
	uint32_t    uintval;
	bool        boolval;
	int32_t     intval;
} mixer_register_val_u;


typedef struct
__attribute__((packed))
{
	uint16_t    group;
	uint16_t    index;
} mixer_register_ref_s;

typedef union {
	mixer_register_ref_s ref;
	mixer_register_val_u val;
} mixer_register_u;

typedef struct
__attribute__((packed))
{
	uint16_t    data_size;
	uint8_t     base_type;
	uint8_t     mixer_type;
} mixer_base_header_s;


typedef struct
__attribute__((packed))
{
	mixer_base_header_s     base;
	mixer_register_ref_s    ref_left;
	mixer_register_ref_s    ref_right;
	mixer_register_ref_s    ref_out;
} mixer_data_operator_s;


typedef struct
__attribute__((packed))
{
	uint8_t     input_count;
	uint8_t     output_count;
	uint8_t     input_offset;    //Offset of input registers in data
	uint8_t     output_offset;   //Offset of output registers in data
} mixer_data_io_s;


typedef struct
__attribute__((packed))
{
	mixer_base_header_s     base;
	mixer_data_io_s         io;
} mixer_data_function_s;


typedef struct
__attribute__((packed))
{
	uint16_t    data_size;
	uint16_t    data_offset;
	uint8_t     data[0];
} mixer_data_s;


typedef struct
__attribute__((packed))
{
	mixer_base_header_s     base;
	mixer_data_io_s         io;
	mixer_data_s            data;
} mixer_data_object_s;


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

	bool                group_required;

protected:
	//* groupSize enables checks for access beyond the array maximum*/
	uint16_t             _group_size;
	mixer_register_val_u *_group_data;
	bool                 _read_only;
};

#define MIXER_REG_GROUP_NAMES {"CNTRLS_PRIMARY", "CONTROL1", "CONTROL2", "CONTROL3", "OUTPUTS", "PARAMETERS", "CONSTANTS", "STACK"}

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
		REGS_CONSTANTS,
		REGS_STACK,
		MIXER_REGISTER_GROUPS_MAX,
	} MIXER_REG_GROUPS;

	MixerRegisterGroups();

	float *getFloatValue(mixer_register_ref_s regref) {return &(register_groups[regref.group].groupData()[regref.index].floatval);}

	bool validRegister(mixer_register_ref_s *regref);


	MixerRegisterGroup  register_groups[MIXER_REGISTER_GROUPS_MAX];

#if !defined(MIXER_REMOTE)
	const char *getGroupName(int index);
#endif //MIXER_REMOTE


protected:
};




#endif
