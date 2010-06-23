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




/**
 @file
 @internalComponent 
*/

#ifndef __UISUPPORT_H__
#define __UISUPPORT_H__

#include <e32std.h>

namespace Swi
{
	
/// The name of the UI Support Server
_LIT(KUissServerName,"!UISUPPORTSERVER");

/// The current version of the UI Support Server
const TInt KUissServerVersionMajor=0;
const TInt KUissServerVersionMinor=1;
const TInt KUissServerVersionBuild=0;

} // namespace Swi

#endif //#ifndef __UISUPPORT_H__ 
