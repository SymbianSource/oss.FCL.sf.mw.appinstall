/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file 
 @internalComponent 
*/
 
#ifndef	__CAPABILITIES_H__
#define	__CAPABILITIES_H__

static const char* CapabilityList[]=
	{
	"TCB" ,
	"CommDD",
	"PowerMgmt",
	"MultimediaDD",
	"ReadDeviceData",
	"WriteDeviceData",
	"DRM"	,
	"TrustedUI"	,
	"ProtServ" ,
	"DiskAdmin" ,
	"NetworkControl" ,
	"AllFiles" ,
	"SwEvent" ,
	"NetworkServices" ,
	"LocalServices" ,
	"ReadUserData" ,
	"WriteUserData" ,
	"Location" ,
	"SurroundingsDD" ,
	"UserEnvironment"
	};

#define KNumberOfCaps (sizeof(CapabilityList) / sizeof(CapabilityList[0]))

const int KCapabilityHardlimit = 256;

#endif