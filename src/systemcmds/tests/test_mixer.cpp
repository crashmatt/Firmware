/****************************************************************************
 *
 *   Copyright (c) 2013, 2017 PX4 Development Team. All rights reserved.
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
 * @file test_mixer.hpp
 *
 * Mixer load test
 */

#include <px4_config.h>

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <fstream>

#include <iostream>
#include <iterator>
#include <cstdio>

#include <systemlib/err.h>
#include <systemlib/mixer/mixers.h>
#include <systemlib/mixer/mixer_data_parser.h>
#include <systemlib/mixer/mixer_json_parser.h>
#include <systemlib/mixer/mixer_load.h>

#include <systemlib/pwm_limit/pwm_limit.h>
#include <drivers/drv_hrt.h>
#include <drivers/drv_pwm_output.h>
#include <px4iofirmware/mixer.h>
#include <px4iofirmware/protocol.h>

#include <uORB/topics/actuator_controls.h>
#include <uORB/topics/actuator_outputs.h>

#include "tests_main.h"

#include <unit_test/unit_test.h>

//#define debug(fmt, args...)	do { } while(0)
#define debug(fmt, args...)	do { printf("[test_mixer] " fmt "\n", ##args); } while(0)


//#define MOUNTPOINT PX4_ROOTFSDIR "/fs/microsd"
//static const char *kMixerJsonTestFile    = MOUNTPOINT "/test.json";

const unsigned output_max = 8;
//static float actuator_controls[output_max];
//static bool should_prearm = false;

#define NAN_VALUE 0.0f/0.0f

#ifdef __PX4_DARWIN
#define MIXER_DIFFERENCE_THRESHOLD 30
#else
#define MIXER_DIFFERENCE_THRESHOLD 2
#endif

#ifndef PATH_MAX
#ifdef __PX4_NUTTX
#define PATH_MAX 512
#else
#define PATH_MAX 4096
#endif
#endif

#if defined(CONFIG_ARCH_BOARD_SITL)
#define MIXER_PATH(_file)  "ROMFS/px4fmu_test/mixers/"#_file
#define MIXER_ONBOARD_PATH "ROMFS/px4fmu_common/mixers"
#else
#define MIXER_ONBOARD_PATH "/etc/mixers"
#define MIXER_PATH(_file) MIXER_ONBOARD_PATH"/"#_file
#endif


#define MIXER_VERBOSE

#define MIXER_BUFFER_SIZE 64

class MixerTest : public UnitTest
{
public:
	virtual bool run_tests();
	MixerTest();

private:
	bool mixerGroupFromDataTest();
	bool mixerParserTest();
	bool mixerJson11ParserTest();
	bool mixerPicoJsonParserTest();
	bool mixerBsonParserTest();
//	bool mixerTest();
//	bool loadIOPass();
//	bool loadVTOL1Test();
//	bool loadVTOL2Test();
//	bool loadQuadTest();
//	bool loadComplexTest();
//	bool loadAllTest();
//	bool load_mixer(const char *filename, unsigned expected_count, bool verbose = false);
//	bool load_mixer(const char *filename, const char *buf, unsigned loaded, unsigned expected_count,
//			const unsigned chunk_size, bool verbose);

	MixerGroup mixer_group;
};

MixerTest::MixerTest() : UnitTest(),
	mixer_group(nullptr)
{
}

bool MixerTest::run_tests()
{
	ut_run_test(mixerGroupFromDataTest);
	ut_run_test(mixerParserTest);
	ut_run_test(mixerJson11ParserTest);
	ut_run_test(mixerPicoJsonParserTest);
//	ut_run_test(loadQuadTest);
//	ut_run_test(loadVTOL1Test);
//	ut_run_test(loadVTOL2Test);
//	ut_run_test(loadComplexTest);
//	ut_run_test(loadAllTest);
//	ut_run_test(mixerTest);

	return (_tests_failed == 0);
}

ut_declare_test_c(test_mixer, MixerTest)

//bool MixerTest::loadIOPass()
//{
//	return load_mixer(MIXER_PATH(IO_pass.mix), 8);
//}

//bool MixerTest::loadQuadTest()
//{
//	return load_mixer(MIXER_PATH(quad_test.mix), 5);
//}

//bool MixerTest::loadVTOL1Test()
//{
//	return load_mixer(MIXER_PATH(vtol1_test.mix), 4);
//}

//bool MixerTest::loadVTOL2Test()
//{
//	return load_mixer(MIXER_PATH(vtol2_test.mix), 6);
//}

//bool MixerTest::loadComplexTest()
//{
//	return load_mixer(MIXER_PATH(complex_test.mix), 8);
//}

