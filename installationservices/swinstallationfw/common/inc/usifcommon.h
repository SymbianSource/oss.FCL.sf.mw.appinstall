/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* This header defines common USIF entities
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef USIFCOMMON_H
#define USIFCOMMON_H

#include <e32base.h>

namespace Usif
	{
	/**
		This enumeration defines the possible states of a software component. According to the SCOMO specification
		a component may be Deactivated or Activated. New software components are activated during installation by
		default. However, the 'InstallInactive' param can be passed to the SIF in order not to activated a newly installed
		component. Please refer to the SIF API documentation for more details.
	 */
	enum TScomoState
		{
		EDeactivated = 0,
		EActivated   = 1
		};

	/**
		This enumeration defines the possible options for user dialogs. 
		If EAllowed is selected then the response to the user option dialog is treated as 'Yes'
		If ENotAllowed is selected then the response to the user option dialog is treated as 'No'
		If EUserConfirm is selected then user option dialog is presented to the user.
	*/
	enum TSifPolicy
		{
		EAllowed		= 0,
		ENotAllowed		= 1,
		EUserConfirm	= 2
		};

	/**
		TComponentId defines an instance of a software component in the Software Component Registry. Two different components cannot 
		have the same id on the system, however, it is not global and different components may have the same component id
		on separate devices.Please refer to @see GlobalId for a unique identifier for software components across different devices.
	*/
	typedef TInt TComponentId;
	
	/**
		Pre-defined software type names for the Universal Software Install Framework. 
		'native' stands for Symbian native components delivered in the SIS/SISX packages.
		'java' stands for Java MIDlets.
		'widget' stands for Widgets
	*/
	_LIT(KSoftwareTypeNative, "native");
	_LIT(KSoftwareTypeJava, "java");
	_LIT(KSoftwareTypeWidget, "widget");

	
	} // end namespace Usif

#endif
