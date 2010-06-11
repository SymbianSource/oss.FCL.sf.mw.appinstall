/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


namespace Swi
{

// CUserSelections inline functions

inline TLanguage CUserSelections::Language() const
	{
	return iLanguage;
	}

inline TChar CUserSelections::Drive() const
	{
	return iDrive;
	}

inline const RArray<TBool>& CUserSelections::Options() const
	{
	return iOptions;
	}

inline void CUserSelections::SetLanguage(TLanguage aLanguage)
	{
	iLanguage=aLanguage;
	}

inline void CUserSelections::SetDrive(TChar aDrive)
	{
	iDrive=aDrive;
	}

inline CUserSelections::~CUserSelections()
	{
	iOptions.Reset();
	}
	
} // namespace Swi
