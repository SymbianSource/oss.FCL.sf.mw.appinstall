/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     This file contains panic codes.
*
*
*/



#ifndef __ADM_PAN__
#define __ADM_PAN__

/** ADM application panic codes */
enum TADMPanics
	{
	EPanicAdmUi = 1,
	EPanicAdmCmdLineArgs,
	EPanicAdmCmdInvalidProtocol,
	EPanicAdmCmdInvalidBootstrap,
	EPanicAdmCmdInvalidWrapperUid,
	EPanicAdmCmdInvalidIAP,
	EPanicAdmCmdInvalidLaunchState,
	EPanicAdmCmdInvalidLaunchState2,
	EPanicAdmFileSystemAccess
	};

inline void Panic(TADMPanics aReason)
	{
	_LIT(applicationName, "SmartInstADM");
	User::Panic(applicationName, aReason);
	}

#endif // __ADM_PAN__
