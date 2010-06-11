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
* Interface for the CSwiSidChecker class
*
*/


/**
 @file 
 @internalComponent 
*/
 
#ifndef __SWISIDCHECKER_H__
#define __SWISIDCHECKER_H__

#include <apsidchecker.h>
#include <swi/sisregistrysession.h>

class CSwiInstallMonitor;

NONSHARABLE_CLASS(CSwiSidChecker) : public CAppSidChecker
/** Swi Sid checker.

This is an implementation of CAppSidChecker for checking nativa application Sids

This instance of the CAppSidChecker is used to verify that a native application
installed and can be launched.

@internalComponent */
	{
public:
	static CSwiSidChecker* NewL();
	~CSwiSidChecker();

public:
	virtual TBool AppRegisteredAt(const TUid& aSid, TDriveUnit aDrive);
	virtual void SetRescanCallBackL(const TCallBack &aCallback);

private:
	CSwiSidChecker();
	TBool AppRegisteredAtInternalL(const TUid& aSid, TDriveUnit aDrive);

private:
	CSwiInstallMonitor* iSwiInstallMonitor;	
	};

#endif