// Tests behaviour of MixerGroup and MixerFactory when creating mixers from data
bool MixerTest::mixerGroupFromDataTest()
{
	actuator_controls_s _controls[actuator_controls_s::NUM_ACTUATOR_CONTROL_GROUPS];
	actuator_outputs_s outputs = {};
	size_t num_outputs = 16;

	uint8_t *mixbuff = (uint8_t *) malloc(MIXER_BUFFER_SIZE);
	uint8_t *buffpos = mixbuff;

	MixerRegisterGroups _reg_groups = MixerRegisterGroups();
	mixer_group.setRegGroups(&_reg_groups);
	mixer_group.reset();

	_reg_groups.register_groups[MixerRegisterGroups::REGS_CONTROL_0].setGroup(actuator_controls_s::NUM_ACTUATOR_CONTROLS,
			(mixer_register_val_u *) _controls[0].control, true);

	_reg_groups.register_groups[MixerRegisterGroups::REGS_OUTPUTS].setGroup(actuator_outputs_s::NUM_ACTUATOR_OUTPUTS,
			(mixer_register_val_u *) outputs.output, false);

	mixer_data_operator_s *oppdata = (mixer_data_operator_s *) buffpos;

	int expected_count = 0;
	actuator_outputs_s expected_outputs = {};

	/* default ports to disabled by setting output to NaN */
	for (size_t i = 0; i < sizeof(outputs.output) / sizeof(outputs.output[0]); i++) {
		if (i >= num_outputs) {
			outputs.output[i] = NAN;
			expected_outputs.output[i] = NAN;
		}
	}

	// Binary scaled inputs for testable results
	_controls[0].control[0] = 0.5;
	_controls[0].control[1] = 0.25;
	_controls[0].control[2] = 0.125;
	_controls[0].control[3] = 0.625;
	_controls[0].control[4] = 0.3125;
	_controls[0].control[5] = 0.015625;
	_controls[0].control[6] = 0.0078125;
	_controls[0].control[7] = 0.00390625;

	//ADD
	memset(mixbuff, 0, MIXER_BUFFER_SIZE);
	oppdata->header.mixer_type = MIXER_TYPES_ADD;
	oppdata->header.data_size = sizeof(mixer_data_operator_s);
	oppdata->ref_left.group = MixerRegisterGroups::REGS_CONTROL_0;
	oppdata->ref_left.index = 0;
	oppdata->ref_right.group = MixerRegisterGroups::REGS_CONTROL_0;
	oppdata->ref_right.index = 1;
	oppdata->ref_out.group = MixerRegisterGroups::REGS_OUTPUTS;
	oppdata->ref_out.index = 0;

	if (mixer_group.from_buffer(mixbuff, sizeof(mixer_data_operator_s)) == -1) {
		return false;
	}

	expected_count++;
	expected_outputs.output[0] = 0.75;

	//COPY
	memset(mixbuff, 0, MIXER_BUFFER_SIZE);
	oppdata->header.mixer_type = MIXER_TYPES_COPY;
	oppdata->header.data_size = sizeof(mixer_data_operator_s);
	oppdata->ref_left.group = 0x00;
	oppdata->ref_left.index = 0x00;
	oppdata->ref_right.group = MixerRegisterGroups::REGS_CONTROL_0;
	oppdata->ref_right.index = 0;
	oppdata->ref_out.group = MixerRegisterGroups::REGS_OUTPUTS;
	oppdata->ref_out.index = 1;

	if (mixer_group.from_buffer(mixbuff, sizeof(mixer_data_operator_s)) == -1) {
		return false;
	}

	expected_count++;
	expected_outputs.output[1] = 0.5;

	//MULTIPLY
	memset(mixbuff, 0, MIXER_BUFFER_SIZE);
	oppdata->header.mixer_type = MIXER_TYPES_MULTIPLY;
	oppdata->header.data_size = sizeof(mixer_data_operator_s);
	oppdata->ref_left.group = MixerRegisterGroups::REGS_CONTROL_0;
	oppdata->ref_left.index = 0;
	oppdata->ref_right.group = MixerRegisterGroups::REGS_CONTROL_0;
	oppdata->ref_right.index = 1;
	oppdata->ref_out.group = MixerRegisterGroups::REGS_OUTPUTS;
	oppdata->ref_out.index = 2;

	if (mixer_group.from_buffer(mixbuff, sizeof(mixer_data_operator_s)) == -1) {
		return false;
	}

	expected_count++;
	expected_outputs.output[2] = 0.125;

	//ADD CONSTANT
	memset(mixbuff, 0, MIXER_BUFFER_SIZE);
	mixer_data_const_operator_s *oppcdata = (mixer_data_const_operator_s *) buffpos;
	oppcdata->header.mixer_type = MIXER_TYPES_ADD_CONST;
	oppcdata->header.data_size = sizeof(mixer_data_const_operator_s);
	oppcdata->constval.floatval = 0.75;
	oppcdata->ref_in.group = MixerRegisterGroups::REGS_CONTROL_0;
	oppcdata->ref_in.index = 0;
	oppcdata->ref_out.group = MixerRegisterGroups::REGS_OUTPUTS;
	oppcdata->ref_out.index = 3;

	if (mixer_group.from_buffer(mixbuff, sizeof(mixer_data_const_operator_s)) == -1) {
		return false;
	}

	expected_count++;
	expected_outputs.output[3] = 1.25;

	memset(mixbuff, 0, MIXER_BUFFER_SIZE);
	free(mixbuff);

	if (mixer_group.count() != 4) {
		debug("mixerGroupFromDataTest: MixerGroup has wrong mixer count");
		return false;
	}

	int valid = mixer_group.check_mixers_valid();

	if (valid != 0) {
		debug("mixerGroupFromDataTest: Mixers not valid at mixer:%u", 1 - valid);
		return false;
	}

	//Do mixing
	mixer_group.mix_group();

	for (size_t i = 0; i < sizeof(outputs.output) / sizeof(outputs.output[0]); i++) {
		if (outputs.output[i] != expected_outputs.output[i]) {
			debug("mixerGroupFromDataTest: Output not correct for output:%u", i);
			return false;
		}
	}

	return true;
}

