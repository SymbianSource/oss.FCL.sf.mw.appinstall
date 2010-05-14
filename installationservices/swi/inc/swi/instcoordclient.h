/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Definition of the CInstCoordClient class which acts as the client-side
* mediator between the CSISHelper and the CServerSISInstaller.
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef INSTCOORDCLIENT_H
#define INSTCOORDCLIENT_H

#include <e32base.h>
#include "InstallSession.h"
#include <swi\MSisUihandlers.h>

class CSISInstallationResult;
class CSISRemovalResult;

namespace Swi
{

/**
 * The CInstCoordClient class is the client side mediator between the
 * CSISHelper class (on the client side) and the CServerSISInstaller class
 * (on the server).
 *
 * A request from the SISHelper to start a software installation process is
 * received by this class. It in turn will:
 *
 * 1. Send the message to the server
 * 2. Wait for the message to complete - the message will complete with
 *    a server side request
 *
 * The message may be completed in one of three ways:
 * 1. Overflow - This indicates that the server wants to make a request but
 *    it needs more space for the input buffer to marshall parameters. 
 *    The space required is embedded in the response. The client will
 *    respond with an EContinueInstall with a sufficiently sized buffer
 *
 * 2. EndInstall - This indicates that installation is complete or terminated
 *    and the result is received. This in turn will complete CInstCoordClient's
 *    client and will set their result.
 *
 * 3. A request from the server for data is received. This will call invoke the
 *    applicable method implemented in the client-supplied 
 *    MInstallationCoordinator object. The data will be marshalled back to the
 *    server
 *
 * @internalComponent
 * @released
 */
class CInstCoordClient : private CActive
	{
public:
	IMPORT_C static CInstCoordClient* NewL(MUiHandler& aInstaller);
	virtual ~CInstCoordClient();

	/**
	 * Start the installation process.
	 * @param The SIS filename to install
	 * @param A reference to where the result is to be stored
	 * @param Request status which is completed when the installation is
	 *        is finished
	 */
	IMPORT_C void Install(const TFileName& aSISFileName, 
			      CSISInstallationResult& aInstallationResult,
			      TRequestStatus& aStatus);

	/**
	 * Start the removal process.
	 * @param The UID to uninstall
	 * @param A reference to where the result is to be stored
	 * @param Request status which is completed when the installation is
	 *        is finished
	 */
	IMPORT_C void Remove(const TUid& aUid,
			     CSISRemovalResult& aRemovalResult,
			     TRequestStatus& aStatus);

	/** Cancel the running operation */
	IMPORT_C void CancelOperation();

private:	// from CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

private: // constructors
	CInstCoordClient(MUiHandler& aInstallerUI);
	void ConstructL();

	// Allocates the input buffer and sets the pointer properly
	void AllocInputBufL(TInt aSize);

	// Handles the finished event, which will simply unmarshall the result
	// and then complete the client. No response is necessary to the server.
	void HandleFinishedOperation();

	// If the status returned from the reply is KErrOverflow, we need to
	// allocate larger allocated buffer for the server
	void HandleOverflowL();

	// Handle all other messages
	void HandleGeneralCallL();

private:
	MUiHandler& iInstaller;
	RInstallSession iSession;

	CSISInstallationResult* iInstallResult; // client's install result placeholder
	CSISRemovalResult* iRemoveResult;
	TRequestStatus* iClientStatus;

	// The following parameters are those used by the server
	TInt iServerRequest; // received request from the server
	TPckg<TInt> iReqBuffer; // packaged version of above parameter
	HBufC8* iServerBuffer; // buffer containing parameters from server
	TPtr8 iServerBufPtr;
	HBufC8* iClientBuffer; // buffer containing parameters from client
	};

} // namespace Swi

#endif

