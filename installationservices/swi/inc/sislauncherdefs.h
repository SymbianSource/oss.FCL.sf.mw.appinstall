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
* SisLauncher - shared client/server data structure definitions
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef __SISLAUNCHERDEF_H__
#define __SISLAUNCHERDEF_H__

class TAppInUse
	{
public:
	TUid iAppUid;
	TBool iInUse;
	};
#endif