// tests behaviour of mixer parser when creating/setting data from data blocks
bool MixerTest::mixerParserTest()
{
	actuator_controls_s _controls[actuator_controls_s::NUM_ACTUATOR_CONTROL_GROUPS];
	actuator_outputs_s outputs = {};
	size_t num_outputs = 8;

	MixerRegisterGroups _reg_groups = MixerRegisterGroups();
	mixer_group.setRegGroups(&_reg_groups);
	mixer_group.reset();

	_reg_groups.register_groups[MixerRegisterGroups::REGS_CONTROL_0].setGroup(actuator_controls_s::NUM_ACTUATOR_CONTROLS,
			(mixer_register_val_u *) _controls[0].control, true);

	_reg_groups.register_groups[MixerRegisterGroups::REGS_CONTROL_1].setGroup(actuator_controls_s::NUM_ACTUATOR_CONTROLS,
			(mixer_register_val_u *) _controls[1].control, true);

	_reg_groups.register_groups[MixerRegisterGroups::REGS_CONTROL_2].setGroup(actuator_controls_s::NUM_ACTUATOR_CONTROLS,
			(mixer_register_val_u *) _controls[2].control, true);

	_reg_groups.register_groups[MixerRegisterGroups::REGS_CONTROL_3].setGroup(actuator_controls_s::NUM_ACTUATOR_CONTROLS,
			(mixer_register_val_u *) _controls[3].control, true);

	_reg_groups.register_groups[MixerRegisterGroups::REGS_OUTPUTS].setGroup(actuator_outputs_s::NUM_ACTUATOR_OUTPUTS,
			(mixer_register_val_u *) outputs.output, false);

	debug("size of Mixer:%u", sizeof(Mixer));
	debug("size of MixerOperator:%u", sizeof(MixerOperator));
	debug("size of MixerAdd:%u", sizeof(MixerAdd));
	debug("size of mixer_register_val_u:%u", sizeof(mixer_register_val_u));
	debug("size of mixer_register_ref_s:%u", sizeof(mixer_register_ref_s));
	debug("size of mixer_data_operator_s:%u", sizeof(mixer_data_operator_s));

	uint8_t *mixbuff = (uint8_t *) malloc(256);

	MixerParameters mixparams   = MixerParameters();
	MixerVariables  mixvars     = MixerVariables();

	MixerDataParser mixparser = MixerDataParser(&mixer_group, &mixparams, &_reg_groups, &mixvars);
	mixer_datablock_header_s *hdr = (mixer_datablock_header_s *) mixbuff;

	int expected_mixer_count = 0;

	// Create and parse variables datablock
	hdr = (mixer_datablock_header_s *) mixbuff;
	hdr->start = MIXER_DATABLOCK_START;
	hdr->type = MIXER_DATABLOCK_VARIABLE_COUNT;
	hdr->size = sizeof(mixer_variables_s);
	mixer_variables_s *vars_size = (mixer_variables_s *) hdr->data;
	vars_size->variable_count = 4;
	debug("Parse variables datablock");
	mixparser.parse_buffer(mixbuff, sizeof(mixer_datablock_header_s) + hdr->size);

	debug("Assign varraibles to register group");
	_reg_groups.register_groups[MixerRegisterGroups::REGS_VARIABLES].setGroup(mixvars.count(),
			mixvars.variables(), false);


	// Create parameters datablock
	hdr = (mixer_datablock_header_s *) mixbuff;
	hdr->start = MIXER_DATABLOCK_START;
	hdr->type = MIXER_DATABLOCK_PARAMETERS;
	hdr->size = sizeof(mixer_parameters_s);
	mixer_parameters_s *params_size = (mixer_parameters_s *) hdr->data;
	params_size->parameter_count = 1;
	params_size->parameter_value_count = 1;
	debug("Parse params datablock\n");
	mixparser.parse_buffer(mixbuff, sizeof(mixer_datablock_header_s) + hdr->size);

	// Create parameter value datablock
	hdr = (mixer_datablock_header_s *) mixbuff;
	hdr->start = MIXER_DATABLOCK_START;
	hdr->type = MIXER_DATABLOCK_PARAM_VALUES;
	mixer_param_values_s *valdata = (mixer_param_values_s *) hdr->data;
	mixer_register_val_u *regval;
	valdata->value_index = 0;
	valdata->value_count = 1;
	//set header size to value count
	hdr->size = sizeof(mixer_param_values_s) + (valdata->value_count * sizeof(mixer_register_val_u)); //One value only
	regval = (mixer_register_val_u *) valdata->values;
	regval->floatval = 0.66666;
	debug("Parse param values datablock");
	mixparser.parse_buffer(mixbuff, sizeof(mixer_datablock_header_s) + hdr->size);
	// Assign parameters to parameter register group

	debug("Assign params to param register group");
	_reg_groups.register_groups[MixerRegisterGroups::REGS_PARAMS].setGroup(mixparams.paramCount(),
			mixparams.paramValues(), true);

	// Create parameter metadata datablock
	hdr = (mixer_datablock_header_s *) mixbuff;
	hdr->start = MIXER_DATABLOCK_START;
	hdr->type = MIXER_DATABLOCK_PARAMETER_METADATA;
	hdr->size = sizeof(mixer_parameter_metadata_s);
	mixer_parameter_metadata_s *metadata = (mixer_parameter_metadata_s *) hdr->data;
	metadata->param_index = 0;
	metadata->array_size = 1;
	strncpy(metadata->name, "PARAM_1", 16);
	debug("Parse parameter metadata datablock");
	mixparser.parse_buffer(mixbuff, sizeof(mixer_datablock_header_s) + hdr->size);

	//TEST SINGLE MIXER DATABLOCK
	hdr = (mixer_datablock_header_s *) mixbuff;
	hdr->start = MIXER_DATABLOCK_START;
	hdr->type = MIXER_DATABLOCK_MIXER;
	hdr->size = sizeof(mixer_data_operator_s);
	mixer_data_operator_s *oppdata = (mixer_data_operator_s *) hdr->data;
	oppdata->header.mixer_type = MIXER_TYPES_ADD;
	oppdata->header.data_size = sizeof(mixer_data_operator_s);
	oppdata->ref_left.group = MixerRegisterGroups::REGS_CONTROL_0;
	oppdata->ref_left.index = actuator_controls_s::INDEX_ROLL;
	oppdata->ref_right.group = MixerRegisterGroups::REGS_CONTROL_0;
	oppdata->ref_right.index = actuator_controls_s::INDEX_PITCH;
	oppdata->ref_out.group = MixerRegisterGroups::REGS_OUTPUTS;
	oppdata->ref_out.index = 0;
	expected_mixer_count++;
	debug("Parse mixer values datablock");
	mixparser.parse_buffer(mixbuff, sizeof(mixer_datablock_header_s) + hdr->size);


	//TEST TWO MIXER DATABLOCKS IN ONE BUFFER LENGTH
	int totalsize = 0;
	hdr = (mixer_datablock_header_s *) mixbuff;
	// Copy Control to output
	hdr->start = MIXER_DATABLOCK_START;
	hdr->type = MIXER_DATABLOCK_MIXER;
	hdr->size = sizeof(mixer_data_operator_s);
	oppdata = (mixer_data_operator_s *) hdr->data;
	oppdata->header.mixer_type = MIXER_TYPES_COPY;
	oppdata->header.data_size = sizeof(mixer_data_operator_s);
	oppdata->ref_left.group = 0x00;
	oppdata->ref_left.index = 0x00;
	oppdata->ref_right.group = MixerRegisterGroups::REGS_CONTROL_0;
	oppdata->ref_right.index = actuator_controls_s::INDEX_PITCH;
	oppdata->ref_out.group = MixerRegisterGroups::REGS_OUTPUTS;
	oppdata->ref_out.index = 1;
	expected_mixer_count++;
	// move header to next buffer location
	totalsize = sizeof(mixer_datablock_header_s) + hdr->size;
	hdr = (mixer_datablock_header_s *)(mixbuff + totalsize);
	// Add control inputs to output
	hdr->start = MIXER_DATABLOCK_START;
	hdr->type = MIXER_DATABLOCK_MIXER;
	hdr->size = sizeof(mixer_data_operator_s);
	oppdata = (mixer_data_operator_s *) hdr->data;
	oppdata->header.mixer_type = MIXER_TYPES_ADD;
	oppdata->header.data_size = sizeof(mixer_data_operator_s);
	oppdata->ref_left.group = MixerRegisterGroups::REGS_CONTROL_0;
	oppdata->ref_left.index = actuator_controls_s::INDEX_ROLL;
	oppdata->ref_right.group = MixerRegisterGroups::REGS_CONTROL_0;
	oppdata->ref_right.index = actuator_controls_s::INDEX_YAW;
	oppdata->ref_out.group = MixerRegisterGroups::REGS_OUTPUTS;
	oppdata->ref_out.index = 2;
	expected_mixer_count++;
	// parse result
	totalsize += (sizeof(mixer_datablock_header_s) + hdr->size);
	debug("Parse double mixer datablocks");
	mixparser.parse_buffer(mixbuff, totalsize);


	//TEST ???
	hdr = (mixer_datablock_header_s *) mixbuff;
	hdr->start = MIXER_DATABLOCK_START;
	hdr->type = MIXER_DATABLOCK_MIXER;
	hdr->size = sizeof(mixer_data_operator_s);
	totalsize = hdr->size;
	//Multiply control with parameter to output
	oppdata = (mixer_data_operator_s *) hdr->data;
	oppdata->header.mixer_type = MIXER_TYPES_MULTIPLY;
	oppdata->header.data_size = sizeof(mixer_data_operator_s);
	oppdata->ref_left.group = MixerRegisterGroups::REGS_CONTROL_0;
	oppdata->ref_left.index = actuator_controls_s::INDEX_THROTTLE;
	oppdata->ref_right.group = MixerRegisterGroups::REGS_PARAMS;
	oppdata->ref_right.index = 0;
	oppdata->ref_out.group = MixerRegisterGroups::REGS_OUTPUTS;
	oppdata->ref_out.index = 3;
	expected_mixer_count++;

	debug("Parse multi mixers in single datablock with datasize:%u\n", hdr->size);
	mixparser.parse_buffer(mixbuff, sizeof(mixer_datablock_header_s) + hdr->size);

	// Clear memory biffer to check nothing is using it
	memset(mixbuff, 0, 256);
	free(mixbuff);

	if (mixer_group.count() != expected_mixer_count) {
		debug("Mixer count expected:%u but got:%u\n", expected_mixer_count, mixer_group.count());
		return false;
	}

	int errindex =  mixer_group.check_mixers_valid();

	if (errindex != 0) {
		debug("Mixers %u is not valid\n", 1 - errindex);
		return false;
	}

	// Binary scaled inputs for clear add results
	_controls[0].control[0] = 0.01;
	_controls[0].control[1] = 0.02;
	_controls[0].control[2] = 0.04;
	_controls[0].control[3] = 0.08;
	_controls[0].control[4] = 0.16;
	_controls[0].control[5] = 0.32;
	_controls[0].control[6] = 0.64;
	_controls[0].control[7] = 0.128;

	/* default ports to disabled by setting output to NaN */
	for (size_t i = 0; i < sizeof(outputs.output) / sizeof(outputs.output[0]); i++) {
		if (i >= num_outputs) {
			outputs.output[i] = NAN;
		}
	}

	/* do mixing */
	mixer_group.mix_group();
	outputs.timestamp = hrt_absolute_time();

	for (size_t i = 0; i < sizeof(outputs.output) / sizeof(outputs.output[0]); i++) {
		if (outputs.output[i] != NAN) {
			if (i < 6) {
				debug("OP[%u]=%0.2f ", i, (double) outputs.output[i]);
			}

			num_outputs = i + 1;
		}
	}

	return true;
}

