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
* Interface for the COcspSupportSession class
*
*/


/**
 @file 
 @internalComponent 
*/
 
#ifndef __OCSPSUPPORTSESSION_H__
#define __OCSPSUPPORTSESSION_H__

#include <e32base.h>

namespace Swi
{

class COcspQueryRunner;
class COcspSupportServer;


/**
 * Session used by the OCSP support Server.
 */
class COcspSupportSession : public CSession2
	{
public:
	static COcspSupportSession* NewL(COcspSupportServer& aServer);
	static COcspSupportSession* NewLC(COcspSupportServer& aServer);

	/// Message passed to the server
	enum TMessage
	{
		// OCSP Messages
		KMessageSendRequest,
		KMessageCancelRequest,
		KMessageGetResponse,
		
		KMaxMessage,
	};

	~COcspSupportSession();	

// from CSession2
	void ServiceL(const RMessage2& aMessage);

private:
	COcspSupportSession(COcspSupportServer& aServer);


	// Command handling functions
	
	/// Handle KMessageSendRequest Message
	void HandleSendRequestL(const RMessage2& aMessage);

	/// Handle KMessageCancelRequest Message
	void HandleCancelRequest(const RMessage2& aMessage);

	/// Handle KMessageSendRequest Message
	void HandleGetResponseL(const RMessage2& aMessage);

	/// Cancel the outstanding request associated with this session.
	void CancelRequest();
	
	/// The class used to run the OCSP queries
	COcspQueryRunner* iOcspQueryRunner;
	
	/// The server which this session is associated with
	COcspSupportServer& iServer;
	
	// The IAP selected to perform the OCSP request
	TUint32 iIap;
	};

} // namespace Swi

#endif // #ifndef __OCSPSUPPORTSESSION_H__
