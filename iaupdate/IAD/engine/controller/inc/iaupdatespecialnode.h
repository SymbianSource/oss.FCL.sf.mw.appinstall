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



#ifndef IA_UPDATE_SPECIAL_NODE_H
#define IA_UPDATE_SPECIAL_NODE_H


#include "iaupdatenodeimpl.h"


/**
 * CIAUpdateSpecialNode
 * 
 * This node is created for the self update content.
 */
class CIAUpdateSpecialNode : public CIAUpdateNode
    {

public:

    /**
     * @see CIAUpdateNode::NewLC
     **/
    static CIAUpdateSpecialNode* NewLC( MNcdNode* aNode,
                                        CIAUpdateController& aController );
    
    /**
     * @see CIAUpdateSpecialNode::NewLC
     **/
    static CIAUpdateSpecialNode* NewL( MNcdNode* aNode,
                                       CIAUpdateController& aController );
    
    
    /**
     * Destructor
     **/
    virtual ~CIAUpdateSpecialNode();


public: // CIAUpdateNode    

    /**
     * This implementation always returns ETrue.
     * @see MIAUpdateNode::IsSelfUpdate
     */
    virtual TBool IsSelfUpdate() const;

    /**
     * @see CIAUpdateNode::SetInstallStatusToPurchaseHistoryL
     */
    virtual void SetInstallStatusToPurchaseHistoryL( 
        TInt aErrorCode, TBool aForceVisibleInHistory );


private:

    // Prevent these if not implemented
    CIAUpdateSpecialNode( const CIAUpdateSpecialNode& aObject );
    CIAUpdateSpecialNode operator =( const CIAUpdateSpecialNode& aObject );


    /**
     * @see CIAUpdateNode::NewLC
     */
    CIAUpdateSpecialNode( CIAUpdateController& aController );

    /**
     * @see CIAUpdateNode::NewLC
     */    
    void ConstructL( MNcdNode* aNode );


private: // data

    };
    
    
#endif  //  IA_UPDATE_SPECIAL_NODE_H
