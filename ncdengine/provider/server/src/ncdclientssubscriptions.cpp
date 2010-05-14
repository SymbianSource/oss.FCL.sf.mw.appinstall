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
* Description:  
*
*/


#include "ncdclientssubscriptions.h"
#include "ncdsubscriptiongroup.h"
#include "catalogsutils.h"
#include "ncdsubscriptionssourceidentifier.h"

// ======== MEMBER FUNCTIONS ========

CNcdClientsSubscriptions::CNcdClientsSubscriptions( TUid aId ) :
    iId( aId )
    {
    }


void CNcdClientsSubscriptions::ConstructL()
    {
    }


CNcdClientsSubscriptions* CNcdClientsSubscriptions::NewL( TUid aId )
    {
    CNcdClientsSubscriptions* self =
        CNcdClientsSubscriptions::NewLC( aId );
    CleanupStack::Pop( self );
    return self;
    }


CNcdClientsSubscriptions* CNcdClientsSubscriptions::NewLC( TUid aId )
    {
    CNcdClientsSubscriptions* self =
        new( ELeave ) CNcdClientsSubscriptions( aId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdClientsSubscriptions::~CNcdClientsSubscriptions()
    {
    // Should close all objects in iClientsSubscriptions
    ResetAndCloseArray( iClientsSubscriptions );
    }


// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdClientsSubscriptions::AddGroupL( CNcdSubscriptionGroup* aGroup )
    {
    iClientsSubscriptions.AppendL( aGroup );
    }

void CNcdClientsSubscriptions::RemoveGroup(
    CNcdSubscriptionGroup* aGroup )
    {
    TInt groupIndex( iClientsSubscriptions.Count() - 1 );
    while ( groupIndex > -1  )
        {
        // Comparison of pointers
        if ( iClientsSubscriptions[groupIndex] == aGroup )
            {
            iClientsSubscriptions[groupIndex]->Close();
            iClientsSubscriptions.Remove( groupIndex );
            return;
            }
                
        --groupIndex;
        } 
    }

RPointerArray<CNcdSubscriptionGroup>& CNcdClientsSubscriptions::Groups()
    {
    return iClientsSubscriptions;
    }

TUid CNcdClientsSubscriptions::ClientId() const
    {
    return iId;
    }
