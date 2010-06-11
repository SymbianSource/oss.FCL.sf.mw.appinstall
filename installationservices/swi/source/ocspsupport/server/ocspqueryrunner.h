/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




/**
 @file
 @internalTechnology
*/


#ifndef __OCSPQUERYRUNNER_H__
#define __OCSPQUERYRUNNER_H__

#include <e32base.h>


class RMessagePtr2;
class MOCSPTransport;

namespace Swi
{

/**
 * Class which is used to run the OCSP queries
 */
class COcspQueryRunner : public CActive
	{
public:
	static COcspQueryRunner* NewL(MOCSPTransport* aTransport, const RMessage2& aMessage, TPriority aPriority = CActive::EPriorityStandard);
	static COcspQueryRunner* NewLC(MOCSPTransport* aTransport, const RMessage2& aMessage, TPriority aPriority = CActive::EPriorityStandard);

	/**
	 * Get the response from the OCSP query. Only valid if a request has
	 * been made and compelted with no error.
	 * @return The response data.
	 */
	TPtrC8 GetResponse () const;

	~COcspQueryRunner();
private:
	COcspQueryRunner(MOCSPTransport* aTransport, const RMessage2& aMessage, TPriority aPriority);

	/// Starts the query using the stored message, unmarshalling the arguments needed
	void StartL();

	/// Helper function to send the request
	void SendRequest (const TDesC8 &aUri, const TDesC8 &aRequest, const TInt aTimeout);

	/// Helper function to cancel the request
	void CancelRequest ();

// from CActive
	/// RunL is called when the query has finished
	void RunL();
	void DoCancel();
private:
	// Message which started the query, used to complete with result
	const RMessage2 iMessage;
	
	/**
	 * The OCSP transport to use for the query. We take ownership of this
	 */
	MOCSPTransport* iTransport;
		
	/// The uri for the query.
	HBufC8* iUri;
	
	/// The query request data
	HBufC8* iRequest;
	};
}

#endif //#ifndef __OCSPQUERYRUNNER_H__

