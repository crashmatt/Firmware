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
 * @file mixer_registers.cpp
 *
 * Programmable multi-channel mixer library.
 */

#include "mixer_registers.h"
#include "string.h"

static const char *register_group_names_table[MixerRegisterGroups::MIXER_REGISTER_GROUPS_MAX] = MIXER_REG_GROUP_NAMES;


/****************************************************************************/

MixerRegisterGroup::MixerRegisterGroup()
	: group_required(false)
	, _group_size(0)
	, _group_data(nullptr)
	, _read_only(true)
{

}

void
MixerRegisterGroup::setGroup(unsigned group_size, mixer_register_val_u *group_data, bool read_only)
{
	_group_size = group_size;
	_group_data = group_data;
	_read_only = read_only;
}


bool
MixerRegisterGroup::validRegister(uint16_t reg_index, bool read_only)
{
	if (_group_data == nullptr) {
		return false;
	}

	if (reg_index >= _group_size) {
		return false;
	}

	if (!read_only && _read_only) {
		return false;
	}

	return true;
}


/****************************************************************************/

MixerRegisterGroups::MixerRegisterGroups()
	: register_groups()
{
	memset(&register_groups, 0, sizeof(register_groups));
}

bool MixerRegisterGroups::validRegister(mixer_register_ref_s *regref, bool read_only)
{
	if (regref->group >= MIXER_REGISTER_GROUPS_MAX) {
		return false;
	}

	return register_groups[regref->group].validRegister(regref->index, read_only);
}

const char *MixerRegisterGroups::getGroupName(int index)
{
	if (index >= MixerRegisterGroups::MIXER_REGISTER_GROUPS_MAX) {
		return nullptr;
	}

	return register_group_names_table[index];
}
