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


#ifndef _SYSTEMLIB_MIXER_MIXER_H
#define _SYSTEMLIB_MIXER_MIXER_H value

#include <px4_config.h>

#include "mixer_registers.h"

#include "mixer_types.h"

/****************************************************************************/

/**
 * Abstract class defining the basic mixer oeprations
 */
class __EXPORT Mixer
{
public:
	/** next mixer in a list */
	Mixer				*_next;

	/**
	 * Constructor.
	 *
	 * @param control_cb		Callback invoked when reading controls.
	 */
	Mixer(mixer_base_header_s *mixdata);
	virtual ~Mixer() {;}


	/**
	 * Perform the mixing function.
	 *
	 * @return			Saturation status
	 */
	virtual uint16_t		mix(MixerRegisterGroups *reg_groups, mixer_register_types_e type = MIXER_REGISTER_TYPE_NONE) = 0;

	/**
	 * Get the base type of the mixer to perform appropriate operations on data structures
	 *
	 * @return			Mixer base type
	 */
	virtual uint16_t    getBaseType()  {return MIXER_BASE_TYPE_NONE;}

	/**
	 * Get the mixer type from the mixer data
	 *
	 * @return			Mixer type
	 */
	uint16_t getMixerType() {if (_mixdata != nullptr) return _mixdata->mixer_type; else return MIXER_TYPES_NONE;}

	/**
	 * Get the mixer data size
	 *
	 * @return			Mixer data size
	 */
	uint16_t getDataSize() {if (_mixdata != nullptr) return _mixdata->data_size; else return 0;}

	/**
	 * Get the mixer data
	 *
	 * @return			Mixer data reference
	 */
	mixer_base_header_s *getMixerData() {return _mixdata;}

	typedef enum {
		MIXER_BASE_TYPE_NONE            = 0,
		MIXER_BASE_TYPE_OPERATOR        = 1,
		MIXER_BASE_TYPE_CONST_OPERATOR  = 2,
		MIXER_BASE_TYPE_FUNCTION        = 3,
		MIXER_BASE_TYPE_OBJECT          = 4,
	} MIXER_BASE_TYPE;

protected:
	/**
	 * Pointer to data structure for the mixer
	 */
	mixer_base_header_s             *_mixdata;

private:
	/* do not allow to copy due to pointer data members */
	Mixer(const Mixer &);
	Mixer &operator=(const Mixer &);
};


/****************************************************************************/

/**
 * Abstract class defining mixers that are operators
 */
class __EXPORT MixerOperator : public Mixer
{
public:
	MixerOperator(mixer_data_operator_s *mixdata);
	~MixerOperator();

	uint16_t getBaseType()  {return MIXER_BASE_TYPE_OPERATOR;}
protected:
private:
};

/****************************************************************************/

/**
 * Abstract class defining mixers that are operators
 */
class __EXPORT MixerConstOperator : public Mixer
{
public:
	MixerConstOperator(mixer_data_const_operator_s *mixdata);
	~MixerConstOperator();

	uint16_t getBaseType()  {return MIXER_BASE_TYPE_CONST_OPERATOR;}
protected:
private:
};


/****************************************************************************/

/**
 * Abstract class defining the basic mixer oeprations
 */
class __EXPORT MixerFunction : public Mixer
{
public:
	MixerFunction(mixer_data_function_s *mixdata);
	~MixerFunction();

	uint16_t getBaseType()  {return MIXER_BASE_TYPE_FUNCTION;}
protected:
private:
};



/****************************************************************************/

/**
 * Class for Mixer Objects, mixers with internal state, initialization or owning its own parameters
 *  Example is a complete mixer for a multirotor initialized with the multirotor geometry
 */
class __EXPORT MixerObject : public Mixer
{
public:
	MixerObject(mixer_data_object_s *mixdata);
	virtual ~MixerObject() {;}

	uint16_t getBaseType()  {return MIXER_BASE_TYPE_OBJECT;}

#if !defined(MIXER_REMOTE)
	/**
	* gets a mixer parameter and metadata
	 *
	* @param param         Contains parameter address.  Data returned in structure.
	* @return              Count of parameters read.  -1 if error
	 */
	virtual int16_t        get_parameter(mixer_param_s *param) {return -1;}

