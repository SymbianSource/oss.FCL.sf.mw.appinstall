/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of CIAUpdateFwNode class 
*
*/


#ifndef IA_UPDATE_FW_NODE_IMPL_H
#define IA_UPDATE_FW_NODE_IMPL_H


#include <e32base.h>

#include "iaupdatebasenodeimpl.h"
#include "iaupdatefwnode.h"
#include "iaupdatecontentoperationobserver.h"

class CIAUpdateContentOperation;


/**
 *
 */
class CIAUpdateFwNode : public CIAUpdateBaseNode, 
                        public MIAUpdateFwNode,
                        public MIAUpdateContentOperationObserver
    {

public:

    /**
     * @see CIAUpdateFwNode::CIAUpdateFwNode
     * @see CIAUpdateFwNode::ConstructL
     **/
    static CIAUpdateFwNode* NewLC( MNcdNode* aNode,
                                   CIAUpdateController& aController,
                                   MIAUpdateFwNode::TFwUpdateType aFwType );
    
    /**
     * @see CIAUpdateFwNode::NewLC
     **/
    static CIAUpdateFwNode* NewL( MNcdNode* aNode,
                                  CIAUpdateController& aController,
                                  MIAUpdateFwNode::TFwUpdateType aFwType );
    
    
    /**
     * Destructor
     **/
    virtual ~CIAUpdateFwNode();


public: // MIAUpdateFwNode

    /**
     * @see MIAUpdateFwNode::Type
     **/
    virtual TFwUpdateType FwType() const;

    /**
     * @see MIAUpdateFwNode::Version1
     */
    virtual const TDesC& FwVersion1() const;

    /**
     * @see MIAUpdateFwNode::Version2
     */
    virtual const TDesC& FwVersion2() const;

    /**
     * @see MIAUpdateFwNode::Version3
     */
    virtual const TDesC& FwVersion3() const;

    /**
     * @see MIAUpdateFwNode::InitDownloadDataL
     */
    virtual TBool InitDownloadDataL( 
        MIAUpdateFwNodeObserver& aObserver );

    /**
     * @see MIAUpdateFwNode::ContentUrl
     */
    virtual const TDesC& ContentUrl() const;

    /**
     * @see MIAUpdateFwNode:::NodeType
     */
    virtual TNodeType NodeType() const;
    
    /**
     * @see MIAUpdateFwNode::Base
     */
    virtual MIAUpdateBaseNode& Base();


public: // MIAUpdateContentOperationObserver

    /**
     * @see MIAUpdateContentOperationObserver::ContentOperationComplete
     */
    virtual void ContentOperationComplete( CIAUpdateBaseNode& aNode, 
                                           TInt aError );

    /**
     * @see MIAUpdateContentOperationObserver::ContentOperationProgress
     */
    virtual void ContentOperationProgress( CIAUpdateBaseNode& aNode, 
                                           TInt aProgress, 
                                           TInt aMaxProgress );


private:

    // Prevent these if not implemented
    CIAUpdateFwNode( const CIAUpdateFwNode& aObject );
    CIAUpdateFwNode& operator =( const CIAUpdateFwNode& aObject );


    /**
     * @see CIAUpdateBaseNode::CIAUpdateBaseNode
     * @param aFwType This defines what kind of firmware update
     * this node is.
     */
    CIAUpdateFwNode( CIAUpdateController& aController,
                     MIAUpdateFwNode::TFwUpdateType aFwType );

    /**
     * @see CIAUpdateBaseNode::ConstructL
     */    
    void ConstructL( MNcdNode* aNode );


    // Sets the content url information to the private variable.
    // The information is gotten from the purchase history.
    void SetContentUrlL();
        
    void SetNameL( const MNcdNodeMetadata& aMetaData );


private: // data

    TFwUpdateType iType;

    // Not owned.
    // This is set when operation that requires observation is
    // started. NULL if no operation is going on.
    MIAUpdateFwNodeObserver* iObserver; 

    // This operation is used to do purchasing and to
    // initialize the download related information.
    CIAUpdateContentOperation* iPurchaseOperation;

    // Content URL that is set when purchase operation finishes.
    HBufC* iContentUrl;

    // Flag to inform if operation is cancelled.
    TBool iCancelling;

    };
    
    
#endif  //  IA_UPDATE_FW_NODE_IMPL_H
