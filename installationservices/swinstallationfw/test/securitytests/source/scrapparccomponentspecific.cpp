/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "scrapparccomponentspecific.h"

#include <usif/scr/scr.h>
#include <scs/cleanuputils.h>
#include <e32def.h>

_LIT(KScrCompSecName, "SCR-APPARC Component-specific APIs test");

using namespace Usif;

CScrApparcComponentSpecificSecTest* CScrApparcComponentSpecificSecTest::NewL()
    {
    CScrApparcComponentSpecificSecTest* self=new(ELeave) CScrApparcComponentSpecificSecTest();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CScrApparcComponentSpecificSecTest::CScrApparcComponentSpecificSecTest()
    {
    SetSidRequired(TUid::Uid(0x10003A3F)); // Use tscrapparc SID as the one required for accessinf apparc specific subsession
    }
    
void CScrApparcComponentSpecificSecTest::ConstructL()
    {
    SetNameL(KScrCompSecName);
    }

void CScrApparcComponentSpecificSecTest::RunTestL()
    {
    RSoftwareComponentRegistry scrSession;
    User::LeaveIfError(scrSession.Connect());
    CleanupClosePushL(scrSession);
    RApplicationRegistryView subSession;   
    CleanupClosePushL(subSession);
    TRAPD(err,subSession.OpenViewL(scrSession));     
    CheckFailL(err, _L("OpenApplicationRegistrationView"));
          
    if(err == NULL)
        {
        RPointerArray<CApplicationRegistrationData> actualAppRegData;
        CleanupClosePushL(actualAppRegData);
        TRAP(err,subSession.GetNextApplicationRegistrationInfoL(5, actualAppRegData));
        CheckFailL(err, _L("GetNextApplicationRegistrationInfo"));
        CleanupStack::PopAndDestroy(&actualAppRegData);
        }
    CleanupStack::PopAndDestroy(&subSession);
    
    RPointerArray<CLauncherExecutable> launchers;
    CleanupClosePushL(launchers);
    TRAP(err,scrSession.GetApplicationLaunchersL(launchers));
    CheckFailL(err, _L("GetApplicationLaunchersL"));
    CleanupStack::PopAndDestroy(&launchers);
    
    CleanupStack::PopAndDestroy(&scrSession);
    }

