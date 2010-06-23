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
* Description:   This module contains the header file of CIAUpdateNodeId
*                class.
*/




#ifndef IAUPDATENODEID_H
#define IAUPDATENODEID_H

//INCLUDES
#include <e32base.h>

#include "iaupdateanynode.h"


/**
 *
 */
class CIAUpdateNodeId : public CBase
    {

public:

    static CIAUpdateNodeId* NewL();

    static CIAUpdateNodeId* NewLC();
    
    
    virtual ~CIAUpdateNodeId();

    const TDesC& Id() const;
    void SetIdL( const TDesC& aId );
    
    const TDesC& Namespace() const;
    void SetNamespaceL( const TDesC& aNamespace );
    
    MIAUpdateAnyNode::TNodeType NodeType() const;
    void SetNodeType( MIAUpdateAnyNode::TNodeType aNodeType );
    
    TBool Selected() const;
    void SetSelected( TInt aSelected );
    

private:

    // Prevent these if not implemented
    CIAUpdateNodeId( const CIAUpdateNodeId& aObject );
    CIAUpdateNodeId& operator =( const CIAUpdateNodeId& aObject );


    CIAUpdateNodeId();
    
    void ConstructL();


private: // data
    
    HBufC* iId;
    HBufC* iNamespace;
    MIAUpdateAnyNode::TNodeType  iNodeType;
    TBool                        iSelected;
    
    };
		
#endif // IAUPDATENODEID_H

