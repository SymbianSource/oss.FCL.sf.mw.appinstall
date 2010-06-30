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
*     Functions to read from the config file.
*
*
*/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <e32def.h>
#include <f32file.h>

#include "globals.h"

// Configuration file containing the URL for the server. Stored in private directory.
_LIT(KConfigFile, "config.ini");
_LIT(KCfgTagUrl, "Url=");

// Resume file fields
_LIT(KRFieldWrapperDrive, "WrapperDrive=");
_LIT(KRFieldBootstrap, "BootstrapVersion=");
_LIT(KRFieldAppFileName, "AppFile=");
_LIT(KRFieldDepFileName, "DepFile=");
_LIT(KRFieldRootUid, "RootUid=");
_LIT(KRFieldDepPkgs, "DepPkgs=");
_LIT(KRFieldDepPkgUid, "DepPkgUid=");
_LIT(KRFieldDepPkgStatus, "DepPkgStatus=");
_LIT(KRFieldWrapperPkgUid, "WrapperPkgUid=");
_LIT(KRFieldAppName, "AppName=");

TInt WriteIntToConfigFile( RFile& aFile, const TDesC& aTag, const TUint32 aValue );
TInt WriteToConfigFile( RFile& aFile, const TDesC& aTag, const TDesC& aValue );
TInt EnsureNewLineAtEnd( RFile& aFile );
TInt ReadConfigFile( RFile& aFile, TDes& aBuffer, RArray< TPtrC >& aLineBuffer );
HBufC* ReadConfigFile( RFs& aFs, const TDesC& aFullPath, RArray< TPtrC >& aLineBuffer, TInt& aError );
TPtrC GetConfigValue( const TDesC& aTag, const RArray< TPtrC >& aLineBuffer, TInt& aError );
TInt CompareVersions( TVersion& version1,TVersion& version2 );
TBool SetVersion( const TDesC8& aVersionPtr, TVersion& aVer );

#endif
