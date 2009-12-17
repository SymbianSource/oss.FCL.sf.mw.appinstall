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
* Some utility functions needed by SWI
* @internalComponent
*
*/

 

 
#include "swiutility.h"
#include "swiconsoleconstants.h"
 
TLanguage SwiUtility::GetLanguageType(const TDesC&aLanguage)
	{
	// ELangTest shall be used to cancel the installation
	if(1 == aLanguage.Length() && '#' == aLanguage[0])
		{
		return ELangTest;
		}
			
	for(TInt i = 1 ; i < ELangOther; ++i)
		{
		if(0 == aLanguage.CompareF(languageArr[i]))
			{
			return static_cast<TLanguage>(i);
			}
		}
	
	return ELangNone;
	}
 
void SwiUtility::FillArray(RArray<TInt>& aArray, TInt aValue)
	{
	for(TInt i = aArray.Count() - 1; i >= 0; --i)
		{
		aArray[i] = aValue;
		}
	}
