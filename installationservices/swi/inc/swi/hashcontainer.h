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
* Definition of the CHashContainer. 
* It serves as a container combining a hash value and a hash algorithm
* and can in/externalise the contents from/to a stream
*
*/


/**
 @file 
 @released
 @publishedPartner
*/

#ifndef __SISHASHCONTAINER_H__
#define __SISHASHCONTAINER_H__

#include <hash.h>
#include <e32base.h>
#include <hash.h>

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/scr/scr.h>
#endif

class RReadStream;
class RWriteStream;

namespace Swi
  {
  class CHashContainer;
  
	namespace Sis
	  {
		class CHash;
	  }
	  
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Forward declare the friend functions.
	namespace ScrHelperUtil
		{
		void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CHashContainer& aHashContainer, TInt aIndex);
		void ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CHashContainer*& aHashContainer, TInt aIndex);		
		void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CHashContainer& aHashContainer, const TDesC& aFileName, TBool aWildcardedFile, TInt aIndex);
		void ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CHashContainer*& aHashContainer, const TDesC& aFileName, TBool aWildcardedFile, TInt aIndex);
		}
#endif

/**
 * Serves as a container combining a hash value and a hash algorithm
 *
 * @publishedPartner
 * @released
 */	
class CHashContainer : public CBase
	{
public:

	/**
	 * Creates a new CHashContainer object 
	 *
	 * @param aHash  a Sis::CHash object to obtain the hash algorithm and data 
	 *
	 * @return An instance of CHashContainer object 	 	 	 	 	 
	 */
	IMPORT_C static CHashContainer* NewL(const Sis::CHash& aHash);

	/**
	 * Creates a new CHashContainer object.
	 *
	 * @param aHash  a Sis::CHash object to obtain the hash algorithm and data 
	 *
	 * @return An instance of CHashContainer object on the cleanup stack 	 	 	 	 	 
	 */
	IMPORT_C static CHashContainer* NewLC(const Sis::CHash& aHash);

	/**
	 * Creates a new CHashContainer object.
	 *
	 * @param aAlgorithm  An instance of a CMessageDigest::THashId to specify the encoding algorithm
	 * @param aData       A descriptor of TDesC8 type containing the data
	 *
	 * @return An instance of CHashContainer 	 	 	 	 	 
	 */
	IMPORT_C static CHashContainer* NewL(const CMessageDigest::THashId aAlgorithm, const TDesC8& aData);

	/**
	 * Creates a new CHashContainer object.
	 *
	 * @param aAlgorithm  an instance of a CMessageDigest::THashId to specify the encoding algorithm
	 * @param aData       A descriptor of TDesC8 type containing the data
	 *
	 * @return An instance of CHashContainer object on the cleanup stack 	 	 	 	 	 
	 */
	IMPORT_C static CHashContainer* NewLC(const CMessageDigest::THashId aAlgorithm, const TDesC8& aData);

	/**
	 * Creates a new CHashContainer object.
	 *
	 * @param aStream  RReadStream to read the entity from.
	 *
	 * @return An instance of CHashContainer representing the data read.	 	 	 	 	 
	 */
	IMPORT_C static CHashContainer* NewL(RReadStream& aStream);

	/**
	 * Creates a new CHashContainer object.
	 *
	 * @param aStream   RReadStream to read the entity from.
	 *
	 * @return An instance of CHashContainer on the cleanup stack	 	 	 	 	 
	 */
	IMPORT_C static CHashContainer* NewLC(RReadStream& aStream);
	
	/**
	 * Destructor of CHashContainer.
	 */
	
	IMPORT_C virtual ~CHashContainer();
	
	/**
	 * Reads a CHashContainer object contents from a stream.
	 *
	 * @param aStream   RReadStream to read the entity from.
	 *	 	 	 	 	 
	 */
	IMPORT_C void InternalizeL(RReadStream& aStream);
	
	/**
	 * Streams CHashContainer object contents to a stream.
	 *
	 * @param aStream    RWriteStream to write the entity contents to.
	 *
	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	
    // Accessors

	/**
	 * This method returns the algorithm used to produce the hash value.
	 *
	 * @return the algorithm used to produce the hash value of type CMessageDigest::THashId.	 	 	 	 	 
	 */
	inline CMessageDigest::THashId Algorithm() const;

	/**
	 * This method returns the actual hash value.
	 *
	 * @return the hash value as const raw data.	 	 	 	 	 
	 */
	inline const TDesC8& Data() const;

	/**
	 * @internalTechnology
	 * This method returns serialized size of the object
	 */
	inline TUint32 SerializedSize() const;
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
public: // Friend Functions
	friend void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CHashContainer*& aHashContainer, TInt aIndex);
	
	friend void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CHashContainer*& aHashContainer, const TDesC& aFileName, TBool aWildcardedFile, TInt aIndex);
#endif

private:
	
	/* Constructor
	 *
	 */
	CHashContainer();
	
	/* Constructor
	 *
	 * @param aAlgorithm  An instance of a CMessageDigest::THashId to specify the encoding algorithm
	 */
	CHashContainer(const CMessageDigest::THashId aAlgorithm);

	/**
	 * The second-phase constructor.
	 *
	 * @param aHash 	the Sis::CHash to copy
	 */
	void ConstructL(const Sis::CHash& aHash);

	/**
	 * The second-phase constructor.
	 *
	 * @param aData       A descriptor of TDesC8 type containing the data
	 */
	void ConstructL(const TDesC8& aData);

	/**
	 * The second-phase constructor.
	 *
	 * @param aStream		 An instance of a RReadStream to read the entity from.
	 */
	void ConstructL(RReadStream& aStream);


private:

	CMessageDigest::THashId iAlgorithm;
	
	HBufC8* iData;
	};

// inline functions from CHashContainer
CMessageDigest::THashId CHashContainer::Algorithm() const
	{
	return iAlgorithm;
	}

const TDesC8& CHashContainer::Data() const
	{
	return *iData;	
	}

TUint32 CHashContainer::SerializedSize() const
	{
	return iData->Size() + sizeof(iAlgorithm);
	}

} //namespace Swi



#endif //__SISHASHCONTAINER_H__
