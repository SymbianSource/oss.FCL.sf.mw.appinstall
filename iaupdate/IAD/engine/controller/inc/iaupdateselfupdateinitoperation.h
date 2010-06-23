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
* Description:   CIAUpdateSelfUpdateInitOperation 
*
*/


#ifndef IA_UPDATE_SELF_UPDATE_INIT_OPERATION_H
#define IA_UPDATE_SELF_UPDATE_INIT_OPERATION_H


#include "iaupdatecontentoperation.h"
#include "iaupdateselfupdaterinitobserver.h"

class CIAUpdateNode;
class CIAUpdateSelfUpdaterInitializer;


/**
 * CIAUpdateSelfUpdateInitOperation
 * This operation does not install self updates itself but 
 * initializes the data that will be used by the self updater program
 * that handles the actual installation.
 */
class CIAUpdateSelfUpdateInitOperation : public CIAUpdateContentOperation,
                                         public MIAUpdateSelfUpdaterInitObserver
    {

public:

    /**
     * @see CIAUpdateSelfUpdateInitOperation::CIAUpdateSelfUpdateInitOperation
     * @return CIAUpdateSelfUpdateInitOperation* Created object.
     * Ownership is transferred.
     */
    static CIAUpdateSelfUpdateInitOperation* NewL( 
        CIAUpdateNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );
    
    /**
     * @see CIAUpdateSelfUpdateInitOperation:: NewL
     */ 
    static CIAUpdateSelfUpdateInitOperation* NewLC(
        CIAUpdateNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );


    /**
     * Destructor
     */
    virtual ~CIAUpdateSelfUpdateInitOperation();


public: // MIAUpdateSelfUpdaterInitObserver

    /**
     * @see MIAUpdateSelfUpdaterInitObserver::SelfUpdateInitComplete
     */
    virtual void SelfUpdateInitComplete( TInt aError );
    

protected: // CIAUpdateContentOperation

    /**
     * @see CIAUpdateContentOperation::StartOperationL
     */
    virtual TBool StartOperationL();

    /**
     * @see CIAUpdateContentOperation::CancelOperation
     */
    virtual void CancelOperation();


private:
    
    // Prevent these if not implemented
    CIAUpdateSelfUpdateInitOperation( const CIAUpdateSelfUpdateInitOperation& aObject );
    CIAUpdateSelfUpdateInitOperation& operator =( const CIAUpdateSelfUpdateInitOperation& aObject );


    /**
     * @see CIAUpdateContentOperation::CIAUpdateContentOperation
     */ 
    CIAUpdateSelfUpdateInitOperation( 
        CIAUpdateNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );

    /**
     * 2nd. phase constructor
     */ 
    void ConstructL();
    

private: // data

    // Initializer of the self updater. 
    // Self update content file paths are needed to be set for the actual self 
    // updater.
    CIAUpdateSelfUpdaterInitializer* iOperation;

    };

#endif // IA_UPDATE_SELF_UPDATE_INIT_OPERATION_H

