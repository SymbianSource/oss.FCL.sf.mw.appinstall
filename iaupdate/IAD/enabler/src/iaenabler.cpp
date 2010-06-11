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
* Description:   Source code for enabling FF_IAUPDATE feature flag
*
*/



#include <featurecontrol.h>


// MACROS
#ifdef  _DEBUG
#define TRACE( aText ) { RDebug::Print( _L(aText) ); }
#define TRACE_P( aText, aParam ) {RDebug::Print( _L(aText), aParam );}
#else
#define TRACE( aText )
#define TRACE_P( aText, aParam )
#endif  //_DEBUG


// ---------------------------------------------------------------------------
// Enables feature FF_IAUPDATE
// ---------------------------------------------------------------------------
//
void EnableIAUpdateL( )
    {
    RFeatureControl featureControl;
    TInt error( KErrNone );

    error = featureControl.Connect();
    TRACE_P( "[IAEnabler] Connection to the Feature Manager Server: %d.", error );

    User::LeaveIfError( error );

    // Enable FF_IAUPDATE.
    TUid iaupdatefeature;
    iaupdatefeature.iUid = KFeatureIdIAUpdate;
    error = featureControl.EnableFeature( iaupdatefeature );
    TRACE_P( "[IAEnabler] Enable feature status: %d.", error );

    featureControl.Close();
    }


// ---------------------------------------------------------------------------
// Main function
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt error( KErrNone );
    TRAP( error, EnableIAUpdateL() );
    delete cleanup;
    return error;
    }
