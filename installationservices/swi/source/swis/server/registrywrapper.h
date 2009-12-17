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
* The file contains the wrapper class to access the software installer registry
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#ifndef __REGISTRYWRAPPER_H__
#define __REGISTRYWRAPPER_H__

#include <e32base.h>
#include "sisregistrywritablesession.h"

namespace Swi
{

/* Wrapper class to ensure that all multiple mutable SWI operations (for eg. multiple 
 * uninstalls during embedded processing) are carried out in the same transaction
 */
class CRegistryWrapper : public CBase
	{
public:
	~CRegistryWrapper();

	static CRegistryWrapper* NewL();
	void StartMutableOperationsL();
	void CommitMutableOperationsL();
	void RollbackMutableOperationsL();
	inline RSisRegistryWritableSession& RegistrySession();

private:
	CRegistryWrapper() {};
	void ConstructL();

private:
	// Handle to the writable registry
	RSisRegistryWritableSession iRegistrySession;
	TBool iMutableOperationInProgress;
	};

RSisRegistryWritableSession& CRegistryWrapper::RegistrySession()
	{
	return iRegistrySession;
	}

}
#endif // __REGISTRYWRAPPER_H__
