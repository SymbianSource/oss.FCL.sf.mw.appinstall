// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// apaid.h - Application capabilities.
//
/** 
* @file apaid.h
*
* @internalComponent
* @released
*/

#ifndef __APAID_H__
#define __APAID_H__
#include<iostream>
#include<vector>
#include "commontypes.h"
using namespace std;
#include "apmstd.h"		// class TDataTypeWithPriority
#include "uidtype.h"

class TAppCapability
{
public:
	/** Defines an application's support for embeddability. */
	enum TEmbeddability {
		/** An application cannot be embedded. */
		ENotEmbeddable=0,
		/** An application can be run embedded or standalone and can read/write embedded document-content. */
		EEmbeddable=1,
		/** An application can only be run embedded and can read/write embedded document-content. */
		EEmbeddableOnly=2,
		/** An application can be run embedded or standalone and cannot read/write embedded document-content. */
		EEmbeddableUiOrStandAlone=5,
		/** An application can only be run embedded and cannot read/write embedded document-content. */
		EEmbeddableUiNotStandAlone=6 };
	/** Defines an application's attributes as a set of bit flags. */
	enum TCapabilityAttribute
		{
		/** This functionality is deprecated from v9.5 onwards.
		@deprecated
		*/
		EBuiltAsDll			= 0x00000001,
		/** If set, the application provides control panel functionality. */
		EControlPanelItem	= 0x00000002,
		/** If set, the application is not a native executable, and hence the "path" and "extension" (as well as the "name") must be provided in the app_file field of the APP_REGISTRATION_INFO resource. */
		ENonNative			= 0x00000004
		};
public:
	/** Indicates the extent to which the application can be embedded. */
	TEmbeddability iEmbeddability;
	/** Indicates whether the application is document-based and supports being asked 
	to create a new file. */
	TUint iSupportsNewFile;
	/** Indicates whether the existence of the application should be advertised to the 
	user. If this is set to ETrue, the application does not appear on the Extras 
	Bar (or equivalent). */
	TUint iAppIsHidden;  // not shown in the Shell etc.
	/** Allows the application to be launched in the foreground (when set to EFalse) 
	or in the background (when set to ETrue). */
	TUint iLaunchInBackground; // Series 60 extension to allow apps to be launched in the background
	/** Stores the application's logical group name. */
	TAppGroupName iGroupName; // Series 60 extension to allow apps to be categorized according a logical group name, e.g. 'games'

	/** Indicates the application attributes. One or more values from TCapabilityAttribute may be specified. */
	TUint iAttributes;
private:
	// expandable class - add new members to the end, add them to the end of int/ext also, and increment the version no.
	// default value for all data members must be 0
	enum { EVersion=4 };
private:
	TInt iTAppCapability_Reserved1;
	};


/** Application service information.

Encapsulates a service UID and associated opaque data.

An instance of this class provides information about
a specific implementation of the service identified by
the encapsulated service UID.

@publishedPartner
@released
*/

class TAppServiceInfo
	{
public:
	 TAppServiceInfo();
	 ~TAppServiceInfo();
	 TAppServiceInfo(TUid aUid, std::vector<TDataTypeWithPriority*> aDataTypes);
	 void Release();
	 TUid GetUid();
	 std::vector<TDataTypeWithPriority*> GetDataType();
	 
private:
	TUid iUid;
	std::vector<TDataTypeWithPriority*> iDataTypes;
	TInt iTAppServiceInfo;
	};

#endif