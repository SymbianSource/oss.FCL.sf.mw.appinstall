/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __CERTCHAINDATA_H__
#define __CERTCHAINDATA_H__


#include "symbiantypes.h"
#include "structure.h"
#include "sisblob.h"

class CCertChainData : public CStructure <CSISFieldRoot::ESISCertificateChain>

	{
public:
	/**
	 * Default Constructor
	 */
	CCertChainData ();
	/**
	 * Copy constructor
	 */
	CCertChainData (const CCertChainData& aInitialiser);

public:
	/**
	 * Class Name
	 */
	inline virtual std::string Name () const;
	/**
	 * Retrieves certificate data.
	 */
	inline const CSISBlob&		CertificateData() const;
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;

private:
	CSISBlob		iCertificateData;
	};

inline CCertChainData::CCertChainData ()
	{ 
	InsertMember (iCertificateData);
	}


inline CCertChainData::CCertChainData (const CCertChainData& aInitialiser) :
		CStructure <CSISFieldRoot::ESISCertificateChain> (aInitialiser),
		iCertificateData (aInitialiser.iCertificateData)
	{ 
	InsertMember (iCertificateData); 
	}

inline const CSISBlob& CCertChainData::CertificateData() const
	{
	return iCertificateData;
	}

inline std::string CCertChainData::Name () const
	{
	return "Certificate Chain";
	}


inline void CCertChainData::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	if(aVerbose)
		{
		aStream << L"; Certificate chain of " << iCertificateData.Size() << L" bytes" << std::endl;
		}
	}

#endif // __CERTCHAINDATA_H__

