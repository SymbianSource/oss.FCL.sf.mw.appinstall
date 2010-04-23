/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SISREGISTRYHELPERCLIENT_H
#define SISREGISTRYHELPERCLIENT_H

#include <e32std.h>
#include <e32cmn.h>
#include <f32file.h>
#include <scs/scsclient.h>
#include "sisregistryhelperservercommon.h"

namespace Swi
	{
	
	class RSisRegistryHelper : public RScsClientBase
		{
	public:
		/**
			Default constructor of SISRegistry helper session.
		 */
		IMPORT_C RSisRegistryHelper();
		
		/**
			Opens a connection to the SISRegistry Helper server.
			@return KErrNone, if the connection is successful. Otherwise, a system-wide error code.  
		 */
		IMPORT_C TInt Connect();
		
		/**
			Closes the connection with the SISRegistry Helper server.
		 */
		IMPORT_C void Close();
		
		IMPORT_C void GetEquivalentLanguagesL(TLanguage aLangId,RArray<TLanguage>& aEquivLangs);
	
	private:
		
		};
	
	} // namespace Swi

#endif /* SISREGISTRYHELPERCLIENT_H */
