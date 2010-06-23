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
* Interface for the CDSAKeyGenerator class
*
*/


/**
 @file 
 @internalComponent 
*/

#if !defined(AFX_DSAKEYGENERATOR_H__7030227B_AD30_44E7_875F_540D985021EB__INCLUDED_)
#define AFX_DSAKEYGENERATOR_H__7030227B_AD30_44E7_875F_540D985021EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KeyGenerator.h"
#include "openssl/dsa.h"

static void DSAKeyStatus(int aCode, int aArg, void* aCbArg);

class CDSAKeyGenerator : public CKeyGenerator  
{
public:
	int Generate();
	CDSAKeyGenerator();
	virtual ~CDSAKeyGenerator();

};

#endif // !defined(AFX_DSAKEYGENERATOR_H__7030227B_AD30_44E7_875F_540D985021EB__INCLUDED_)
