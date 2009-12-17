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
* Definition of the Swi::Sis::CCrc
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISCRC_H__
#define __SISCRC_H__

#include "sisfield.h"

namespace Swi
{
namespace Sis
 {

/**
 * This class represents a Crc field, it can be either SisDataCrc or SisControllerCrc. 
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CCrc : public CField
	{
public:

	/**
	 * This creates a new CCrc object and places it on the cleanup stack.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CCrc representing the data read.	 	
	 */
	static CCrc* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CCrc object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CCrc representing the data read.	 	 
	 */
	static CCrc* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	virtual ~CCrc();

public: // Business methods

	/**
	 * Accessor for the actual checksum. 
	 *
	 * @return The checksum data as a TUint16
	 */
	TUint16 Checksum() const;
	
private: // Life cycle methods

	CCrc();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider      An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether to check the field code or take it for granted.
	 *	 
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:
	
	TUint16 iChecksum;	
	};
	
inline TUint16 CCrc::Checksum() const
	{
	return iChecksum;
	}	

 } // namespace Sis

} // namespace Swi



#endif // __SISCRC_H__