// tests behaviour of json11 parser
bool MixerTest::mixerJson11ParserTest()
{
	actuator_controls_s _controls[actuator_controls_s::NUM_ACTUATOR_CONTROL_GROUPS];
	actuator_outputs_s outputs = {};

	MixerRegisterGroups _reg_groups = MixerRegisterGroups();
	mixer_group.setRegGroups(&_reg_groups);
	mixer_group.reset();

	_reg_groups.register_groups[MixerRegisterGroups::REGS_CONTROL_0].setGroup(actuator_controls_s::NUM_ACTUATOR_CONTROLS,
			(mixer_register_val_u *) _controls[0].control, true);

	_reg_groups.register_groups[MixerRegisterGroups::REGS_CONTROL_1].setGroup(actuator_controls_s::NUM_ACTUATOR_CONTROLS,
			(mixer_register_val_u *) _controls[1].control, true);

	_reg_groups.register_groups[MixerRegisterGroups::REGS_CONTROL_2].setGroup(actuator_controls_s::NUM_ACTUATOR_CONTROLS,
			(mixer_register_val_u *) _controls[2].control, true);

	_reg_groups.register_groups[MixerRegisterGroups::REGS_CONTROL_3].setGroup(actuator_controls_s::NUM_ACTUATOR_CONTROLS,
			(mixer_register_val_u *) _controls[3].control, true);

	_reg_groups.register_groups[MixerRegisterGroups::REGS_OUTPUTS].setGroup(actuator_outputs_s::NUM_ACTUATOR_OUTPUTS,
			(mixer_register_val_u *) outputs.output, false);

	MixerParameters mixparams   = MixerParameters();
	MixerVariables  mixvars     = MixerVariables();

	MixerDataParser mixparser = MixerDataParser(&mixer_group, &mixparams, &_reg_groups, &mixvars);

	MixerJsonParser parser;
	parser.setDataParser(&mixparser);

	FILE    *fp;
	char    buff[2048];

	/* open the mixer definition file */
	fp = fopen(MIXER_PATH(mixer.json), "r");

	if (fp == nullptr) {
		debug("file not found");
		return false;
	}

	size_t newLen = fread(buff, sizeof(char), 2046, fp);

	if (newLen == 0) {
		fputs("Error reading file", stderr);
		fclose(fp);
		return false;
	}

	buff[newLen] = '\0'; /* Just to be safe. */

	parser.parse_json(buff, 2046);

	return true;
}

