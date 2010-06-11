/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CNcdExtendedInstallInfo
*
*/


#include "ncdextendedinstallinfo.h"
#include "catalogsutils.h"


CNcdExtendedInstallInfo* CNcdExtendedInstallInfo::NewL()
    {
    CNcdExtendedInstallInfo* self =
         CNcdExtendedInstallInfo::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CNcdExtendedInstallInfo* CNcdExtendedInstallInfo::NewLC()
    {
    CNcdExtendedInstallInfo* self = 
        new (ELeave) CNcdExtendedInstallInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }



CNcdExtendedInstallInfo* CNcdExtendedInstallInfo::NewLC( 
    const MNcdPurchaseInstallInfo& aInstallInfo,
    const TDesC& aMimeType,
    TBool aLaunchable )
    {    
    CNcdExtendedInstallInfo* self = 
        new (ELeave) CNcdExtendedInstallInfo( aLaunchable );
    CleanupStack::PushL( self );
    self->ConstructL( aInstallInfo, aMimeType );
    return self;
    }



void CNcdExtendedInstallInfo::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    CNcdPurchaseInstallInfo::InternalizeL( aStream );    
    InternalizeDesL( iMimeType, aStream );
    iLaunchable = aStream.ReadInt32L();
    InternalizeDesL( iParameter, aStream );
    InternalizeEnumL( iContentType, aStream );    
    InternalizeEnumL( iInstalledStatus, aStream );
    iUriExists = aStream.ReadInt32L();
    DLTRACEOUT(("Internalization successful"));
    }


void CNcdExtendedInstallInfo::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    CNcdPurchaseInstallInfo::ExternalizeL( aStream );    
    ExternalizeDesL( *iMimeType, aStream );
    aStream.WriteInt32L( iLaunchable );
    ExternalizeDesL( *iParameter, aStream );    
    ExternalizeEnumL( iContentType, aStream );
    ExternalizeEnumL( iInstalledStatus, aStream );
    aStream.WriteInt32L( iUriExists );
    DLTRACEOUT(("Externalization successful"));
    }



const TDesC& CNcdExtendedInstallInfo::MimeType() const
    {
    DASSERT( iMimeType );
    return *iMimeType;
    }


TBool CNcdExtendedInstallInfo::IsLaunchable() const
    {    
    return iLaunchable;
    }


void CNcdExtendedInstallInfo::SetLaunchable( TBool aLaunchable )
    {
    DLTRACEIN(("aLaunchable: %d", aLaunchable));
    iLaunchable = aLaunchable;
    }


void CNcdExtendedInstallInfo::SetContentType( 
    MNcdInstalledContent::TInstalledContentType aType )
    {
    iContentType = aType;
    }


MNcdInstalledContent::TInstalledContentType 
    CNcdExtendedInstallInfo::ContentType() const
    {
    return iContentType;
    }


void CNcdExtendedInstallInfo::SetParameterL( const TDesC& aParam )
    {
    DLTRACEIN((""));
    AssignDesL( iParameter, aParam );
    }

const TDesC& CNcdExtendedInstallInfo::Parameter() const
    {
    DASSERT( iParameter );
    return *iParameter;
    }


void CNcdExtendedInstallInfo::SetInstalledStatus( 
    TNcdApplicationStatus aInstalledStatus )
    {
    DLTRACEIN(("aInstalledStatus: %d", aInstalledStatus ));
    iInstalledStatus = aInstalledStatus;
    }
    
TNcdApplicationStatus CNcdExtendedInstallInfo::InstalledStatus() const
    {
    return iInstalledStatus;
    }
    
void CNcdExtendedInstallInfo::SetUriExists( TBool aUriExists )
    {
    iUriExists = aUriExists;
    }

TBool CNcdExtendedInstallInfo::UriExists() const
    {
    return iUriExists;
    }

CNcdExtendedInstallInfo::CNcdExtendedInstallInfo( TBool aLaunchable )
    : CNcdPurchaseInstallInfo(), 
    iLaunchable( aLaunchable ),
    iInstalledStatus( ENcdApplicationNotInstalled )
    {
    }


void CNcdExtendedInstallInfo::ConstructL()
    {
    DLTRACEIN((""));
    CNcdPurchaseInstallInfo::BaseConstructL();
    iMimeType = KNullDesC().AllocL();  
    iParameter = KNullDesC().AllocL();  
    }


void CNcdExtendedInstallInfo::ConstructL(
    const MNcdPurchaseInstallInfo& aInstall,
    const TDesC& aMimeType )
    {
    DLTRACEIN(( _L("MimeType: %S"), &aMimeType ));
    // Initialize the base class
    CNcdPurchaseInstallInfo::BaseConstructL( aInstall );

    iMimeType = aMimeType.AllocL();
    iParameter = KNullDesC().AllocL(); 
    }
        

CNcdExtendedInstallInfo::~CNcdExtendedInstallInfo()
    {
    delete iMimeType;    
    delete iParameter;
    }
