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
* Description:   Contains CCatalogsInterfaceIdentifier class implementation
*
*/


#include <e32err.h>

#include "catalogsinterfaceidentifier.h"
#include "catalogsbase.h"
#include "catalogsdebug.h"


CCatalogsInterfaceIdentifier* CCatalogsInterfaceIdentifier::NewL( TAny* aInterfaceObject,
                                                                  CCatalogsInterfaceBase* aInterfaceBaseObject,
                                                                  TInt aInterfaceId )
    {
    if( aInterfaceObject == NULL || aInterfaceBaseObject == NULL )
        {
        User::Leave( KErrArgument );
        }
        
    CCatalogsInterfaceIdentifier* self = 
        CCatalogsInterfaceIdentifier::NewLC( aInterfaceObject, aInterfaceBaseObject, aInterfaceId );
    CleanupStack::Pop( self );
    return self;    
    }
    
CCatalogsInterfaceIdentifier* CCatalogsInterfaceIdentifier::NewLC( TAny* aInterfaceObject,
                                                                   CCatalogsInterfaceBase* aInterfaceBaseObject,
                                                                   TInt aInterfaceId )
    {
    if( aInterfaceObject == NULL || aInterfaceBaseObject == NULL )
        {
        User::Leave( KErrArgument );
        }
        
    CCatalogsInterfaceIdentifier* self = 
        new( ELeave ) CCatalogsInterfaceIdentifier( aInterfaceObject, aInterfaceBaseObject, aInterfaceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;            
    }

CCatalogsInterfaceIdentifier::~CCatalogsInterfaceIdentifier()
    {
    
    }

const TAny* CCatalogsInterfaceIdentifier::InterfaceObject() const
    {
    return iInterfaceObject;
    }
    
const CCatalogsInterfaceBase* CCatalogsInterfaceIdentifier::InterfaceBaseObject() const 
    {
    return iInterfaceBaseObject;
    }
    
TInt CCatalogsInterfaceIdentifier::InterfaceId() const
    {
    return iInterfaceId;
    }
    

CCatalogsInterfaceIdentifier::CCatalogsInterfaceIdentifier( TAny* aInterfaceObject,
                                                            CCatalogsInterfaceBase* aInterfaceBaseObject,
                                                            TInt aInterfaceId )
: iInterfaceObject( aInterfaceObject ), iInterfaceBaseObject( aInterfaceBaseObject ),
  iInterfaceId( aInterfaceId )
    {
    DLTRACEIN(( "obj: %X, baseObj: %X, id: %d", aInterfaceObject, aInterfaceBaseObject, aInterfaceId ));
    }
    
    
void CCatalogsInterfaceIdentifier::ConstructL()
    {
    
    }

