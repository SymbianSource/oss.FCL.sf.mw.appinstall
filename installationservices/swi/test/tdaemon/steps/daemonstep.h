/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef __DAEMONSTEP_H__
#define __DAEMONSTEP_H__

#include "watcherstep.h"
#include "daemon.h"

#include "tdaemonstep.h"

namespace Swi
{
class CAppInfo;

namespace Test
{
_LIT(KDaemonStep,"DaemonStep");
_LIT(KAddDriveStep,"AddDriveStep");

class CDaemonStep : public CDaemonTestStep, public MDaemonBehaviour
	{
public:
	CDaemonStep();
	~CDaemonStep();

	virtual TVerdict runTestStepL(TBool aOomTest);
  	
	virtual TBool StartupL();
	virtual void MediaChangeL(TInt aDrive, TChangeType aChangeType);

protected:
	void FreeMemory();

	CDaemon* iDaemon;
	};

class CAddDriveStep : public CDaemonTestStep
    {
public:
    CAddDriveStep();
    ~CAddDriveStep();

    virtual TVerdict runTestStepL(TBool aOomTest);
    
    };

} // namespace Swi::Test

} //namespace Swi

#endif // #ifndef __TIMEOUTSTEP_H__

