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
* CSisRegistryProperty class declaration 
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef __SISREGISTRYPROPERTY_H__
#define __SISREGISTRYPROPERTY_H__

#include <e32base.h>

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "scrhelperutil.h"
#endif

namespace Swi
{
	namespace Sis
	{
	class CProperty;
	}

class CSisRegistryProperty : public CBase
	{
public:
	/**
	 * This creates a new CProperty object based on a Sis::CProperty
	 */
	IMPORT_C static CSisRegistryProperty* NewL(const Sis::CProperty& aProperty);

	/**
	 * This creates a new CProperty object based on a Sis::CProperty
	 */
	IMPORT_C static CSisRegistryProperty* NewLC(const Sis::CProperty& aProperty);

	/**
	 * This creates a new CProperty object from a stream
	 */
	IMPORT_C static CSisRegistryProperty* NewL(RReadStream& aStream);

	/**
	 * This creates a new CProperty object from a stream
	 */
	IMPORT_C static CSisRegistryProperty* NewLC(RReadStream& aStream);

	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	TInt32 Key() const;
	TInt32 Value() const;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
public: // Friend Functions
	friend void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryProperty& aRegistryProperty, TInt aIndex);
	friend void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryProperty*& aRegistryProperty, TInt aIndex);
#endif

private:

	/**
	 * The second-phase constructor.
	 * @param aProperty The property.
	 */
	void ConstructL(const Sis::CProperty& aProperty);
	
	/**
	 * The second-phase constructor.
	 * @param aStream The stream to read the property from.
	 */
	void ConstructL(RReadStream& aStream);


private:

	TInt32 iKey;
	TInt32 iValue;
	};


// inline functions from CSisregistryProperty

inline TInt32 CSisRegistryProperty::Key() const
	{
	return iKey;		
	}

inline TInt32 CSisRegistryProperty::Value() const
	{
	return iValue;		
	}
	
} //namespace Swi
#endif
