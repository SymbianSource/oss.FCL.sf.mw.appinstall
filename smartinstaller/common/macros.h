/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*     Macros used in Bootstrap/ADM
*
*
*/

#ifndef __MACROS_H__
#define __MACROS_H__

#ifndef DELETE_IF_NONNULL
#define DELETE_IF_NONNULL(x) \
/*	if (x) */       \
		{           \
		delete x;  \
		x = NULL;   \
		}
#endif

#endif
