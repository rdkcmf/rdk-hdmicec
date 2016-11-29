/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
/*****************************************************************************/
/*!
\file
\brief This file defines interface of Condition class.

*/
/*****************************************************************************/


/**
* @defgroup hdmicec
* @{
* @defgroup osal
* @{
**/


#ifndef HDMI_CCEC_OSAL_CONDITION_HPP_
#define HDMI_CCEC_OSAL_CONDITION_HPP_

#include "OSAL.hpp"

CCEC_OSAL_BEGIN_NAMESPACE

/***************************************************************************/
/*!

This is a simple class which abstract boolean functionality and is used by 
ConditionVariable class. 

*/
/**************************************************************************/

class Condition {
public:
/***************************************************************************/
/*!
\brief Constructor.
Creates an Condition object with state set to false.

*/
/**************************************************************************/

	Condition(void) : cond(false) {};
/***************************************************************************/
/*!
\brief Constructor.
Creates an Condition object with state set to given parameter.

\param initial - initial state to be set. This will be set as default state 
of the object as well.
*/
/**************************************************************************/

	Condition(bool initial) : cond(initial) {this->initial = initial;};
/***************************************************************************/
/*!
\brief Destructor.
Destroys the Condition object.
*/
/**************************************************************************/

	~Condition() {};

/***************************************************************************/
/*!
\brief Set the state of the object.
Sets the state of the object, which will be either true/false.

\param cond - state to be set.This will default to true is method called with
 no parameters.
*/
/**************************************************************************/

	virtual void set(void) {cond = true;};
	
/***************************************************************************/
/*!
\brief Check the state of the object.
Returns the state of the object, which will be either true/false.

\return true if set and false if not set.
*/
/**************************************************************************/
	virtual bool isSet(void) {return cond;};
	
/***************************************************************************/
/*!
\brief Resets the state of the object.
Reset the state of object to default, which is set while creating the object.

*/
/**************************************************************************/
	
	virtual void reset(void) {cond = initial;};
private:
	bool initial;
	bool cond;
};

CCEC_OSAL_END_NAMESPACE

#endif


/** @} */
/** @} */
