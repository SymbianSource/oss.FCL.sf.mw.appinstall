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
* CSisRegistryDependency declaration
*
*/


/**
 @file 
 @released
 @publishedPartner
*/

#ifndef __SISREGISTRYDEPENDENCY_H__
#define __SISREGISTRYDEPENDENCY_H__

#include <e32base.h>

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/scr/scr.h>
#include "installtypes.h"
#endif

namespace Swi
{
class CSisRegistryDependency;

namespace Sis
	{
	class CDependency;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
// Forward declare friend functions
namespace ScrHelperUtil
	{
	void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aCompUid, const TDesC& aCompName, Sis::TInstallType aInstallType, CSisRegistryDependency& aRegistryDependency);
	void ReadFromScrL(Usif::CVersionedComponentId& aVersionedSupplierCompId, CSisRegistryDependency*& aRegistryDependency);
	}
#endif

class CSisRegistryDependency : public CBase
	{
public:
	/**
	 * Creates a new CSisRegistryDependency object based on a Sis::CDependency
	 */
	IMPORT_C static CSisRegistryDependency* NewL(const Sis::CDependency& aDependency);

	/**
	 * Creates a new CSisRegistryDependency object on the cleanup stack.
	 */
	IMPORT_C static CSisRegistryDependency* NewLC(const Sis::CDependency& aDependency);

	/**
	 * Creates a new CSisRegistryFileDescription from a stream
	 */
	IMPORT_C static CSisRegistryDependency* NewL(RReadStream& aStream);

	/**
	 * Creates a new CSisRegistryFileDescription from a stream on the cleanup stack
	 */
	IMPORT_C static CSisRegistryDependency* NewLC(RReadStream& aStream);
	
	/**
	 * Creates a new CSisRegistryFileDescription from a CSisRegistryDependency object
	 */
	IMPORT_C static CSisRegistryDependency* NewL(const CSisRegistryDependency& aDependency);

	/**
	 * Creates a new CSisRegistryFileDescription from a CSisRegistryDependency object and places the created object on the cleanup stack
	 */
	IMPORT_C static CSisRegistryDependency* NewLC(const CSisRegistryDependency& aDependency);
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Creates a new CSisRegistryFileDescription from a CSisRegistryDependency object
	 */
	IMPORT_C static CSisRegistryDependency* NewL(const TUid aUid, const TVersion aFromVersion, const TVersion aToVersion);

	/**
	 * Creates a new CSisRegistryFileDescription from a CSisRegistryDependency object and places the created object on the cleanup stack
	 */
	IMPORT_C static CSisRegistryDependency* NewLC(const TUid aUid, const TVersion aFromVersion, const TVersion aToVersion);
#endif

	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	TUid Uid() const;
	
	TVersion FromVersion() const;
	
	TVersion ToVersion() const;

	IMPORT_C TBool IsCompatible(const TVersion& aVersion) const;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
public: // Friend Functions
	friend void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aCompUid, const TDesC& aCompName, Sis::TInstallType aInstallType, CSisRegistryDependency& aRegistryDependency);
	friend void ScrHelperUtil::ReadFromScrL(Usif::CVersionedComponentId& aVersionedSupplierCompId, CSisRegistryDependency*& aRegistryDependency);
#endif
	
private:
	/**
	 * The constructor.
	 */
	CSisRegistryDependency();

	/**
	 * The second-phase constructor.
	 * @param aDependency The Sis::CDependency dependency object reference argument.
	 */
	void ConstructL(const Sis::CDependency& aDependency);
	
	/**
	 * The second-phase constructor.
	 * @param aDependency The CSisRegistryDependency to read the file description from.
	 */	
	void ConstructL(const CSisRegistryDependency& aDependency);

	/**
	 * The second-phase constructor.
	 * @param aStream The stream to read the file description from.
	 */
	void ConstructL(RReadStream& aStream);
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
 	void ConstructL(const TUid aUid, const TVersion aFromVersion, const TVersion aToVersion);
#endif

private:
	TUid iUid;
	TVersion iFromVersion;
	TVersion iToVersion;	
	};

// inline functions from CSisRegistryDependency

inline TUid CSisRegistryDependency::Uid() const
	{
	return iUid;
	}

inline TVersion CSisRegistryDependency::FromVersion() const
	{
	return iFromVersion;
	}

inline TVersion CSisRegistryDependency::ToVersion() const
	{
	return iToVersion;
	}
	
} //namespace Swi
#endif

