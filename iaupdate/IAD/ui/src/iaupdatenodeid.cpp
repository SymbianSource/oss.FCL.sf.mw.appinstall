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
* Description:   This module contains the implementation of CIAUpdateNodeId
*                class member functions.
*/




#include "iaupdatenodeid.h"


// -----------------------------------------------------------------------------
// CIAUpdateNodeId::NewL
//
// -----------------------------------------------------------------------------
//
CIAUpdateNodeId* CIAUpdateNodeId::NewL()
    {
    CIAUpdateNodeId* self =
        CIAUpdateNodeId::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeId::NewLC
//
// -----------------------------------------------------------------------------
//
CIAUpdateNodeId* CIAUpdateNodeId::NewLC()
    {
    CIAUpdateNodeId* self =
        new( ELeave) CIAUpdateNodeId();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeId::CIAUpdateNodeId
//
// -----------------------------------------------------------------------------
//
CIAUpdateNodeId::CIAUpdateNodeId()
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeId::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeId::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeId::~CIAUpdateNodeId
//
// -----------------------------------------------------------------------------
//
CIAUpdateNodeId::~CIAUpdateNodeId()
    {
    delete iId;
    delete iNamespace;
    }

// -----------------------------------------------------------------------------
// CIAUpdateNodeId::Id
//
// -----------------------------------------------------------------------------
//
const TDesC& CIAUpdateNodeId::Id() const
    {
    if ( !iId )
        {
        return KNullDesC;
        }
        
    return *iId;
    }
    

// -----------------------------------------------------------------------------
// CIAUpdateNodeId::SetIdL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeId::SetIdL( const TDesC& aId )
    {
    HBufC* tmp( aId.AllocL() );
    delete iId;
    iId = tmp;
    }
    
    
// -----------------------------------------------------------------------------
// CIAUpdateNodeId::Namespace
//
// -----------------------------------------------------------------------------
//
const TDesC& CIAUpdateNodeId::Namespace() const
    {
    if ( !iNamespace )
        {
        return KNullDesC;
        }
    
    return *iNamespace;
    }
    

// -----------------------------------------------------------------------------
// CIAUpdateNodeId::SetNamespaceL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeId::SetNamespaceL( const TDesC& aNamespace )
    {
    HBufC* tmp( aNamespace.AllocL() );
    delete iNamespace;
    iNamespace = tmp;
    }

// -----------------------------------------------------------------------------
// CIAUpdateNodeId::NodeType
//
// -----------------------------------------------------------------------------
//
MIAUpdateAnyNode::TNodeType CIAUpdateNodeId::NodeType() const
    {
    return iNodeType;
    }

// -----------------------------------------------------------------------------
// CIAUpdateNodeId::SetNodeType
//
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeId::SetNodeType( MIAUpdateAnyNode::TNodeType aNodeType )
    {
    iNodeType = aNodeType;
    }

// -----------------------------------------------------------------------------
// CIAUpdateNodeId::Selected
//
// -----------------------------------------------------------------------------
//
TBool CIAUpdateNodeId::Selected() const
    {
    return iSelected;
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateNodeId::SetSelected
//
// -----------------------------------------------------------------------------
//    
void CIAUpdateNodeId::SetSelected( TInt aSelected )
    {
    iSelected = aSelected; 
    }
