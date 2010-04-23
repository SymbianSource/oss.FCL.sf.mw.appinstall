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
* Description:   IAUpdate client launcher.
*
*/



// INCLUDE FILES
#include <eikstart.h>
#include "iaupdatelauncherapplication.h"
#include "iaupdatedebug.h"


LOCAL_C CApaApplication* NewApplication()
	{
	IAUPDATE_TRACE("[IAUPDATE] NewApplication() begin");
	return new CIAUpdateLauncherApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}

