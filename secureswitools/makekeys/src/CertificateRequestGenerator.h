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
* Interface for the CCertificateRequestGenerator class
*
*/


/**
 @file 
 @internalComponent 
*/

#if !defined(AFX_CERTIFICATEREQUESTGENERATOR_H__1F570325_DFDF_4630_8A01_F04030BACCF1__INCLUDED_)
#define AFX_CERTIFICATEREQUESTGENERATOR_H__1F570325_DFDF_4630_8A01_F04030BACCF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CertificateGenerator.h"

class CCertificateRequestGenerator : public CCertificateGenerator  
{
public:
	int Generate();
	void SetRequestName(_TCHAR* aStr);
	CCertificateRequestGenerator();
	CCertificateRequestGenerator(_TCHAR* aDnPtr);
	virtual ~CCertificateRequestGenerator();

private:
	_TCHAR m_RequestFile[MAXLEN];
};

#endif // !defined(AFX_CERTIFICATEREQUESTGENERATOR_H__1F570325_DFDF_4630_8A01_F04030BACCF1__INCLUDED_)
