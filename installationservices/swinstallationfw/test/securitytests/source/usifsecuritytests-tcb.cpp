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
 @test
*/

#include <captestframework/aprcaptestutility.h>
#include "sifsecuritycontext.h"

#include <s32file.h>

// Factory function
MCapabilityTestFactory* CapabilityTestFactoryL()
	{
	CDefaultCapabilityTestFactory* factory=new(ELeave) CDefaultCapabilityTestFactory();

	factory->AddTestL(CSifSecurityContextTest::NewL());

	return factory;
	}

EXPORT_C MCapabilityTestFactory* CapabilityTestFactory()
	{
	MCapabilityTestFactory* factory=NULL;
	TRAP_IGNORE(factory = CapabilityTestFactoryL());
	return factory;
	}


