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
* Interface for the CSwiInstallMonitor class
*
*/


/**
 @file 
 @internalComponent 
*/

#ifndef __SWIINSTALLMONITOR_H__
#define __SWIINSTALLMONITOR_H__

#include <e32base.h>
#include <e32property.h>
#include <swi/swispubsubdefs.h>

NONSHARABLE_CLASS(CSwiInstallMonitor) : public CActive
/** 
CSwiInstallMonitor

A low priority (EPriorityLow) active object which monitors software
install for installations/uninstallation of applications.

@internalComponent
*/
	{
public:
	static CSwiInstallMonitor* NewL(TCallBack aCallBack);
	~CSwiInstallMonitor();
	void Start();
private:
	CSwiInstallMonitor(TCallBack aCallBack);
	void ConstructL();
	void RunL();
	void DoStart();
	void DoCancel();

	static inline Swi::TSwisOperation SwisOperation(TInt& aState)
		{
		return STATIC_CAST(Swi::TSwisOperation,aState & Swi::KSwisOperationMask);
		}
	static inline Swi::TSwisOperationStatus SwisStatus(TInt& aState)
		{
		return STATIC_CAST(Swi::TSwisOperationStatus,aState & Swi::KSwisOperationStatusMask);
		}
private:
	TCallBack iCallBack;
	RProperty iSwisProperty;
	TInt iSwisState;
	};
	
#endif
