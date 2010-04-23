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
* RSisRevocationSession - external client revocation session interface
* The defined fuctionality is used by clients to access the sisregistry to 
* request the revocation status of a package.
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef SISREVOCATIONENTRY_H
#define SISREVOCATIONENTRY_H

#include <swi/sisregistrywritableentry.h>

namespace Swi
{
class RSisTrustStatus;

class RSisRevocationEntry : public RSisRegistryWritableEntry
	{
public:
	
	/**
	 * Synchronous call for a revocation status check for the package 
	 * identified by the UID. The trust status will be updated as a result of
	 * this call. Use RSisRegistryEntry::TrustStatusL to view the result.
	 *
	 * @param  aUid The UID. 
 	 * @param aOcspUri The Ocsp resource to be used to check the revocation
 	 * status
	 *
	 * @capability WriteUserData
	 * @capability NetworkServices
	 *
	 */
	IMPORT_C void CheckRevocationStatusL(const TDesC8& aOcspUri);

	/*	 
	 * Asynchronous call for a revocation status check for the package 
	 * identified by the UID. The trust status will be updated as a result of
	 * this call. Use RSisRegistryEntry::TrustStatusL to view the result.
	 * 
 	 * @param aUid The UID
 	 * @param aOcspUri The Ocsp resource to be used to check the revocation
 	 * status
	 * @param aRequestStatus The completion status of a request made to a service provider.
	 *
	 * @capability WriteUserData
	 * @capability NetworkServices
	 */
	IMPORT_C void CheckRevocationStatus(const TDesC8& aOcspUri,
	                                    TRequestStatus& aStatus);
	                                     
	 /**
 	 * Cancel current asnchronous RevocationStatus request
 	 *
	 * @capability WriteUserData
	 * @capability NetworkServices
  	 */
  	IMPORT_C void CancelRevocationStatusL();
	};

} // namespace

#endif
