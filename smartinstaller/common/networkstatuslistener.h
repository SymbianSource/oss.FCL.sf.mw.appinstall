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
*     CNetworkStatusListener definition
*
*
*/

#ifndef __NETWORKSTATUSLISTENER_H__
#define __NETWORKSTATUSLISTENER_H__

#include <rconnmon.h> // RConnectionMonitor, KConnMonMaxStringAttributeLength
#include "networkstatusobserver.h"
#include "globals.h"

class CActive;

class CNetworkStatusListener : public MConnectionMonitorObserver
	{
public:
	static CNetworkStatusListener* NewL(MNetworkStatusObserver& aObserver, const TUint aIAP = KInvalidIapId);
	static CNetworkStatusListener* NewLC(MNetworkStatusObserver& aObserver, const TUint aIAP = KInvalidIapId);
	~CNetworkStatusListener();

	void SetIapIdToMonitor(const TInt aIAPId);
	TBool IsIapAvailable(const TUint aIAPId);

private:
	// MConnectionMonitorObserver
	void EventL(const CConnMonEventBase& aEvent);

	CNetworkStatusListener(MNetworkStatusObserver& aObserver, const TUint aIap = KInvalidIapId);
	void ConstructL();

	void UpdateIapArray(TConnMonIapInfo aIaps);
	TInt GetIapId(const TUint aConnId, TUint& aIapId);

private:

private:
	MNetworkStatusObserver& iObserver;
	TUint              iIapId;  // Monitored IAP
	RConnectionMonitor iConnMon;
	RArray<TUint>      iAvailableIAPs;  // IAP Array
	};

#endif
