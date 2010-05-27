/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains implementation of common T-classes.
*
*/


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TInstallReq::TInstallReq
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline TInstallReq::TInstallReq()
    : iCharsetId( 0 ),
	iIAP( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TInstallOptions::TInstallOptions
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline TInstallOptions::TInstallOptions()
    : iUpgrade( EPolicyAllowed ),
    iOptionalItems( EPolicyAllowed ),
    iOCSP( EPolicyAllowed ),    
    iIgnoreOCSPWarnings( EPolicyAllowed ),
    iUntrusted( EPolicyNotAllowed ),
    iPackageInfo( EPolicyAllowed ),
    iCapabilities( EPolicyAllowed ),
    iKillApp( EPolicyAllowed ),
    iDownload( EPolicyAllowed ),
    iOverwrite( EPolicyAllowed ),
    iDrive( 'C' ),
    iLang( ELangNone ),
    iUsePhoneLang( ETrue ),
    iUpgradeData( EPolicyAllowed )
    {    
    }

// -----------------------------------------------------------------------------
// TUninstallOptions::TUninstallOptions
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline TUninstallOptions::TUninstallOptions()
    : iKillApp( EPolicyAllowed ),
    iBreakDependency( EPolicyAllowed )
    {
    }
    
// -----------------------------------------------------------------------------
// TInstallReqHeapURL::TInstallReqHeapURL
//
// -----------------------------------------------------------------------------
//
inline TInstallReqHeapURL::TInstallReqHeapURL()
    {
    iSourcePtr = NULL;	
    }
    

//  End of File  
