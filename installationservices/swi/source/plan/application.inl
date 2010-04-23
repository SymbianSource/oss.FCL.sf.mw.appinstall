/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "siscontroller.h"

namespace Swi
{

// CApplication inline functions

inline const CUserSelections& CApplication::UserSelections() const
	{
	return *iUserSelections;
	}

inline CUserSelections& CApplication::UserSelections()
	{
	return *iUserSelections;
	}

inline TInt CApplication::AbsoluteDataIndex() const
	{
   return iAbsoluteDataIndex;
   }

inline void CApplication::SetAbsoluteDataIndex(TInt aDataIndex)
	{
	iAbsoluteDataIndex=aDataIndex;
	}

inline TBool CApplication::ShutdownAllApps() const
	{
   return iShutdown;
   }

inline void CApplication::SetShutdownAllApps(TBool aShutdown)
	{
	iShutdown=aShutdown;
	}

inline TBool CApplication::IsDeletablePreinstalled() const
	{
	return iDeletablePreinstalled;
	}

inline void CApplication::SetDeletablePreinstalled(TBool aDeletablePreinstalled)
	{
	iDeletablePreinstalled = aDeletablePreinstalled;
	}

inline TSisPackageTrust CApplication::Trust() const
	{
	__ASSERT_ALWAYS(iController != NULL, User::Invariant());
	return iController->Trust();	
	}

inline void CApplication::SetStub(TBool aIsStub)
	{
	iIsStub = aIsStub;
	}

inline TBool CApplication::IsStub() const
	{
	return iIsStub;
	}

inline void CApplication::SetDeviceSupportedLanguages(const TBool aIsDeviceSupported) 
	{
	iHasDeviceSupportedLanguages = aIsDeviceSupported;
	}

inline void CApplication::SetLanguageMatch(const TBool aIsLanguageMatch) 
	{
	iHasMatchingLanguages = aIsLanguageMatch;
	}

inline TBool CApplication::IsLanguageMatch() const
	{
	return iHasMatchingLanguages;
	}

inline TBool CApplication::IsDeviceSupportedLanguages() const
	{
	return iHasDeviceSupportedLanguages;
	}

inline const RArray<TInt>&  CApplication::GetDeviceSupportedLanguages(void)
	{
	return iDeviceSupportedLanguages;
	}
} // namespace Swi
