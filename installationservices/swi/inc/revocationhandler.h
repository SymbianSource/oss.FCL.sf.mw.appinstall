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
*
*/


/**
 @file
 @released
 @internalTechnology
*/

#ifndef __REVOCATIONHANDLER_H__
#define __REVOCATIONHANDLER_H__

#include <ocsp.h>
#include <e32base.h>

class CPKIXCertChain;
class MCTCertStore;
class COCSPParameters;
class COCSPClient;

namespace Swi
{

/**
 * Security Manager helper class to handle certificate revocation checking.
 * This class <b>requires TCB capabilities</b> to successfully accomplish its task,
 * Indeed, the OCSP support server calls are policed on TCB.
 *
 */
class CRevocationHandler : public CActive
	{
public:

	/**
	 * Creates a new revocation handler with the given backend certstore
	 *
	 * @param aCertStore The backend certstore used by this handler
	 *
	 * @return A revocation handler.
	 */
	IMPORT_C static CRevocationHandler* NewL(MCTCertStore& aCertStore);

	IMPORT_C ~CRevocationHandler();

	/**
	 * Set default server URI - calling this is optional
	 */
	IMPORT_C void SetDefaultURIL(const TDesC8& aURI);

	/**
	* Set the retry count - calling this is optional
	*
	* @param aRetryCount		The retry count. Default is 1 (no retry)
	*/
	IMPORT_C void SetRetryCount(const TUint aRetryCount);

	/**
	* Set the response timeout - calling this is optional
	*
	* @param aTimeout		The response timeout in milliseconds. Default is -1 (no timeout)
	*/
	IMPORT_C void SetTimeout(const TInt aTimeout);

	/**
	 * Executes the OCSP query
	 *
	 * @param aCertChainList A list of certificate chains to be checked
	 * @param aStatus The request status to be completed upon termination
	 * @param aIap The internet access point to use. May be zero, in which case the user will be asked. Set to the IAP used on completion.
	 */
	IMPORT_C void SendRequestL(RPointerArray<CPKIXCertChainBase>& aCertChainList, TUint32& aIap, TRequestStatus& aStatus);

	// Only valid to call these methods after handler's OCSPComplete() method
	// has been called

	/**
	 * Get OCSP error code if something went wrong
	 *
	 * @return An error code detailing the problem
	 */
	IMPORT_C TInt Error() const;

	/**
	 * Get summary OCSP result
	 *
	 * @return A summary of the OCSP results
	 */
	IMPORT_C OCSP::TResult SummaryResult() const;

	/**
	 * Get the number of transactions made
	 * 
	 * @return The number of transactions made
	 */
	IMPORT_C TInt TransactionCount(void) const;

	/**
	 * Get the outcome for an individual transaction
	 *
	 * @param aIndex The index of the transaction we are interested in
	 *
	 * @return The outcome of the given transaction
	 */
	IMPORT_C const TOCSPOutcome& Outcome(TInt aIndex) const;

private: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	CRevocationHandler(MCTCertStore& aCertStore);
	void ConstructL();

private:

	/**
	 * The list of certificate chains for which OCSP checks are required.
	 * The class <b> does not </b> own this.
	 */
	RPointerArray<CPKIXCertChainBase> iCertChainList; 

	/**
	 * The client request status. We shall complete this when done.
	 */
	TRequestStatus* iClientStatus;

	/**
	 * The backend certstore from where certificates are retrieved.
	 */
	MCTCertStore& iCertStore;

	/**
	 * The OCSP parameters used by this handler, it includes the OCSP transport 
	 * object.
	 */	
	COCSPParameters* iParams;

	/**
	 * The client is the entity to which we delegate the OCSP check request
	 */
	COCSPClient* iClient;

	enum TState
		{
		EChecking,
		EFinished
 		} iState;
	};

} // namespace Swi

#endif // #ifndef __REVOCATIONHANDLER_H__
