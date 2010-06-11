/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#include <e32std.h>

#include "iaupdateengineconfigdata.h"


CIAUpdateEngineConfigData* CIAUpdateEngineConfigData::NewL()
    {
    CIAUpdateEngineConfigData* self =
        CIAUpdateEngineConfigData::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateEngineConfigData* CIAUpdateEngineConfigData::NewLC()
    {
    CIAUpdateEngineConfigData* self =
        new( ELeave ) CIAUpdateEngineConfigData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CIAUpdateEngineConfigData::CIAUpdateEngineConfigData()
: CBase()
    {
    }


void CIAUpdateEngineConfigData::ConstructL()
    {
    iSoftwareType = KNullDesC().AllocL();
    iSoftwareVersion = KNullDesC().AllocL();
    iStorageMaxSize = KNullDesC().AllocL();
    iMasterServerUri = KNullDesC().AllocL();
    iProvisioning = KNullDesC().AllocL();
    iClientRole = KNullDesC().AllocL();
    }


CIAUpdateEngineConfigData::~CIAUpdateEngineConfigData()  
    {
    delete iSoftwareType;
    delete iSoftwareVersion;
    delete iStorageMaxSize;
    delete iMasterServerUri;
    delete iProvisioning;
    delete iClientRole;
    }


    
void CIAUpdateEngineConfigData::ResetL()
    {
    HBufC* softwareType = KNullDesC().AllocLC();
    HBufC* softwareVersion = KNullDesC().AllocLC();
    HBufC* storageMaxSize = KNullDesC().AllocLC();
    HBufC* masterServerUri = KNullDesC().AllocLC();
    HBufC* provisioning = KNullDesC().AllocLC();
    HBufC* clientRole = KNullDesC().AllocLC();

    CleanupStack::Pop( clientRole );
    delete iClientRole;
    iClientRole = clientRole;
    
    CleanupStack::Pop( provisioning );
    delete iProvisioning;
    iProvisioning = provisioning;

    CleanupStack::Pop( masterServerUri );
    delete iMasterServerUri;
    iMasterServerUri = masterServerUri;
    
    CleanupStack::Pop( storageMaxSize );
    delete iStorageMaxSize;
    iStorageMaxSize = storageMaxSize;
    
    CleanupStack::Pop( softwareVersion );
    delete iSoftwareVersion;
    iSoftwareVersion = softwareVersion;
        
    CleanupStack::Pop( softwareType );
    delete iSoftwareType;
    iSoftwareType = softwareType;
    }


const TDesC& CIAUpdateEngineConfigData::SoftwareType() const
    {
    return *iSoftwareType;
    }

void CIAUpdateEngineConfigData::SetSoftwareTypeL( const TDesC& aType )
    {
    HBufC* tmp( aType.AllocL() );
    delete iSoftwareType;
    iSoftwareType = tmp;
    }


const TDesC& CIAUpdateEngineConfigData::SoftwareVersion() const
    {
    return *iSoftwareVersion;
    }

void CIAUpdateEngineConfigData::SetSoftwareVersionL( const TDesC& aVersion )
    {
    HBufC* tmp( aVersion.AllocL() );
    delete iSoftwareVersion;
    iSoftwareVersion = tmp;
    }


const TDesC& CIAUpdateEngineConfigData::StorageMaxSize() const
    {
    return *iStorageMaxSize;
    }
    
void CIAUpdateEngineConfigData::SetStorageMaxSizeL( const TDesC& aSize )
    {
    HBufC* tmp( aSize.AllocL() );
    delete iStorageMaxSize;
    iStorageMaxSize = tmp;
    }


const TDesC& CIAUpdateEngineConfigData::MasterServerUri() const
    {
    return *iMasterServerUri;
    }

void CIAUpdateEngineConfigData::SetMasterServerUriL( const TDesC& aUri )
    {
    HBufC* tmp( aUri.AllocL() );
    delete iMasterServerUri;
    iMasterServerUri = tmp;
    }


const TDesC& CIAUpdateEngineConfigData::Provisioning() const
    {
    return *iProvisioning;
    }

void CIAUpdateEngineConfigData::SetProvisioningL( const TDesC& aProvisioning )
    {
    HBufC* tmp( aProvisioning.AllocL() );
    delete iProvisioning;
    iProvisioning = tmp;
    }


const TDesC& CIAUpdateEngineConfigData::ClientRole() const
    {
    return *iClientRole;
    }

void CIAUpdateEngineConfigData::SetClientRoleL( const TDesC& aClientRole )
    {
    HBufC* tmp( aClientRole.AllocL() );
    delete iClientRole;
    iClientRole = tmp;
    }
