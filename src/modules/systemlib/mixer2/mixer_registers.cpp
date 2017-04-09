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

#include "mixer_registers.h"


static const char *register_group_names_table[][MixerRegisterGroups::MIXER_REGISTER_GROUPS_MAX] = MIXER_REG_GROUP_NAMES;


/****************************************************************************/

MixerRegisterGroup::MixerRegisterGroup()
	: _groupSize(0)
	, _groupData(nullptr)
	, _readOnly(true)
#if !defined(MIXER_REMOTE)
	, _groupName("UNDEFINED_GROUP")
#endif //MIXER_REMOTE
{

}

MixerRegisterGroup::setGroup(unsigned groupSize, mixer_register_u *pgroupData, bool readOnly = false)
{
: _groupSize(groupSize)
	, _groupData(pgroupData)
	, _readOnly(readOnly)
}

MixerRegisterGroup::~MixerRegisterGroup()
{
}


/****************************************************************************/

MixerRegisterGroups::MixerRegisterGroups()
	: _registerGroups()
{
}


bool MixerRegisterGroups::validRegister(mixer_register_ref_s *regref)
{
	if (regref->group-> MIXER_REGISTER_GROUPS_MAX) {
		return false;
	}

	if (regref->index >= _registerGroups[regref->group]->groupSize()) {
		return false;
	}

	return true;
}

const char *MixerRegisterGroups::getGroupName(int index)
{
	if (index >= MixerRegisterGroups::MIXER_REGISTER_GROUPS_MAX) {
		return nullptr;
	}

	return register_group_names_table[index];
}

//#if defined(MIXER_TUNING)
//#if !defined(MIXER_REMOTE)
//#endif //MIXER_REMOTE


#endif //defined(MIXER_TUNING)
