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
* Common header for the server and the client.
*
*/


#ifndef __OCSPCLIENTSERVER_H__
#define __OCSPCLIENTSERVER_H__

#include <e32std.h>

namespace Swi
{
  namespace Test
  {
    
    _LIT(KOcspServerName, "!tsisocsp");
    _LIT(KOcspServerImage, "tsisocsp"); 
    _LIT(KOcspServer, "Test OCSP Server");
 
    const TUid KOcspServerUid3 = { 0x101F7244 };

/**
 * @test
 * @internalTechnology
 */
enum TOcspServerMessage
	{ 
	  ECheck
	};

/**
 * @test
 * @internalTechnology
 */
enum TOcspServerPanic
	{
	EPanicOcspServerBadDescriptor = 10,
	EPanicOcspServerIllegalFunction
	};

/**
 * @test
 * @internalTechnology
 */
class ROcspServerSession : public RSessionBase
	{
public:
	/**
	 * Connect to the server, attempt to start it if it is not yet running
	 * @return KErrNone if successful, or an error code
	 */
	IMPORT_C TInt Connect();
	IMPORT_C TInt Check();
	};

  } // namespace Test

} // namespace Swi

#endif // #ifndef __CONSOLECLIENTSERVER_H__
