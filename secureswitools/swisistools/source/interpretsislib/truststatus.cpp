/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "truststatus.h"
#include "serialiser.h"
#include "deserialiser.h"

/// from swi/sistruststatus.h
const TUint32 TrustStatus::KOcspNotPerformed = 10; 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
const TUint32 TrustStatus::KValidationStatusUnknown = 0;
#endif
const TUint32 TrustStatus::KValidatedToAnchor = 50;
const TUint32 TrustStatus::KPackageInRom = 60;

TrustStatus::TrustStatus ()
	{
	}

TrustStatus::~TrustStatus()
	{
	}

void TrustStatus::Internalize(Deserialiser& des)
	{
	des >> iValidationStatus
		>> iRevocationStatus
		>> iResultDate
		>> iLastCheckDate
		>> iQuaratined
		>> iQuaratinedDate;
	}

void TrustStatus::Externalize(Serialiser& ser)
	{
	ser << iValidationStatus
		<< iRevocationStatus
		<< iResultDate
		<< iLastCheckDate
		<< iQuaratined
		<< iQuaratinedDate;
	}


