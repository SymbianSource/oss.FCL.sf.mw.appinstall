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
	

#ifndef NCDOPERATIONFUNCTIONIDS_H
#define NCDOPERATIONFUNCTIONIDS_H

/**
 * These are used to tell the server side operation what to do.
 */
enum TNcdOperationFunctionId
    {
    ENCDOperationFunctionInit,          // Initialize the operation
    ENCDOperationFunctionStart,         // Start the operation
    ENCDOperationFunctionCancel,        // Cancel the operation
    ENCDOperationFunctionContinue,      // Continue operation
    ENCDOperationFunctionQueryResponse, // Message contains a query response.
    ENCDOperationFunctionPause,         // Pause the operation
    ENCDOperationFunctionResume,        // Resume a paused operation
    ENCDOperationFunctionGetData,       // Used to get data from the operation
    ENCDOperationFunctionSetData,       // Used to set data to the operation
    ENCDOperationFunctionRelease,       // Releases the operation object
    ENcdOperationFunctionOpenFile,      // Opens a file
    ENcdOperationFunctionDeleteFile,    // Deletes a file
    ENCDOperationFunctionReportStart,   // Set start state for server report
    ENCDOperationFunctionReportComplete // Complete server report
    };

/**
 * These are used to notify the proxy of the contents of a completed message.
 */    
enum TNcdOperationMessageCompletionId
    {
    ENCDOperationMessageCompletionInit,        // Message contains initialization info.
    ENCDOperationMessageCompletionProgress,    // Message contains progress info.
    ENCDOperationMessageCompletionQuery,       // Message contains a query.
    ENCDOperationMessageCompletionError,       // An error occured during execution.
    ENCDOperationMessageCompletionComplete,    // Operation complete.
    ENCDOperationMessageCompletionNodesUpdated, // Nodes have been updated
    ENCDOperationMessageCompletionPause,       // Operation paused
    ENCDOperationMessageCompletionResume,      // Operation resumed
    ENCDOperationMessageCompletionPaymentMethods, // Message contains payment methods.
    ENCDOperationMessageCompletionExpirationInfo // Message contains info of expired nodes.
    };
	
	
#endif //  NCDOPERATIONFUNCTIONIDS_H
