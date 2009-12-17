/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef C_NCD_SERVER_REPORT_OPERATION_H
#define C_NCD_SERVER_REPORT_OPERATION_H

#include "ncdbaseoperation.h"
#include "ncdoperationobserver.h"

class CNcdReportManager;


/** 
 * Handles the server report operations.
 *
 * @see CNcdReportManager
 *
 */
class CNcdServerReportOperation : public CNcdBaseOperation
    {
public:
    
    /**
     * NewL
     *
     */
    static CNcdServerReportOperation* NewL(
        CNcdGeneralManager& aGeneralManager,
        MNcdOperationRemoveHandler& aRemoveHandler, 
        CNcdReportManager& aReportManager,
        MCatalogsSession& aSession );
    
    /**
     * Destructor
     */
    ~CNcdServerReportOperation();

    
public: // New methods

    /** 
     * Report manager getter
     */
    CNcdReportManager& ReportManager();
    
    
public: // From CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::Cancel()
     */
    void Cancel();
        

public:  // from MNcdOperationObserver

    void Progress( CNcdBaseOperation& aOperation );
    
    void QueryReceived( CNcdBaseOperation& aOperation,
                        CNcdQuery* aQuery );

    void OperationComplete( CNcdBaseOperation* aOperation,
                            TInt aError );

protected: // From CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::RunOperation()
     */
    TInt RunOperation();        
    
    
protected:

    // Constructor
    CNcdServerReportOperation(      
        CNcdGeneralManager& aGeneralManager,
        MNcdOperationRemoveHandler& aRemoveHandler, 
        CNcdReportManager& aReportManager,
        MCatalogsSession& aSession );
        
    virtual void ConstructL();
    
        
private:

    CNcdReportManager& iReportManager;

    };

#endif // C_NCD_SERVER_REPORT_OPERATION_H
