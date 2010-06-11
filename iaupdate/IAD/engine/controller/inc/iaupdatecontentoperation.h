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
* Description:   CIAUpdateContentOperation 
*
*/


#ifndef IA_UPDATE_CONTENT_OPERATION_H
#define IA_UPDATE_CONTENT_OPERATION_H


#include <e32base.h>
#include "iaupdateoperation.h"

class CIAUpdateBaseNode;
class MIAUpdateContentOperationObserver;


/**
 * CIAUpdateContentOperation is an abstract base class
 * for node content operations.
 */
class CIAUpdateContentOperation : public CBase,
                                  public MIAUpdateOperation
    {

public:

    /**
     * Destructor
     */
    virtual ~CIAUpdateContentOperation();


public: // MIAUpdateOperation 

    /**
     * @see MIAUpdateOperation::StartOperationL
     *
     * @note Notice, return value is always EFalse 
     * because this implementation does only checkings but
     * does not start operation. Also, iInUse flag is left
     * to EFalse. So, it is child classes resonsibility set the
     * correct iInUse flag if operation is started. 
     *
     * @return TBool EFalse always.
     */
    virtual TBool StartOperationL();
 
    /**
     * @see MIAUpdateOperation::StartOperationL
     */
    virtual void CancelOperation() = 0;


protected:

    /**
     * Constructor
     *
     * @param aNode Node of this operation.
     * @param aObserver The observer whose call back will be called
     * when operation is finished.     
     */ 
    CIAUpdateContentOperation( CIAUpdateBaseNode& aNode,
                               MIAUpdateContentOperationObserver& aObserver );

    /**
     * 2nd. phase constructor
     */ 
    virtual void ConstructL();


    /**
     * Called when operation completes.
     *
     * @param aError Error code
     */
    void OperationComplete( TInt aError );
    

    /**
     * Called when operation progresses.
     */
    void OperationProgress( TInt aProgress, TInt aMaxProgress );
    

    /**
     * @return CIAUpdateBaseNode& Node whose operation is going on.
     */
    CIAUpdateBaseNode& Node() const;


    /**
     * @return MIAUpdateContentOperationObserver& Operation observer.
     */    
    MIAUpdateContentOperationObserver& Observer() const; 


    /**
     * @return TBool ETrue if the operation is going on. Else EFalse. 
     */
    TBool OperationInUse() const;


    /**
     * @param aInUse ETrue if the operation is going on. Else EFalse. 
     */
    void SetOperationInUse( TBool aInUse );


private:
    
    // Prevent these if not implemented
    CIAUpdateContentOperation( 
        const CIAUpdateContentOperation& aObject );
    CIAUpdateContentOperation& operator =( 
        const CIAUpdateContentOperation& aObject );

    
private: // data

    // The node that is given when the operation is created.
    CIAUpdateBaseNode& iNode;

    // The observer that is given when the operation is created.
    MIAUpdateContentOperationObserver& iObserver;

    // This flag informs if the operation is going on.
    TBool iInUse;

    };

#endif // IA_UPDATE_CONTENT_OPERATION_H

