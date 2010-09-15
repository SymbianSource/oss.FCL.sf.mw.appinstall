/*
* Copyright (c) 2010- Nokia Corporation and/or its subsidiary(-ies).
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

//  Include Files  
#include "swhandlerplugin.h"	// CSwHandlerPlugin

#include <ecom/ecom.h>
#include <AknServerApp.h>
#ifdef _DEBUG
#include <e32debug.h>
#endif

// ================= MACROS =======================
#ifdef _DEBUG
#define LOG(str) {RDebug::Printf(str);}
#define LOG1(str, a) {RDebug::Printf(str, a);}
#else
#define LOG(str)
#define LOG1(str, a)
#endif

//  Member Functions

CSwHandlerPlugin* CSwHandlerPlugin::NewL()
    {
    CSwHandlerPlugin* self = new (ELeave) CSwHandlerPlugin;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CSwHandlerPlugin::CSwHandlerPlugin()
// note, CBase initialises all member variables to zero
    {
    }

void CSwHandlerPlugin::ConstructL()
    {
    // second phase constructor, anything that may leave must be constructed here
    LOG("CSwHandlerPlugin::ConstructL()");
    }

CSwHandlerPlugin::~CSwHandlerPlugin()
    {
    }

/**
 * Url Handler with embedding
 * @param -
 */
void CSwHandlerPlugin::HandleUrlEmbeddedL()
    {
    LOG("CSwHandlerPlugin::HandleUrlEmbeddedL()");
    
    HandleUrlStandaloneL();
    }

/**
 * Url Handler without embedding
 * @param -
 */
void CSwHandlerPlugin::HandleUrlStandaloneL()
    {
    LOG("CSwHandlerPlugin::HandleUrlStandaloneL()");
    
    _LIT(KIAUpdateLauncherExe, "iaupdatelauncher.exe");
    _LIT(KEmptyDescriptor, "");

    RProcess rProcess;
    User::LeaveIfError(rProcess.Create(KIAUpdateLauncherExe,KEmptyDescriptor));
    LOG("CSwHandlerPlugin::HandleUrlStandaloneL() >> Created rProcess");
    
    // start the process running! Don't forget this.
    rProcess.Resume();
    LOG("CSwHandlerPlugin::HandleUrlStandaloneL() >> Resumed process");

    // free resources before returning
    rProcess.Close();
    LOG("CSwHandlerPlugin::HandleUrlStandaloneL() >> Closed rProcess (handle)");

    if(iSchemeDoc)
        {
        LOG("CSwHandlerPlugin::HandleUrlStandaloneL() >> Calling HandlerServerAppExit for observer");
        iSchemeDoc->HandleServerAppExit(KErrNone);
        }
    
    LOG("CSwHandlerPlugin::HandleUrlStandaloneL() returning");
    }

/**
 * Observer
 * @param - aSchemeDoc
 */
void CSwHandlerPlugin::Observer(MAknServerAppExitObserver* aSchemeDoc)
    {
    LOG("CSwHandlerPlugin::Observer()");
    
    iSchemeDoc = aSchemeDoc;
    }

// End of File
