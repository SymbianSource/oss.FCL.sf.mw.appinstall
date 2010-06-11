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
#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER

#include <iostream>

#include "sisif.h"
#include "siselseif.h"


void CSISElseIf::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	aStream << L"ELSEIF ";
	iExpression.AddPackageEntry(aStream, aVerbose);
	aStream << std::endl;
	iInstallBlock.AddPackageEntry(aStream, aVerbose);
	}

