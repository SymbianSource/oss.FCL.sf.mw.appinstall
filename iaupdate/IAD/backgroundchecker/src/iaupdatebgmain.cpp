/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of background checker
*
*/



// INCLUDE FILES 
#include <featurecontrol.h>
#include "iaupdatebgrefreshtimer.h"
#include "iaupdatebgcontrollerfile.h"
#include "iaupdatebgsoftnotification.h"
#include "iaupdatebglogger.h"
// LOCAL FUNCTION PROTOTYPES
LOCAL_C TInt ThreadStartL();
LOCAL_C TBool IAUpdateEnabledL();

//Const
_LIT( KBackgroundChecker, "iaupdatebgchecker" );

// ---------------------------------------------------------------------------
// E32Main()
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main() 
    {    
    __UHEAP_MARK;
    FLOG("[bgchecker] E32Main() begin");
    if ( User::TrapHandler() != NULL )
        {
        User::SetTrapHandler( NULL );
        }
        
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if ( !cleanup )
        {
        return KErrNone;
        }

    TRAPD( err, ThreadStartL() );
    ( void ) err;
    
    delete cleanup;
    FLOG("[bgchecker] E32Main() end");
    __UHEAP_MARKEND;

    return err;    
    }


// ---------------------------------------------------------------------------
// ThreadStartL()
// ---------------------------------------------------------------------------
//
LOCAL_C TInt ThreadStartL()
    {
    if ( !IAUpdateEnabledL() )
        {
        FLOG("[bgchecker] ThreadStartL() IAUpdate not enabled");
        return KErrNone;  
        }
    TFullName name;
    TFindProcess  iaupdatebgFinder(_L("*iaupdatebgchecker*"));

    if (iaupdatebgFinder.Next(name) == KErrNone )
        {
        FLOG("[bgchecker] Process already exists");
        User::LeaveIfError( KErrAlreadyExists );
        }

    RProcess::Rendezvous( KErrNone );
    
    User::LeaveIfError( User::RenameProcess( KBackgroundChecker ) );
    
    CActiveScheduler *threadScheduler = new CActiveScheduler;
    
    CleanupStack::PushL( threadScheduler );
    
    CActiveScheduler::Install( threadScheduler );

    FLOG("[bgchecker] launch background checker");

    CIAUpdateBGTimer* timer = CIAUpdateBGTimer::NewL();

    CleanupStack::PushL( timer );
    
    timer->StartProcessL();

    FLOG("[bgchecker] background checker is started");
            
    CActiveScheduler::Start();
    
    CleanupStack::PopAndDestroy( 2 );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// IAUpdateEnabledL() 
// ---------------------------------------------------------------------------
//
LOCAL_C TBool IAUpdateEnabledL() 
    {
    FLOG("[bgchecker] IAUpdateEnabledL() begin");
    TBool enabled = EFalse;
    RFeatureControl featureControl;
    TInt error( KErrNone );

    error = featureControl.Connect();
    FLOG_NUM("[bgchecker] IAUpdateEnabledL() Connection to the Feature Manager Server: %d", error );
    
    User::LeaveIfError( error );
    
    TUid iaupdatefeature;
    iaupdatefeature.iUid = KFeatureIdIAUpdate;
    
    TInt ret = featureControl.FeatureSupported( iaupdatefeature );
    if ( ret == KFeatureSupported )
        {
        enabled = ETrue;
        }
    featureControl.Close(); 
    FLOG_NUM("[bgchecker] IAUpdateEnabledL() enabled: %d", enabled );
    return enabled;        
    }

//EOF  

