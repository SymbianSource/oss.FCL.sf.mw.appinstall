/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines Install Central Repository Constants
*
*/


/**
 @file
 @released
 @internalTechnology
*/

namespace Swi
{


static const TUid KUidInstallationRepository  = {0x2002CFF6}; //Install Central Repository UID

//This key indicates whether OCSP checks to be enabled for Express Signed certificates.
static const TUint32 KCheckOCSPForExpressedSignedPkgKey = 0x00000001;

//This key indicates whether OCSP checks to be enabled for Self Signed certificates.
static const TUint32 KCheckOCSPForSelfSignedPkgKey = 0x00000002;

//This key indicates whether OCSP checks to be enabled for Certified Signed certificates.
static const TUint32 KCheckOCSPForCertifiedSignedPkgKey = 0x00000003;

//This key indicates whether OCSP checks to be enabled for  CertifiedWithVeriSign Signed certificates.
static const TUint32 KCheckOCSPForCertifiedWithVeriSignPkgKey = 0x00000004;

//Install Central Repository Key to allow installation of Self Signed SIS files.
static const TUint32 KAllowSelfSignedInstallKey  = 0x00000005; 
} // namespace Swi
