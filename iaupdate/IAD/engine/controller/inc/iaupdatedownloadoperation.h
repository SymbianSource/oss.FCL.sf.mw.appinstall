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
* Description:   CIAUpdateDownloadOperation 
*
*/



#ifndef IA_UPDATE_DOWNLOAD_OPERATION_H
#define IA_UPDATE_DOWNLOAD_OPERATION_H


#include <ncddownloadoperationobserver.h>

#include "iaupdatenormalcontentoperation.h"

class CIAUpdateNode;


/**
 * CIAUpdateDownloadOperation handles downloading of node content.
 */
class CIAUpdateDownloadOperation : public CIAUpdateNormalContentOperation,
                                   public MNcdDownloadOperationObserver
    {

public:

    /**
     * @see CIAUpdateDownloadOperation::CIAUpdateDownloadOperation
     * @return CIAUpdateDownloadOperation* Created object.
     * Ownership is transferred.
     */
    static CIAUpdateDownloadOperation* NewL(
        CIAUpdateNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );
    
    /**
     * @see CIAUpdateDownloadOperation::NewL
     */ 
    static CIAUpdateDownloadOperation* NewLC(
        CIAUpdateNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );

    /**
     * Destructor
     */
    virtual ~CIAUpdateDownloadOperation();


public: // MNcdDownloadOperationObserver
    
    /**
     * @see MNcdDownloadOperationObserver::DownloadProgress    
     */
    virtual void DownloadProgress( 
        MNcdDownloadOperation& aOperation, TNcdProgress aProgress );
        
    /**
     * @see MNcdDownloadOperationObserver::QueryReceived
     */
    virtual void QueryReceived( 
        MNcdDownloadOperation& aOperation, MNcdQuery* aQuery );

    /**
     * @see MNcdDownloadOperationObserver::OperationComplete
     */
    virtual void OperationComplete( 
        MNcdDownloadOperation& aOperation, TInt aError );    
    
    
protected: // CIAUpdateNormalContentOperation

    /**
     * @see CIAUpdateContentOperation::HandleContentL
     */ 
    virtual MNcdOperation* HandleContentL();
    

private:
    
    // Prevent these if not implemented
    CIAUpdateDownloadOperation( const CIAUpdateDownloadOperation& aObject );
    CIAUpdateDownloadOperation& operator =( const CIAUpdateDownloadOperation& aObject );


    /**
     * @see CIAUpdateNormalContentOperation::CIAUpdateNormalContentOperation
     */
    CIAUpdateDownloadOperation(
        CIAUpdateNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );

    /**
     * ConstructL
     */
    void ConstructL();


    /** 
     * Starts the download operation for given node.
     *
     * @return MNcdOperation* Operation that is started.
     */
    MNcdOperation* DownloadL();

    
private: // data

    };

#endif // IA_UPDATE_DOWNLOAD_OPERATION_H

