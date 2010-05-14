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
*
*/


/**
 @file
 @internalComponent
 @released
*/

#include <usif/sts/sts.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "stsrecovery.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "stscommon.h"
#include <e32cmn.h>
#include <e32uid.h>

namespace Usif
	{
	void RStsRecoverySession::ConnectL()
		{
		TInt err = RScsClientBase::Connect(KStsServerName, 
				                           TVersion(KStsVerMajor, KStsVerMinor, KStsVerBuild), 
				                           KStsServerImgName, 
				                           TUidType (KExecutableImageUid, KNullUid, TUid::Uid(KUidStsServer)));
		User::LeaveIfError(err);
		}
	
	EXPORT_C RStsRecoverySession::RStsRecoverySession()
		:	RScsClientBase()
		{
		}

	EXPORT_C void RStsRecoverySession::Close()
		{
		RScsClientBase::Close();
		}

	EXPORT_C void RStsRecoverySession::RollbackAllPendingL()
		{
		ConnectL();
		User::LeaveIfError(CallSessionFunction(ERollBackAllPending));
		Close();
		}
	}
