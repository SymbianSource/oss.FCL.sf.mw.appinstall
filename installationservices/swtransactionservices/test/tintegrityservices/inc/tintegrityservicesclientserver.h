/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Common header for the server and the client.
*
*/


#ifndef __INTEGRITYSERVICESCLIENTSERVER_H__
#define __INTEGRITYSERVICESCLIENTSERVER_H__

#include <e32std.h>
#include "tintegrityservicescommon.h"

/**
 * @test
 * @internalTechnology
 */
class RIntegrityServicesServerSession : public RSessionBase
	{
public:
	/**
	 * Connect to the server, attempt to start it if it is not yet running
	 * @return KErrNone if successful, or an error code
	 */
	IMPORT_C TInt Connect();
	IMPORT_C void AddL(const TDesC& aFileName);
	IMPORT_C void RemoveL(const TDesC& aFileName);
	IMPORT_C void TemporaryL(const TDesC& aFileName);
	IMPORT_C void CommitL();
	IMPORT_C void RollBackL(TBool aAllTransactions);
	IMPORT_C void SetSimulatedFailureL(TPtrC aFailType, TPtrC aFailPosition, TPtrC aFileName);
	IMPORT_C void CreateNewTestFileL(const TDesC& aFileName);
	IMPORT_C void CreateTempTestFileL(const TDesC& aFileName);
	};
#endif // #ifndef __INTEGRITYSERVICESCLIENTSERVER_H__
