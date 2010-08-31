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


#ifndef __TRANSACTIOWRAPPER_H__
#define __TRANSACTIOWRAPPER_H__

#include <e32def.h>
#include <e32base.h>
#include "integrityservices.h"
#include <usif/sts/stsdefs.h>

namespace Usif
{
	class CReferenceCountedTransactionWrapper : public CBase
	{
	public:
		static CReferenceCountedTransactionWrapper* NewLC(TStsTransactionId aTransactionID);
		~CReferenceCountedTransactionWrapper();
		CIntegrityServices* Attach();
		TInt Detach();
		TStsTransactionId TransactionId() const {return iTrPtr->TransactionId();}
		TBool IsCompleted(){return iCompleted;}
		void SetCompleted(){iCompleted=ETrue;}
	private:
		CReferenceCountedTransactionWrapper();
		void ConstructL(TStsTransactionId aTransactionID);
		
	private:
		TInt iRefCount;
		CIntegrityServices* iTrPtr;
		TBool iCompleted;
	};

}//namespace Usif

#endif
