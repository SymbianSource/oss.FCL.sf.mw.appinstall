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
* Description:   This file contains the header file of CIAUpdateServicePackNode class 
*
*/


#ifndef IA_UPDATE_SERVICE_PACK_NODE_H
#define IA_UPDATE_SERVICE_PACK_NODE_H


#include "iaupdatenodeimpl.h"


/**
 * CIAUpdateServicePackNode
 * This class provides service pack specific functionality
 * for the node.
 */
class CIAUpdateServicePackNode : public CIAUpdateNode
    {

public:

    /**
     * @see IAUpdateNodeFactory::CreateNodeLC
     **/
    static CIAUpdateServicePackNode* NewLC( 
        MNcdNode* aNode,
        CIAUpdateController& aController );
    
    /**
     * @see CIAUpdateServicePackNode::NewLC
     **/
    static CIAUpdateServicePackNode* NewL( 
        MNcdNode* aNode,
        CIAUpdateController& aController );
    
    
    /**
     * Destructor
     **/
    virtual ~CIAUpdateServicePackNode();


public: // CIAUpdateNode

    /**
     * @see CIAUpdateNode::SetInstallStatusToPurchaseHistoryL
     */
    virtual void SetInstallStatusToPurchaseHistoryL( 
        TInt aErrorCode, TBool aForceVisibleInHistory );


    /**
     * This implementation checks if the given error code
     * has correct base value included before giving the
     * error value to parent class function.
     *
     * @see CIAUpdateNode::SetIdleErrorToPurchaseHistoryL
     */
    virtual void SetIdleErrorToPurchaseHistoryL( 
        TInt aError, TBool aForceVisibleInHistory );
    

    /**
     * @see CIAUpdateNode::Type
     * @return TPackageType This returns always 
     * MIAUpdateNode::EPackageTypeServicePack
     * because this class is service pack.
     **/
    virtual TPackageType Type() const;


protected: // CIAUpdateNode

    /**
     * @note This function should not be called before 
     * dependency chains are created.
     *
     * @see CIAUpdateNode::IsPurchasedL
     * @return ETrue if all the direct dependencies are purchased.
     **/
    virtual TBool IsPurchasedL() const;
    

    /**
     * @note This function should not be called before 
     * dependency chains are created.
     *
     * @see CIAUpdateNode::IsDownloadedL
     * @return ETrue if all the direct dependencies are downloaded.
     **/
    virtual TBool IsDownloadedL() const;
    

    /**
     * @note This function should not be called before 
     * dependency chains are created.
     *
     * @see CIAUpdateNode::IsInstalledL
     * @return ETrue if all the direct dependencies are installed.
     **/
    virtual TBool IsInstalledL() const;


protected:

    /**
     * @see CIAUpdateNode::CIAUpdateNode
     */
    CIAUpdateServicePackNode( CIAUpdateController& aController );

    /**
     * @see CIAUpdateNode::ConstructL
     */    
    virtual void ConstructL( MNcdNode* aNode );


private:

    // Prevent these if not implemented
    CIAUpdateServicePackNode( const CIAUpdateServicePackNode& aObject );
    CIAUpdateServicePackNode& operator =( const CIAUpdateServicePackNode& aObject );


private: // data

    };
    
    
#endif  // IA_UPDATE_SERVICE_PACK_NODE_H
