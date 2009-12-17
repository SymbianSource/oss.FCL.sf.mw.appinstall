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
* Description:   This module contains the implementation of CIAUpdateLauncherAppUi class 
*                member functions.
*
*/



// INCLUDE FILES
#include "iaupdatelauncherappui.h"
#include "iaupdatelaunchermanager.h"
#include "iaupdatedebug.h"

_LIT8( KRefreshFromNetworkDenied, "1" );


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CIAUpdateLauncherAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateLauncherAppUi::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherAppUi::ConstructL() begin");
    // Initialise app UI with standard value.
    BaseConstructL( ENoAppResourceFile );
    StatusPane()->MakeVisible( EFalse );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherAppUi::ConstructL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherrAppUi::CIAUpdateLauncherAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateLauncherAppUi::CIAUpdateLauncherAppUi()
    {
    SetFullScreenApp( EFalse );
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherAppUi::~CIAUpdateLauncherAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CIAUpdateLauncherAppUi::~CIAUpdateLauncherAppUi()
    {
    delete iLauncherManager;
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherAppUi::HandleCommandL
// 
// -----------------------------------------------------------------------------
//         
void CIAUpdateLauncherAppUi::HandleCommandL( TInt aCommand )
    {
    switch(aCommand)
        {
        case EEikCmdExit:
        case EAknCmdExit:
        case EAknSoftkeyExit:
            {
            delete iLauncherManager;
            iLauncherManager = NULL;
            Exit();	
            break;
            }
             
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherAppUi::ProcessCommandParametersL
// 
// -----------------------------------------------------------------------------
// 
TBool CIAUpdateLauncherAppUi::ProcessCommandParametersL( TApaCommand /*aCommand*/,
                                                         TFileName& /*aDocumentName*/,
                                                         const TDesC8& aTail )
   {
   IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherAppUi::ProcessCommandParametersL() begin");
   if ( aTail == KRefreshFromNetworkDenied )
       {
   	   iRefreshFromNetworkDenied = ETrue;
       }
   iLauncherManager = CIAUpdateLauncherManager::NewL( iRefreshFromNetworkDenied );

   IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherAppUi::ProcessCommandParametersL() end");
   return ETrue;	
   }
    
// End of File

