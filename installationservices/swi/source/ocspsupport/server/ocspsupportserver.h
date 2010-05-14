/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface for the COcspSupportServer class
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __OCSPSUPPORTSERVER_H__
#define __OCSPSUPPORTSERVER_H__

#include <e32base.h>
#include "GenericTimer.h"

class MOCSPTransport;

namespace Swi
{

class COcspSupportServer : public CPolicyServer, public MGenericTimerClient
	{
public:
	IMPORT_C static COcspSupportServer* NewL(CActive::TPriority aPriority = CActive::EPriorityStandard);
	IMPORT_C static COcspSupportServer* NewLC(CActive::TPriority aPriority = CActive::EPriorityStandard);

	~COcspSupportServer();
private:
	/**
	 * Constructor
	 */
	COcspSupportServer(CActive::TPriority aPriority);

	/**
	 * Creates a new session.
	 */
	CSession2* NewSessionL(const TVersion& aClientVersion, const RMessage2 &aMessage) const;

	/// Second phase constructor
	void ConstructL();

public: // API used only by COcspSupportSession
	void AddSession();
	void DropSession();

	/// Creates the actual transport used by the server to make the request
	MOCSPTransport* CreateTransportL(TUint32& aIap) const;

private: // From MGenericTimerClient
	virtual void HandleTimeout();
	
	/**
	 * The number of sessions that are connected to the server. When this 
	 * number reaches 0, the server will initiate its shutdown.
	 */
	TInt iSessionCount;

	/// This is used to shutdown the server after a delay of two seconds.
	CGenericTimer* iTimer;

	// Variables used by the server policy
	static const TUint iRangeCount=2;
	static const TInt iRanges[iRangeCount];
	static const TUint8 iElementsIndex[iRangeCount];
	static const CPolicyServer::TPolicyElement iPolicyElements[1];
	static const CPolicyServer::TPolicy iPolicy;
	};


void StartServerL();

} // namespace Swi


GLDEF_C TInt E32Main();

#endif // #ifndef __OCSPSUPPORTSERVER_H__
