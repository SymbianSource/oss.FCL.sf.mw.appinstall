/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/

#ifndef SCRHELPERCLIENT_H
#define SCRHELPERCLIENT_H

#include <e32std.h>
#include <e32cmn.h>
#include <f32file.h>
#include <scs/scsclient.h>
#include "scrhelpercommon.h"

namespace Usif
	{
	
	class RScrHelper : public RScsClientBase
		{
	public:
		/**
			Default constructor of SCR helper session.
		 */
		IMPORT_C RScrHelper();
		
		/**
			Opens a connection to the SCR Helper server.
			@return KErrNone, if the connection is successful. Otherwise, a system-wide error code.  
		 */
		IMPORT_C TInt Connect();
		
		/**
			Closes the connection with the SCR Helper server.
		 */
		IMPORT_C void Close();
		
		IMPORT_C void RetrieveFileHandlesL(RFile& aScrDatabase, RFile& aScrJournal);
	
	private:
		void GetFileHandleL(Usif::TScrHelperServerMessages aFunction, RFile& aFile);
		};
	
	} // namespace Usif

#endif /* SCRHELPERCLIENT_H */