// tests behaviour of mixer parser when creating/setting data from data blocks
bool MixerTest::mixerBsonParserTest()
{
//	MixerJsonParser parser;
//	int		fd;
////    FILE    *fp;

//	/* open the mixer definition file */
//	fd = open(MIXER_PATH(IO_pass.mix), O_RDONLY);
////    fp = fopen(MIXER_PATH(IO_pass.mix), "r");

//	if (fd < 0) {
//		debug("file not found");
//		return false;
//	}

//    parser.parse_bson(fd);

//	close(fd);
	return false;
}

// tests behaviour of cjosn parser
bool MixerTest::mixerPicoJsonParserTest()
{
	MixerJsonParser parser;

	/* open the mixer definition file */
//	fd = open(MIXER_PATH(IO_pass.mix), O_RDONLY);
//    fp = fopen(MIXER_PATH(mixer.json), "r");
	std::ifstream fs(MIXER_PATH(mixer.json));

	if (!fs) {
		debug("Could not open file at %s", MIXER_PATH(mixer.json));
		return false;
	}

	if (!fs.is_open()) {
		return false;
	}

	parser.parse_picojson(&fs);

	return true;
}

//bool MixerTest::loadAllTest()
//{
//	PX4_INFO("Testing all mixers in %s", MIXER_ONBOARD_PATH);

//	DIR *dp = opendir(MIXER_ONBOARD_PATH);

