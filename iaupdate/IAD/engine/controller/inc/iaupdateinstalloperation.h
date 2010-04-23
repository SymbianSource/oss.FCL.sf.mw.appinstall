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
* Description:   CIAUpdateInstallOperation 
*
*/



#ifndef IA_UPDATE_INSTALL_OPERATION_H
#define IA_UPDATE_INSTALL_OPERATION_H


#include <ncdinstalloperationobserver.h>

#include "iaupdatenormalcontentoperation.h"

class CIAUpdateNode;


/**
 * CIAUpdateInstallOperation can handle installing of node content.
 */
class CIAUpdateInstallOperation : public CIAUpdateNormalContentOperation,
                                  public MNcdInstallOperationObserver
    {

public:

    /**
     * @see CIAUpdateInstallOperation::CIAUpdateInstallOperation
     * @return CIAUpdateInstallOperation* Created object.
     * Ownership is transferred.
     */
    static CIAUpdateInstallOperation* NewL(
        CIAUpdateNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );
    
    /**
     * @see CIAUpdateInstallOperation::NewL
     */ 
    static CIAUpdateInstallOperation* NewLC(
        CIAUpdateNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );

    /**
     * Destructor
     */
    virtual ~CIAUpdateInstallOperation();


public: // MNcdInstallOperationObserver

    /**
     * @see MNcdInstallOperationObserver::InstallProgress
     */
    virtual void InstallProgress( 
        MNcdInstallOperation& aOperation, TNcdProgress aProgress );

    /**
     * @see MNcdInstallOperationObserver::QueryReceived
     */
    virtual void QueryReceived( 
        MNcdInstallOperation& aOperation, MNcdQuery* aQuery );

    /**
     * @see MNcdInstallOperationObserver::OperationComplete
     */
    virtual void OperationComplete( 
        MNcdInstallOperation& aOperation, TInt aError );


protected: // CIAUpdateNormalContentOperation

    /**
     * @see CIAUpdateContentOperation::HandleContentL
     */ 
    virtual MNcdOperation* HandleContentL();


private:
    
    // Prevent these if not implemented
    CIAUpdateInstallOperation( 
        const CIAUpdateInstallOperation& aObject );
    CIAUpdateInstallOperation& operator =( 
        const CIAUpdateInstallOperation& aObject );


    /**
     * @see CIAUpdateNormalContentOperation::CIAUpdateNormalContentOperation
     */
    CIAUpdateInstallOperation(
        CIAUpdateNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );

    /**
     * ConstructL
     */
    void ConstructL();


    /** 
     * Starts the silent install operation for given node.
     *
     * @return MNcdOperation* Operation that is started.
     */
    MNcdOperation* InstallL();

    
private: // data

    };

#endif // IA_UPDATE_INSTALL_OPERATION_H

