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
* Description:   CIAUpdateNormalContentOperation 
*
*/


#ifndef IA_UPDATE_NORMAL_CONTENT_OPERATION_H
#define IA_UPDATE_NORMAL_CONTENT_OPERATION_H


#include "iaupdatecontentoperation.h"

class MNcdOperation;
class TNcdProgress;
class MNcdQuery;


/**
 * CIAUpdateNormalContentOperation is an abstract base class
 * for normal node content operations.
 */
class CIAUpdateNormalContentOperation : public CIAUpdateContentOperation
    {

public:

    /**
     * Destructor
     */
    virtual ~CIAUpdateNormalContentOperation();


protected: // CIAUpdateContentOperation

    /**
     * @see CIAUpdateContentOperation::StartOperationL
     */
    virtual TBool StartOperationL();

    /**
     * @see CIAUpdateContentOperation::CancelOperation
     */
    virtual void CancelOperation();


protected:

    /**
     * @see CIAUpdateContentOperation::CIAUpdateContentOperation
     */ 
    CIAUpdateNormalContentOperation(
        CIAUpdateBaseNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );

    /**
     * 2nd. phase constructor
     */ 
    virtual void ConstructL();


    /**
     * @return MNcdOperation* Pointer to the operation that was started.
     * NULL if operation was not needed for this node content.
     * @exception Leaves with system wide error code.
     */ 
    virtual MNcdOperation* HandleContentL() = 0;


    /**
     * Query received related to the content operation.
     *
     * This is meant for the operation observing.
     * Child classes may use this function when their
     * callbacks are called.
     *
     * @param aOperation Operation that requests query.
     * @param aQuery Query that requires response.
     */ 
    void ContentQueryReceived( MNcdOperation& aOperation, 
                               MNcdQuery* aQuery );
 

    /**
     * This is meant for the operation observing.
     * Child classes may use this function when their
     * callbacks are called.
     *
     * @param aOperation Operation that completed.
     * @param aError Error code for the operation completion.
     */
    void ContentOperationComplete( MNcdOperation& aOperation, 
                                   TInt aError );
    
    
private:
    
    // Prevent these if not implemented
    CIAUpdateNormalContentOperation( 
        const CIAUpdateNormalContentOperation& aObject );
    CIAUpdateNormalContentOperation& operator =( 
        const CIAUpdateNormalContentOperation& aObject );


    // This will call Release to existing operation, if it exists,
    // Ownership of aOperation is transferred.
    void SetOperation( MNcdOperation* aOperation );
    

private: // data

    // This is released and set to NULL when a corresponding 
    // operation finishes.
    MNcdOperation* iOperation;

    };

#endif // IA_UPDATE_NORMAL_CONTENT_OPERATION_H

