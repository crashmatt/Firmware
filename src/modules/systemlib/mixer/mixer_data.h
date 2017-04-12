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
 * @file mixer_data.h
 *
 * Structure for holding resgisters which a mixer uses for input and output.
 * Holds structure information on regsiters so a mixer can validate correct registers
 * Supports mixer parameters for reporting register metadata
 */


#ifndef _SYSTEMLIB_MIXER_DATA_H
#define _SYSTEMLIB_MIXER_DATA_H value

#include <stdint.h>

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
	uint16_t     data_size;
	uint16_t     mixer_type;
} mixer_base_header_s;


typedef struct
__attribute__((packed))
{
	mixer_base_header_s     header;
	mixer_register_ref_s    ref_left;
	mixer_register_ref_s    ref_right;
	mixer_register_ref_s    ref_out;
} mixer_data_operator_s;

typedef struct
__attribute__((packed))
{
	mixer_base_header_s     header;
	mixer_register_ref_s    ref_in;
	mixer_register_val_u    constval;
	mixer_register_ref_s    ref_out;
} mixer_data_const_operator_s;

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
	mixer_base_header_s     header;
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


/****************************************************************************/
// Mixer datablock structures

typedef enum {
	MIXER_DATABLOCK_NONE = 0,
	MIXER_DATABLOCK_MIXERS,
	MIXER_DATABLOCK_GROUPS,
	MIXER_DATABLOCK_PARAMETERS,
	MIXER_DATABLOCK_PARAMETER_METADATA,
} mixer_datablocks_e;


typedef struct
__attribute__((packed))
{
	uint16_t    size;   //Size of the block not including the header.
	uint16_t    type;
} mixer_datablock_header_s;


/****************************************************************************/
// Mixer parameter structures


typedef struct
__attribute__((packed))
{
	uint16_t                parameter_count;
	uint16_t                parameter_value_count;
	mixer_register_val_u    values[0];
} mixer_parameters_s;

typedef struct
__attribute__((packed))
{
	uint16_t        array_size;
	char            name[16];
} mixer_parameter_metadata_s;


struct mixer_param_s {
	int16_t         index;
	int16_t         mix_index;
	int16_t         mix_sub_index;
	int16_t         param_index;
	float           values[6];
	char            name[17];
	int8_t          mix_type;
	int8_t          param_type;
	int8_t          array_size;
	uint8_t         flags;
};

#endif