//	if (dp == nullptr) {
//		PX4_ERR("File open failed");
//		// this is not an FTP error, abort directory by simulating eof
//		return false;
//	}

//	struct dirent *result = nullptr;

//	// move to the requested offset
//	//seekdir(dp, payload->offset);

//	for (;;) {
//		errno = 0;
//		result = readdir(dp);

//		// read the directory entry
//		if (result == nullptr) {
//			if (errno) {
//				PX4_ERR("readdir failed");
//				closedir(dp);

//				return false;
//			}

//			// We are just at the last directory entry
//			break;
//		}

//		// Determine the directory entry type
//		switch (result->d_type) {
//#ifdef __PX4_NUTTX

//		case DTYPE_FILE:
//#else
//		case DT_REG:
//#endif
//			if (strncmp(result->d_name, ".", 1) != 0) {

//				char buf[PATH_MAX];
//				(void)strncpy(&buf[0], MIXER_ONBOARD_PATH, sizeof(buf) - 1);
//				/* enforce null termination */
//				buf[sizeof(buf) - 1] = '\0';
//				(void)strncpy(&buf[strlen(MIXER_ONBOARD_PATH)], "/", 1);
//				(void)strncpy(&buf[strlen(MIXER_ONBOARD_PATH) + 1], result->d_name, sizeof(buf) - strlen(MIXER_ONBOARD_PATH) - 1);

//				bool ret = load_mixer(buf, 0);

//				if (!ret) {
//					PX4_ERR("Error testing mixer %s", buf);
//					return false;
//				}
//			}

//			break;

//		default:
//			break;
//		}
//	}

//	closedir(dp);

//	return true;
//}

//bool MixerTest::load_mixer(const char *filename, unsigned expected_count, bool verbose)
//{
//	char buf[2048];

//	load_mixer_file(filename, &buf[0], sizeof(buf));
//	unsigned loaded = strlen(buf);

//	if (verbose) {
//		PX4_INFO("loaded: \n\"%s\"\n (file: %s, %d chars)", &buf[0], filename, loaded);
//	}

//	// Test a number of chunk sizes
//	for (unsigned chunk_size = 6; chunk_size < PX4IO_MAX_TRANSFER_LEN + 1; chunk_size++) {
//		bool ret = load_mixer(filename, buf, loaded, expected_count, chunk_size, verbose);

//		if (!ret) {
//			PX4_ERR("Mixer load failed with chunk size %u", chunk_size);
//			return ret;
//		}
//	}

//	return true;
//}

//bool MixerTest::load_mixer(const char *filename, const char *buf, unsigned loaded, unsigned expected_count,
//			   const unsigned chunk_size,
//			   bool verbose)
//{
//	/* load the mixer in chunks, like
//	 * in the case of a remote load,
//	 * e.g. on PX4IO.
//	 */

//	/* load at once test */
//	unsigned xx = loaded;
//	mixer_group.reset();
//	mixer_group.load_from_buf(&buf[0], xx);

//	if (expected_count > 0) {
//		ut_compare("check number of mixers loaded", mixer_group.count(), expected_count);
//	}

//	unsigned empty_load = 2;
//	char empty_buf[2];
//	empty_buf[0] = ' ';
//	empty_buf[1] = '\0';
//	mixer_group.reset();
//	mixer_group.load_from_buf(&empty_buf[0], empty_load);

//	if (verbose) {
//		PX4_INFO("empty buffer load: loaded %u mixers, used: %u", mixer_group.count(), empty_load);
//	}

//	ut_compare("empty buffer load", empty_load, 0);

//	/* reset, load in chunks */
//	mixer_group.reset();
//	char mixer_text[PX4IO_MAX_MIXER_LENGHT];		/* large enough for one mixer */

//	unsigned mixer_text_length = 0;
//	unsigned transmitted = 0;
//	unsigned resid = 0;

//	while (transmitted < loaded) {

//		unsigned text_length = (loaded - transmitted > chunk_size) ? chunk_size : loaded - transmitted;

//		/* check for overflow - this would be really fatal */
//		if ((mixer_text_length + text_length + 1) > sizeof(mixer_text)) {
//			PX4_ERR("Mixer text length overflow for file: %s. Is PX4IO_MAX_MIXER_LENGHT too small? (curr len: %d)", filename,
//				PX4IO_MAX_MIXER_LENGHT);
//			return false;
//		}

//		/* append mixer text and nul-terminate */
//		memcpy(&mixer_text[mixer_text_length], &buf[transmitted], text_length);
//		mixer_text_length += text_length;
//		mixer_text[mixer_text_length] = '\0';
//		//fprintf(stderr, "buflen %u, text:\n\"%s\"\n", mixer_text_length, &mixer_text[0]);

//		/* process the text buffer, adding new mixers as their descriptions can be parsed */
//		resid = mixer_text_length;
//		mixer_group.load_from_buf(&mixer_text[0], resid);

//		/* if anything was parsed */
//		if (resid != mixer_text_length) {
//			//PX4_INFO("loaded %d mixers, used %u\n", mixer_group.count(), mixer_text_length - resid);

//			/* copy any leftover text to the base of the buffer for re-use */
//			if (resid > 0) {
//				memmove(&mixer_text[0], &mixer_text[mixer_text_length - resid], resid);
//				/* enforce null termination */
//				mixer_text[resid] = '\0';
//			}

//			mixer_text_length = resid;
//		}

