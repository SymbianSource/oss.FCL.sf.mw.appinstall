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
* Description:            
*
*/




// INCLUDE FILES
#include <sysutil.h>
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
    //iFwUpdIdStateList.Close();
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
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::SetDefaultFotaProfileIdL()" );
    
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );
    FTRACE( FPrint( _L(
        "[IAUPDATEFW] CIAUpdateFWFotaModel::EnableFwUpdRequestL(), opening cenrep returned %d" ),
        err ) );
    User::LeaveIfError( err );
    centrep->Set( KDevManClientInitiatedFwUpdateId, aProfileId );
    delete centrep;
    
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::SetDefaultFotaProfileIdL() completed" );
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::ReadProfileListL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWFotaModel::ReadProfileListL( CDesCArray*          /*aItems*/,
                                         CArrayFixFlat<TInt>* /*aProfileIdList*/ )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::ReadProfileListL() - not implemented" );
    //no implementation
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::SelectDefaultProfileFromList
// -----------------------------------------------------------------------------
//
TInt CIAUpdateFWFotaModel::SelectDefaultProfileFromList(
    const CArrayFixFlat<TInt>* /*aProfileIdList*/ ) const
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::SelectDefaultProfileFromList() - not implemented" );
    TInt retval( KErrNotFound );
    return retval;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::VerifyProfileL
// -----------------------------------------------------------------------------
//
TBool CIAUpdateFWFotaModel::VerifyProfileL( const TInt /*aProfileId*/ ) const
    {
    TBool retval( EFalse );
    return retval;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::RetrieveFwUpdPkgIdListL
// -----------------------------------------------------------------------------
//
RArray< TInt > CIAUpdateFWFotaModel::RetrieveFwUpdPkgIdListL()
    {
    const TInt maxNumOfProfiles = 32;
    
    TBuf16< maxNumOfProfiles > idListDescriptor;
    User::LeaveIfError( iFotaEngine.GetUpdatePackageIds( idListDescriptor ) );
    
    RArray< TInt > idList;
    
    TInt count = idListDescriptor.Length();
    for ( TInt i = 0; i < count; i++ )
        {
        idList.Append( idListDescriptor[ i ] );
        }
    
    return idList;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::FindFwUpdPkgWithStateL
// -----------------------------------------------------------------------------
//
 TInt CIAUpdateFWFotaModel::FindFwUpdPkgWithStateL(
    RFotaEngineSession::TState aState )
    {
    FTRACE( FPrint( _L(
        "[IAUPDATEFW] CIAUpdateFWFotaModel::FindFwUpdPkgWithStateL(): aState = %d" ),
         (TInt) aState ) );

    RArray< TInt > idList = RetrieveFwUpdPkgIdListL();
    TInt retval = KErrNotFound;

    TInt count = idList.Count();
    for ( TInt i = 0; (i < count) && (retval == KErrNotFound); i++ )
        {
        RFotaEngineSession::TState pkgState;
        pkgState = iFotaEngine.GetState( idList[ i ] );
        FTRACE( FPrint( _L(
            "[IAUPDATEFW] CIAUpdateFWFotaModel::FindFwUpdPkgWithStateL(): profileid = %d, pkgstate = %d" ),
             idList[i], (TInt) pkgState ) );
        if ( pkgState == aState )
            {
            retval = idList[ i ];
            }
        }

    FTRACE( FPrint( _L(
        "[IAUPDATEFW] CIAUpdateFWFotaModel::FindFwUpdPkgWithStateL(): completed, retval = %d" ),
         retval ) );
    idList.Close(); 
    
    return retval;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::InitiateFwUpdInstall
// -----------------------------------------------------------------------------
//
 TInt CIAUpdateFWFotaModel::InitiateFwUpdInstall( TInt aPkgId, TInt aProfileId )
    {
    FTRACE( FPrint( _L(
        "[IAUPDATEFW] CIAUpdateFWFotaModel::InitiateFwUpdInstall(): aPkgId = %d, aProfileId = %d" ),
        aPkgId, aProfileId ) );

    TInt retval = KErrGeneral;
    retval = iFotaEngine.Update( aPkgId, aProfileId, KNullDesC8, KNullDesC8 );

    FTRACE( FPrint( _L(
        "[IAUPDATEFW] CIAUpdateFWFotaModel::InitiateFwUpdInstall() completed, retval = %d" ),
        retval ) );
    return retval;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::CurrentFwVersionString
// If the software version retrieval fails, the aVersionstring is not modified.
// -----------------------------------------------------------------------------
//
TInt CIAUpdateFWFotaModel::CurrentFwVersionString( TDes& aVersionString )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::CurrentFwVersionString()" );
    TBuf< KSysUtilVersionTextLength > buf;

    TInt err = SysUtil::GetSWVersion( buf );
    if ( err == KErrNone )
        {
        _LIT( separator, "\n" );
        TInt location = buf.Find( separator );
        if ( location != KErrNotFound)
            {
            // Delete the separator and the text after it. We are
            // only interested in the first section.
            buf.Delete( location, (buf.Length() - location) );
            }

        aVersionString.Copy( buf.Left( aVersionString.MaxLength() ) );
        }
    FTRACE( FPrint( _L(
        "[IAUPDATEFW] CIAUpdateFWFotaModel::CurrentFwVersionString() completed, err = %d, string = \"%S\"" ),
        err, &aVersionString ) );

    return err;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::LastUpdateTime
// -----------------------------------------------------------------------------
//
TInt CIAUpdateFWFotaModel::LastUpdateTime( TTime& aTime )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::LastUpdateTime()" );
    TInt retval = iFotaEngine.LastUpdate( aTime );
    FTRACE( FPrint( _L(
        "[IAUPDATEFW] CIAUpdateFWFotaModel::LastUpdateTime() completed, err = %d" ),
        retval ) );
    return retval;
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
// CIAUpdateFWFotaModel::FwUpdStatesChangedL
// The array is reseted at the end of the execution to save memory, since at
// the moment the information is not needed multiple times.
// -----------------------------------------------------------------------------
//
 TBool CIAUpdateFWFotaModel::FwUpdStatesChangedL()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWFotaModel::FwUpdStatesChangedL()" );
    
    TBool retval = EFalse;

    TInt configFlags( 0 );
        TInt SetGenValue(0);
        CRepository* centrep = NULL;
        TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );
        if ( centrep )
            {
            centrep->Get( KDevManSessionType, configFlags );
            
            }
        if ( err != KErrNone ) 
            {
            User::Leave( err );
            }
        else
            {
            
            centrep->Set( KDevManSessionType, SetGenValue );
            }      
        if(centrep)    
           {
           delete centrep;
           }
        if ( configFlags == 1 )
			{
			retval = ETrue;
			}           
        // configFlags=2 for FOTA Package not downloaded case ,1- successful download
    return retval;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWFotaModel::CIAUpdateFWFotaModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateFWFotaModel::CIAUpdateFWFotaModel( /*CNSCDocument* aDocument*/ ) /*:
    iDocument( aDocument )*/
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
