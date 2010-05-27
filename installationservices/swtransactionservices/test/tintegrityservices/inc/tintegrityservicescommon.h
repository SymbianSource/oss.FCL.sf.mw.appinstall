/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* tintegrityservicesserver.h
*
*/


#ifndef __TINTEGRITYSERVICESCOMMON_H__
#define __TINTEGRITYSERVICESCOMMON_H__

#include <e32def.h>
#include <e32cmn.h>

const TInt KIntegrityServicesSimulatedBatteryFailure=-10205;	


_LIT(KIntegrityServicesServerName, "!tstsintegrityservicesserver");
_LIT(KIntegrityServicesServerImage, "tstsintegrityservicesserver");
_LIT(KIntegrityServicesServer, "Test Integrity Server");

const TUid KIntegrityServicesServerUid3 = { 0x10285DE7 };

/**
 * @test
 * @internalTechnology
 */
enum TIntegrityServicesServerMessage
	{ 
	EAdd,
	ERemove,
	ETemporary,
	ECommit,
	ERollBack,
	ESetSimulatedFailure,
	ECreateNewTestFile,
	ECreateTempTestFile
	};

#endif // #ifndef __TINTEGRITYSERVICESSERVER_H__