//		transmitted += text_length;

//		if (verbose) {
//			PX4_INFO("transmitted: %d, loaded: %d", transmitted, loaded);
//		}
//	}

//	if (verbose) {
//		PX4_INFO("chunked load: loaded %u mixers", mixer_group.count());
//	}

//	if (expected_count > 0 && mixer_group.count() != expected_count) {
//		PX4_ERR("Load of mixer failed, last chunk: %s, transmitted: %u, text length: %u, resid: %u", mixer_text, transmitted,
//			mixer_text_length, resid);
//		ut_compare("check number of mixers loaded (chunk)", mixer_group.count(), expected_count);
//	}

//	return true;
//}

//bool MixerTest::mixerTest()
//{
//	/*
//	 * PWM limit structure
//	 */
//	pwm_limit_t pwm_limit;
//	bool should_arm = false;
//	uint16_t r_page_servo_disarmed[output_max];
//	uint16_t r_page_servo_control_min[output_max];
//	uint16_t r_page_servo_control_max[output_max];
//	uint16_t r_page_servos[output_max];
//	uint16_t servo_predicted[output_max];
//	int16_t reverse_pwm_mask = 0;

//	bool load_ok = load_mixer(MIXER_PATH(IO_pass.mix), 8);

//	if (!load_ok) {
//		return load_ok;
//	}

//	/* execute the mixer */

//	float	outputs[output_max];
//	unsigned mixed;
//	const int jmax = 5;

//	pwm_limit_init(&pwm_limit);

//	/* run through arming phase */
//	for (unsigned i = 0; i < output_max; i++) {
//		actuator_controls[i] = 0.1f;
//		r_page_servo_disarmed[i] = PWM_MOTOR_OFF;
//		r_page_servo_control_min[i] = PWM_DEFAULT_MIN;
//		r_page_servo_control_max[i] = PWM_DEFAULT_MAX;
//	}

//	//PX4_INFO("PRE-ARM TEST: DISABLING SAFETY");

//	/* mix */
//	should_prearm = true;
//	mixed = mixer_group.mix(&outputs[0], output_max, nullptr);

//	pwm_limit_calc(should_arm, should_prearm, mixed, reverse_pwm_mask, r_page_servo_disarmed, r_page_servo_control_min,
//		       r_page_servo_control_max, outputs, r_page_servos, &pwm_limit);

//	//warnx("mixed %d outputs (max %d), values:", mixed, output_max);
//	for (unsigned i = 0; i < mixed; i++) {

//		//fprintf(stderr, "pre-arm:\t %d: out: %8.4f, servo: %d \n", i, (double)outputs[i], (int)r_page_servos[i]);

//		if (i != actuator_controls_s::INDEX_THROTTLE) {
//			if (r_page_servos[i] < r_page_servo_control_min[i]) {
//				warnx("active servo < min");
//				return false;
//			}

//		} else {
//			if (r_page_servos[i] != r_page_servo_disarmed[i]) {
//				warnx("throttle output != 0 (this check assumed the IO pass mixer!)");
//				return false;
//			}
//		}
//	}

//	should_arm = true;
//	should_prearm = false;

//	/* simulate another orb_copy() from actuator controls */
//	for (unsigned i = 0; i < output_max; i++) {
//		actuator_controls[i] = 0.1f;
//	}

//	//PX4_INFO("ARMING TEST: STARTING RAMP");
//	unsigned sleep_quantum_us = 10000;

//	hrt_abstime starttime = hrt_absolute_time();
//	unsigned sleepcount = 0;

//	while (hrt_elapsed_time(&starttime) < INIT_TIME_US + RAMP_TIME_US + 2 * sleep_quantum_us) {

//		/* mix */
//		mixed = mixer_group.mix(&outputs[0], output_max, nullptr);

//		pwm_limit_calc(should_arm, should_prearm, mixed, reverse_pwm_mask, r_page_servo_disarmed, r_page_servo_control_min,
//			       r_page_servo_control_max, outputs, r_page_servos, &pwm_limit);

//		//warnx("mixed %d outputs (max %d), values:", mixed, output_max);
//		for (unsigned i = 0; i < mixed; i++) {

//			//fprintf(stderr, "ramp:\t %d: out: %8.4f, servo: %d \n", i, (double)outputs[i], (int)r_page_servos[i]);

//			/* check mixed outputs to be zero during init phase */
//			if (hrt_elapsed_time(&starttime) < INIT_TIME_US &&
//			    r_page_servos[i] != r_page_servo_disarmed[i]) {
//				PX4_ERR("disarmed servo value mismatch: %d vs %d", r_page_servos[i], r_page_servo_disarmed[i]);
//				return false;
//			}

//			if (hrt_elapsed_time(&starttime) >= INIT_TIME_US &&
//			    r_page_servos[i] + 1 <= r_page_servo_disarmed[i]) {
//				PX4_ERR("ramp servo value mismatch");
//				return false;
//			}
//		}

//		usleep(sleep_quantum_us);
//		sleepcount++;

//		if (sleepcount % 10 == 0) {
//			fflush(stdout);
//		}
//	}

//	//PX4_INFO("ARMING TEST: NORMAL OPERATION");

//	for (int j = -jmax; j <= jmax; j++) {

