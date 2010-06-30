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
*     MNetworkStatusObserver definition
*
*
*/

#ifndef __NETWORKSTATUSOBSERVER_H__
#define __NETWORKSTATUSOBSERVER_H__

#include <rconnmon.h>

class MNetworkStatusObserver
	{
public:
	virtual void HandleNetworkStatusChangedL(const TInt aStatus) = 0;
#ifdef USE_LOGFILE
	virtual void Log(TRefByValue<const TDesC16> aFmt, ...) = 0;
#endif
	};

#endif
