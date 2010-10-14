/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




// INCLUDE FILES
#include <centralrepository.h>
#include <DevManInternalCRKeys.h>

#include "iaupdatefwfotamodel.h"
#include "iaupdatefwdebug.h"
#include "iaupdatefwconst.h"
#include "iaupdatefwnsmlcrkeys.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
 CIAUpdateFWFotaModel* CIAUpdateFWFotaModel::NewL( /* KNST CNSCDocument* aDocument*/ )
    {
    CIAUpdateFWFotaModel* self = new( ELeave ) CIAUpdateFWFotaModel( /*KNST aDocument*/ );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CIAUpdateFWFotaModel::~CIAUpdateFWFotaModel()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::~CIAUpdateFWFotaModel()" );
    iFotaEngine.Close();
    }


// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::DefaultFotaProfileIdL
// -----------------------------------------------------------------------------
//
 TInt CIAUpdateFWFotaModel::DefaultFotaProfileIdL() const
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::DefaultFotaProfileIdL()" );
    
    TInt profileId( KErrNotFound );
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );
    FTRACE( FPrint( _L(
        "[IAUPDATEFW] CIAUpdateFWFotaModel::DefaultFotaProfileIdL(), opening cenrep returned %d" ),
        err ) );

    User::LeaveIfError( err );
    centrep->Get( KNSmlDMDefaultFotaProfileKey, profileId );
    delete centrep;
    
    FTRACE( FPrint( _L(
        "[IAUPDATEFW] CIAUpdateFWFotaModel::DefaultFotaProfileIdL() completed, profileId = %d" ),
        profileId ) );
    return profileId;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::SetDefaultFotaProfileIdL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWFotaModel::SetDefaultFotaProfileIdL( const TInt aProfileId )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::SetDefaultFotaProfileIdL()" );
    
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );
    FTRACE( FPrint( _L(
        "[IAUPDATEFW] CIAUpdateFWFotaModel::SetDefaultFotaProfileIdL(), opening cenrep returned %d" ),
        err ) );

    User::LeaveIfError( err );
    centrep->Set( KNSmlDMDefaultFotaProfileKey, aProfileId );
    delete centrep;
    
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::SetDefaultFotaProfileIdL() completed" );
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::EnableFwUpdRequestL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWFotaModel::EnableFwUpdRequestL( const TInt aProfileId )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::EnableFwUpdRequestL()" );
    
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );
    FTRACE( FPrint( _L(
        "[IAUPDATEFW] CIAUpdateFWFotaModel::EnableFwUpdRequestL(), opening cenrep returned %d" ),
        err ) );
    User::LeaveIfError( err );
    centrep->Set( KDevManClientInitiatedFwUpdateId, aProfileId );
    delete centrep;
    
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::CIAUpdateFWFotaModel::EnableFwUpdRequestL( completed" );
    }
 

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::MarkFwUpdChangesStartL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWFotaModel::MarkFwUpdChangesStartL()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::MarkFwUpdChangesStartL()" );    
    }



// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::CIAUpdateFWFotaModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateFWFotaModel::CIAUpdateFWFotaModel() 
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateFWFotaModel::ConstructL()
    {
    iFotaEngine.OpenL();
    }


// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
TBool CIAUpdateFWFotaModel::IsDMSupportAvailableL()
    {
    TInt packId = -1 ;
    TInt err = iFotaEngine.DeleteUpdatePackage( packId );
    if ( err == KErrAccessDenied )
        {
        return EFalse;
        }
    return ETrue;   
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::GetCurrentFwUpdState
// Fetches the state of last or current Fota operation
// -----------------------------------------------------------------------------
//
RFotaEngineSession::TState CIAUpdateFWFotaModel::GetCurrentFwUpdState()
	  {
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::GetCurrentFwUpdState()" );	
	  // Workaround because fotaserver has closed all sessions if DM UI was closed.
	  // So, we cannot rely on existing session but a new one needs to be opened.
	  iFotaEngine.Close();
	  iFotaEngine.OpenL();
	  const TInt x = -1;
	  return iFotaEngine.GetState(x);
	  }


TInt CIAUpdateFWFotaModel::TryResumeFwUpdDownload()
    {	
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::TryResumeFwUpdDownload()" );	    
    TInt retval = iFotaEngine.TryResumeDownload();
    return retval;
    }
	
//  End of File  
