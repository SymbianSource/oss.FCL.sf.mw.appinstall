/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface definition for Test ECOM plug-in.
*
*/


/**
 @file
 @test
*/

#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <ecom/implementationproxy.h>
#include <e32base.h>

NONSHARABLE_CLASS(CTestPlugin) : public CBase
	{
public:
	static CTestPlugin* NewL();
	~CTestPlugin();	
 		
protected:
		

		
private:
	CTestPlugin();
		
private:

	};

#endif