//		for (unsigned i = 0; i < output_max; i++) {
//			actuator_controls[i] = j / 10.0f + 0.1f * i;
//			r_page_servo_disarmed[i] = PWM_LOWEST_MIN;
//			r_page_servo_control_min[i] = PWM_DEFAULT_MIN;
//			r_page_servo_control_max[i] = PWM_DEFAULT_MAX;
//		}

//		/* mix */
//		mixed = mixer_group.mix(&outputs[0], output_max, nullptr);

//		pwm_limit_calc(should_arm, should_prearm, mixed, reverse_pwm_mask, r_page_servo_disarmed, r_page_servo_control_min,
//			       r_page_servo_control_max, outputs, r_page_servos, &pwm_limit);

//		//fprintf(stderr, "mixed %d outputs (max %d)", mixed, output_max);

//		for (unsigned i = 0; i < mixed; i++) {
//			servo_predicted[i] = 1500 + outputs[i] * (r_page_servo_control_max[i] - r_page_servo_control_min[i]) / 2.0f;

//			if (abs(servo_predicted[i] - r_page_servos[i]) > MIXER_DIFFERENCE_THRESHOLD) {
//				fprintf(stderr, "\t %d: %8.4f predicted: %d, servo: %d\n", i, (double)outputs[i], servo_predicted[i],
//					(int)r_page_servos[i]);
//				PX4_ERR("mixer violated predicted value");
//				return false;
//			}
//		}
//	}

//	//PX4_INFO("ARMING TEST: DISARMING");

//	starttime = hrt_absolute_time();
//	sleepcount = 0;
//	should_arm = false;

//	while (hrt_elapsed_time(&starttime) < 600000) {

//		/* mix */
//		mixed = mixer_group.mix(&outputs[0], output_max, nullptr);

//		pwm_limit_calc(should_arm, should_prearm, mixed, reverse_pwm_mask, r_page_servo_disarmed, r_page_servo_control_min,
//			       r_page_servo_control_max, outputs, r_page_servos, &pwm_limit);

//		//warnx("mixed %d outputs (max %d), values:", mixed, output_max);
//		for (unsigned i = 0; i < mixed; i++) {

//			//fprintf(stderr, "disarmed:\t %d: out: %8.4f, servo: %d \n", i, (double)outputs[i], (int)r_page_servos[i]);

//			/* check mixed outputs to be zero during init phase */
//			if (r_page_servos[i] != r_page_servo_disarmed[i]) {
//				PX4_ERR("disarmed servo value mismatch");
//				return false;
//			}
//		}

//		usleep(sleep_quantum_us);
//		sleepcount++;

//		if (sleepcount % 10 == 0) {
//			//printf(".");
//			//fflush(stdout);
//		}
//	}

//	//printf("\n");

//	//PX4_INFO("ARMING TEST: REARMING: STARTING RAMP");

//	starttime = hrt_absolute_time();
//	sleepcount = 0;
//	should_arm = true;

//	while (hrt_elapsed_time(&starttime) < 600000 + RAMP_TIME_US) {

//		/* mix */
//		mixed = mixer_group.mix(&outputs[0], output_max, nullptr);

//		pwm_limit_calc(should_arm, should_prearm, mixed, reverse_pwm_mask, r_page_servo_disarmed, r_page_servo_control_min,
//			       r_page_servo_control_max, outputs, r_page_servos, &pwm_limit);

//		//warnx("mixed %d outputs (max %d), values:", mixed, output_max);
//		for (unsigned i = 0; i < mixed; i++) {
//			/* predict value */
//			servo_predicted[i] = 1500 + outputs[i] * (r_page_servo_control_max[i] - r_page_servo_control_min[i]) / 2.0f;

//			/* check ramp */

//			//fprintf(stderr, "ramp:\t %d: out: %8.4f, servo: %d \n", i, (double)outputs[i], (int)r_page_servos[i]);

//			if (hrt_elapsed_time(&starttime) < RAMP_TIME_US &&
//			    (r_page_servos[i] + 1 <= r_page_servo_disarmed[i] ||
//			     r_page_servos[i] > servo_predicted[i])) {
//				PX4_ERR("ramp servo value mismatch");
//				return false;
//			}

//			/* check post ramp phase */
//			if (hrt_elapsed_time(&starttime) > RAMP_TIME_US &&
//			    abs(servo_predicted[i] - r_page_servos[i]) > 2) {
//				printf("\t %d: %8.4f predicted: %d, servo: %d\n", i, (double)outputs[i], servo_predicted[i], (int)r_page_servos[i]);
//				PX4_ERR("mixer violated predicted value");
//				return false;
//			}
//		}

//		usleep(sleep_quantum_us);
//		sleepcount++;

//		if (sleepcount % 10 == 0) {
//			//	printf(".");
//			//	fflush(stdout);
//		}
//	}

//	return true;
//}

//static int
//mixer_callback(uintptr_t handle, uint8_t control_group, uint8_t control_index, float &control)
//{
//	if (control_group != 0) {
//		return -1;
//	}

//	if (control_index >= (sizeof(actuator_controls) / sizeof(actuator_controls[0]))) {
//		return -1;
//	}

//	control = actuator_controls[control_index];

//	if (should_prearm && control_group == actuator_controls_s::GROUP_INDEX_ATTITUDE &&
//	    control_index == actuator_controls_s::INDEX_THROTTLE) {
//		control = NAN_VALUE;
//	}

//	return 0;
//}
