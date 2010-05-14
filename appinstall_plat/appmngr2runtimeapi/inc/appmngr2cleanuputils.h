/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Utility definitions to push pointer arrays in cleanup stack
*
*/


#ifndef C_APPMNGR2CLEANUPUTILS_H
#define C_APPMNGR2CLEANUPUTILS_H

template <class T>
class CleanupResetAndDestroy
	{
public:
	inline static void PushL( T& aRef );
private:
	static void ResetAndDestroy( TAny *aPtr );
	};

template <class T>
inline void CleanupResetAndDestroyPushL( T& aRef );

#include "appmngr2cleanuputils.inl"

#endif  // C_APPMNGR2CLEANUPUTILS_H
