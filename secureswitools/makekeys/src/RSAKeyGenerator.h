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
* Interface for the CRSAKeyGenerator class
*
*/


/**
 @file 
 @internalComponent 
*/

#if !defined(AFX_RSAKEYGENERATOR_H__C7A82751_E706_430F_8974_DA949A194FCC__INCLUDED_)
#define AFX_RSAKEYGENERATOR_H__C7A82751_E706_430F_8974_DA949A194FCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KeyGenerator.h"
#include "openssl/rsa.h"

static void RSAKeyStatus(int aCode, int aArg, void *aCbArg);

class CRSAKeyGenerator : public CKeyGenerator  
	{
	public:
		int Generate();
		CRSAKeyGenerator();
		virtual ~CRSAKeyGenerator();

	};

#endif // !defined(AFX_RSAKEYGENERATOR_H__C7A82751_E706_430F_8974_DA949A194FCC__INCLUDED_)
