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
*     Declaration of Bootstrap constants.
*
*
*/

#ifndef __BOOTSTRAPCONST_H__
#define __BOOTSTRAPCONST_H__

#include <e32base.h>

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidBootstrapApp = { 0x2002CCCD };

// ADM Uid
const TUid KUidAdm = { 0x2002CCCE };

// Max no of connection attempts to Installer
const TInt KMaxInstallerConnectionAttempts = 6;

// Max no of download reattempts
const TInt KMaxDlConnectionAttempts = 6;
#endif // __BOOTSTRAPCONST_H__

// End of File