	/**
	 * sets mixer parameter array values
	 *
	* @param param         Message including all information required to set values
	* @return              Count of parameters written.  <0 if error
	 */
	virtual int16_t         set_parameter(mixer_param_s *param) {return -1;}
#endif  //MIXER_REMOTE

	/**
	* Get a count of parameters for this mixer
	 *
	* @return              A count of parameters for the mixer
	 */
	virtual int16_t         parameter_count() {return 0;}

	/**
	 * sets an array value in a mixer parameter
	 * Separated from set_parameter to support remote mixers with limited communication
	 *
	* @param paramIndex    The mixer index for the parameter
	* @param arrayIndex    Index of the array value to set
	* @param value         The value to set
	* @return              negative on error
	 */
	virtual int16_t        set_param_value(int16_t paramIndex, int16_t arrayIndex, float value) {return -1;}
};


/****************************************************************************/

/**
 * Group of mixers, built up from single mixers and processed
 * in order when mixing.
 */
class __EXPORT MixerGroup
{
public:
	MixerGroup(MixerRegisterGroups *reg_groups);
	~MixerGroup();

	uint16_t get_saturation_status(void);

	/**
	 * Check that the mixers are using valid data from the registers
	 *
	 * @return		true if mixers ok.
	 */
	bool check_mixers_valid();

	/**
	 * Perform mixer for the group mixer collection
	 *
	 * @return              Saturation status
	 */
	uint16_t mix_group();

	/**
	 * Remove all the mixers from the group.
	 */
	void reset();

	/**
	 * Count the mixers in the group.
	 */
	unsigned count();

	/**
	 * Adds a mixer to the group
	 *
	 * @param mixer     mixer to add to the list
	 * @return          Zero on successful load, nonzero otherwise.
	 */
	int append_mixer(Mixer *mixer);


	/**
	 * Create new mixer(s) from a data buffer and adds them into a MixerGroup.
	 * Does not take ownership of the buffer.  Copies buffer to new mixer data structure
	 * ownder by mixer created by factory.
	 * Supports streamed buffer with partial mixer data by returning the reminaing
	 * unused bytes in the buffer
	 *
	 * @param[in]   group       MixerGroup object to put parsed mixers in.
	 * @param[in]   mixbuff     mixer data buffer describing all mixers
	 * @param[in]   bufflen     buffer length.
	 * @return                  Remaining buffer length not used.
	 */
	int from_buffer(uint8_t *mixbuff, int bufflen);

	//	void setMixRegsGroup(MixerRegisterGroups *reg_groups) {_reg_groups = reg_groups;}

#if !defined(MIXER_REMOTE)

	/**
	 * Copy all mixer data in sequence to a referenced buffer
	 *
	 * @param       mixbuff     buffer to put data
	 * @param[in]   bufflen     Buffer length available.
	 * @return                  Buffer length used. -1 if buffer overflowed.
	 */
	int to_buffer(uint8_t *mixbuff, int bufflen);

	/**
	* @brief                   Get the value of a mixer parameter
	 *
	* @param[in] index         Index to get the parameter from
	* @param[out] values       Array of parameter values
	* @return                  0 if ok, <0 for failure
	 */
	int16_t group_get_param(mixer_param_s *param);

	/**
	* @brief                       Get the count of parameters in the group
	* @return                      Return count, -1 on failure.
	 */
	int16_t group_param_count();

	/**
	* @brief                       Set the value of a mixer parameter
	 *
	* @param[in]   index           index of param to set
	* @param[in]   values          values to set parameter to
	* @return                      Zero on success, -1 on failure.
	 */
	int16_t group_set_param(mixer_param_s *param);

#endif  //MIXER_REMOTE

	/**
	 * sets an array value in a mixer parameter
	 *
	* @param index         The group parameter index
	* @param arrayIndex    Index of the array value to set
	* @param value         The value to set
	* @return              negative on error
	 */
	int16_t        group_set_param_value(int16_t index, int16_t arrayIndex, float value);

private:
	Mixer				*_first;	/**< linked list of mixers */

	MixerRegisterGroups             *_reg_groups;
	/* do not allow to copy due to pointer data members */
	MixerGroup(const MixerGroup &);
	MixerGroup operator=(const MixerGroup &);
};


#endif
