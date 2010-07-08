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

#include "daemonstep.h"
#include "sisregistrywritablesession.h"

namespace Swi
{

namespace Test
{


CDaemonStep::CDaemonStep()
	{
	}

CDaemonStep::~CDaemonStep()
	{
	}

void CDaemonStep::FreeMemory()
	{
	delete iDaemon;
	}

void CDaemonStep::MediaChangeL(TInt /*aDrive*/, TChangeType /*aChangeType*/)
	{
	
	}
	
TBool CDaemonStep::StartupL()
	{
	return ETrue;
	}

TVerdict CDaemonStep::runTestStepL(TBool /*aOomTest*/)
	{
	iDaemon=CDaemon::NewL(*this);

	// Check all the correct wtachers were started up
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TDriveList driveList;
	
	User::LeaveIfError(fs.DriveList(driveList));

	TInt count=0;
	
	for (TInt drive=0; drive < KMaxDrives; ++drive)
		{
		if (driveList[drive] == 0)
			{
			continue;
			}
		TDriveInfo info;
		
		// Ignore errors since the next drive might work
		if (KErrNone != fs.Drive(info, drive))
			{
			/// @todo, log error
			continue;
			}

		if (info.iDriveAtt & KDriveAttRemovable)
			{
			if (drive != iDaemon->Watchers()[count++]->Drive())
				{
				ERR_PRINTF1(_L("Error, watched drives do not match"));
				SetTestStepResult(EFail);
				User::Leave(KErrNotFound);
				}
			}
		}
	
	// Different number started up
	if (count != iDaemon->Watchers().Count())
		{
		ERR_PRINTF1(_L("Error, watched drive count does not match"));
		SetTestStepResult(EFail);
		User::Leave(KErrGeneral);
		}


	// Different number started up
	if (count == 0)
		{
		ERR_PRINTF1(_L("Error, there must be at least one removable drive present"));
		SetTestStepResult(EFail);
		User::Leave(KErrGeneral);
		}
		
	
	CleanupStack::PopAndDestroy(&fs);
	return EPass;
	}

//CAddDriveStep
CAddDriveStep::CAddDriveStep()
    {
    }

CAddDriveStep::~CAddDriveStep()
    {
    }

TVerdict CAddDriveStep::runTestStepL(TBool /*aOomTest*/)
    {
    TPtrC str;
    TInt drive(0);
    if (!GetStringFromConfig(ConfigSection(), _L("drive"), str))
        {
        ERR_PRINTF1(_L("Missing drive setting"));
        SetTestStepResult(EFail);
        }
    else
        {
        RFs fs;
        fs.Connect();
        
        User::LeaveIfError(fs.CharToDrive(str[0], drive));
        //iDriveChar = str[0];
        fs.Close();
        }
    RSisRegistryWritableSession registrySession;
            
    User::LeaveIfError(registrySession.Connect());
    CleanupClosePushL(registrySession);
    registrySession.AddDriveL(drive);

    CleanupStack::PopAndDestroy(&registrySession);    

    return EPass;
    }
} // namespace Swi::Test

} //namespace Swi

