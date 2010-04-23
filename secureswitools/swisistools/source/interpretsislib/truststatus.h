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


#ifndef	TRUSTSTATUS_H
#define	TRUSTSTATUS_H

#include <istream>
#include "serialisable.h"
#include "symbiantypes.h"

class Serialiser;
class Deserialiser;
/** 
* @file TRUSTSTATUS.H
*
* @internalComponent
* @released
*/
class TrustStatus : public Serialisable
	{
public:
	/**
	Constructor
	@param aFileContents The input stream
	*/
	TrustStatus ();
	virtual ~TrustStatus ();

	virtual void Internalize(Deserialiser& des);
	virtual void Externalize(Serialiser& ser);

	TUint64 GetQuaratinedDate()
	{ return iQuaratinedDate; }

	void SetQuaratinedDate(TUint64& aQuaratinedDate)
	{ iQuaratinedDate = aQuaratinedDate; }

	TUint32 GetQuaratined()
	{ return iQuaratined; }

	void SetQuaratined(TUint32 aQuaratined)
	{ iQuaratined = aQuaratined; }

	TUint64 GetLastCheckDate()
	{ return iLastCheckDate; }

	void SetLastCheckDate(TUint64& aLastCheckDate)
	{ iLastCheckDate = aLastCheckDate; }

	TUint64 GetResultDate()
	{ return iResultDate; }

	void SetResultDate(TUint64& aResultDate)
	{ iResultDate = aResultDate; }

	TUint32 GetRevocationStatus()
	{ return iRevocationStatus; }

	void SetRevocationStatus(TUint32 aRevocationStatus)
	{ iRevocationStatus = aRevocationStatus; }

	TUint32 GetValidationStatus()
	{ return iValidationStatus; }

	void SetValidationStatus(TUint32 aValidationStatus)
	{ iValidationStatus = aValidationStatus; }

	/// from swi/sistruststatus.h
	static const TUint32 KOcspNotPerformed; 
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	static const TUint32 KValidationStatusUnknown;
	#endif
	static const TUint32 KValidatedToAnchor;
	static const TUint32 KPackageInRom;

private:
	TUint32 iValidationStatus;
	TUint32 iRevocationStatus;
	TUint64 iResultDate;
	TUint64 iLastCheckDate;
	TUint32 iQuaratined;
	TUint64 iQuaratinedDate;
	
	};

#endif	/* TRUSTSTATUS_H */
