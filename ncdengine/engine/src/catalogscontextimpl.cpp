/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of context-class
*
*/


#include "catalogscontextimpl.h"
#include "catalogsdebug.h"



// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Warning: default-constructor leaves iSecureId in a undefined state
// ---------------------------------------------------------------------------
//
CCatalogsContextImpl::CCatalogsContextImpl( TUid aFamilyId, TSecureId aSecureId,
    TInt aInstanceId ) :
    iFamilyId( aFamilyId ),
    iSecureId( aSecureId ),
    iInstanceId( aInstanceId )
    {
    DLTRACEIN(("this-ptr: %x", this));
    }

void CCatalogsContextImpl::ConstructL()
    {
    
    }

CCatalogsContextImpl* CCatalogsContextImpl::NewL( TUid aFamilyId, TSecureId aSecureId,
    TInt aInstanceId )
    {
    CCatalogsContextImpl* self = CCatalogsContextImpl::NewLC( aFamilyId, aSecureId,
        aInstanceId );
    CleanupStack::Pop( self );
    return self;
    }

CCatalogsContextImpl* CCatalogsContextImpl::NewLC( TUid aFamilyId, TSecureId aSecureId,
    TInt aInstanceId )
    {
    CCatalogsContextImpl* self = new( ELeave ) CCatalogsContextImpl( aFamilyId,
        aSecureId, aInstanceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CCatalogsContextImpl::~CCatalogsContextImpl()
    {
    DLTRACEIN(("this-ptr: %x", this));
    for( TInt i=0; i<iProviderData.Count(); i++ )
        {
        iProviderData[i].Close();
        }
    iProviderData.Close();
    }


// ---------------------------------------------------------------------------
// Function to store familyid
// ---------------------------------------------------------------------------
//
void CCatalogsContextImpl::SetFamilyId( TUid aNewId )
    {
    DLTRACEIN(("%X",aNewId.iUid));
    iFamilyId = aNewId;
    }

// ---------------------------------------------------------------------------
// Function to store secure id
// ---------------------------------------------------------------------------
//
void CCatalogsContextImpl::SetSecureId( TSecureId aNewId )
    {
    DLTRACEIN(("%X",aNewId.iId));
    iSecureId = aNewId;
    }

// ---------------------------------------------------------------------------
// From class MCatalogsContext.
// Function that returns family id.
// ---------------------------------------------------------------------------
//
TUid CCatalogsContextImpl::FamilyId() const
    {
    return iFamilyId;
    }

// ---------------------------------------------------------------------------
// From class MCatalogsContext.
// Function that returns secure id.
// ---------------------------------------------------------------------------
//
TSecureId CCatalogsContextImpl::SecureId() const
    {
    return iSecureId;
    }

// ---------------------------------------------------------------------------
// From class MCatalogsContext.
// Function that returns instance id.
// ---------------------------------------------------------------------------
//
TInt CCatalogsContextImpl::InstanceId() const
    {
    return iInstanceId;
    }

// ---------------------------------------------------------------------------
// From class MCatalogsContext.
// Function that returns provider specific data buffer handle.
// ---------------------------------------------------------------------------
//
RBuf8& CCatalogsContextImpl::ProviderDataL( TInt aProviderIndex )
    {
    while( iProviderData.Count() <= aProviderIndex )
        {
        RBuf8 emptyBuf;
        iProviderData.AppendL( emptyBuf );
        }
        
    return iProviderData[ aProviderIndex ];
    }
