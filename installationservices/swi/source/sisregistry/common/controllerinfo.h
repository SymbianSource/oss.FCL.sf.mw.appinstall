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
* CControllerInfo - a class associating a hash value and a version. 
* It is used to contain the hash value  and the verion of a particular sis controller  
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef __CONTROLLERINFO_H__
#define __CONTROLLERINFO_H__

#include <e32base.h>

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "scrhelperutil.h"
#endif

class RReadStream;
class RWriteStream;

namespace Swi
{
class CHashContainer;

class CControllerInfo : public CBase
	{
public:
	IMPORT_C static CControllerInfo* NewL(TVersion aVersion, 
		const CHashContainer& aHashContainer, TInt aOffset);
	IMPORT_C static CControllerInfo* NewLC(TVersion aVersion, 
		const CHashContainer& aHashContainer, TInt aOffset);

	IMPORT_C static CControllerInfo* NewL(RReadStream& aStream);
	IMPORT_C static CControllerInfo* NewLC(RReadStream& aStream);
	
	IMPORT_C static CControllerInfo* NewL(const CControllerInfo& aControllerInfo);
	IMPORT_C static CControllerInfo* NewLC(const CControllerInfo& aControllerInfo);
	
 	/* This set of contructors effectively deprecated */
 
 	IMPORT_C static CControllerInfo* NewL(TVersion aVersion, const CHashContainer& aHashContainer);
 	IMPORT_C static CControllerInfo* NewLC(TVersion aVersion, const CHashContainer& aHashContainer);
 
	virtual ~CControllerInfo();
	
	/**
	 * Write the object content to a stream 
	 *
	 * @param aStream The stream to write to
	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	
	/**
	 * Read the object content from a stream - initialize from a stream
	 *
	 * @param aStream The stream to read from
	 */
	IMPORT_C void InternalizeL(RReadStream& aStream);
	
	/**
	 * Provides the version of the controller object
	 *
	 * @return The version of the controller object
	 */
	TVersion Version() const;
 	
 	/**
 	 * Provides the offset in the registry entry this controller is stored at
 	 *
 	 *
 	 * @return The offset of the controller in the registry
 	 *
 	 */
 	TInt Offset() const;
 	
	/**
	 * Provides constant reference to the hash container of the controller object
	 *
	 * @return constant reference to the hash container of the controller object
	 */
	const CHashContainer& HashContainer() const;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
public: // Friend Functions
	friend void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CControllerInfo& aControllerInfo, TInt aIndex);
	friend void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CControllerInfo*& aControllerInfo, TInt aIndex);
#endif
	
private:
 	CControllerInfo(TVersion aVersion, TInt aOffset);
	CControllerInfo();
	
	void ConstructL(const CHashContainer& aHashContainer);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(const CControllerInfo& aControllerInfo);
	
private:
	TVersion iVersion;
 	TInt iOffset;	
	CHashContainer* iHashContainer;
	};

inline TVersion CControllerInfo::Version() const
	{
	return iVersion;
	}

inline TInt CControllerInfo::Offset() const
 	{
 	return iOffset;
 	}

inline const CHashContainer& CControllerInfo::HashContainer() const
	{
	return *iHashContainer;
	}

} // namespace
#endif //__CONTROLLERINFO_H__
