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
* CSisRegistryFileDescription interface
* This class is based on Sis::CFileDescription class but allows 
* also to be internalised/externalised
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef __SISREGISTRYFILEDESCRIPTION_H__
#define __SISREGISTRYFILEDESCRIPTION_H__

#include <hash.h>
#include <e32base.h>
#include "sisfieldtypes.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "scrhelperutil.h"
#endif

class RReadStream;
class RWriteStream;

namespace Swi
{

namespace Sis
	{
	class CFileDescription;
	}
	
class CHashContainer;
	
class CSisRegistryFileDescription : public CBase
	{
public:
	/**
	 * Creates a new CSisRegistryFileDescription object based on a Sis::CFileDescription object.
	 *
	 * @param aFileDescription the file description object used to initialise the new object
	 * @param aDrive the user selected drive for this installation
	 * @return the new object 
	 */
	IMPORT_C static CSisRegistryFileDescription* NewL(const Sis::CFileDescription& aFileDescription, TChar aDrive, TBool aIsStub = EFalse);

	/**
	 * Creates a new CSisRegistryFileDescription based on a Sis::CFileDescription  object.
	 *
	 * @param aFileDescription the file description object used to initialise the new object
	 * @param aDrive the user selected drive for this installation
	 * @return the new object on the cleanup stack
	 */
	IMPORT_C static CSisRegistryFileDescription* NewLC(const Sis::CFileDescription& aFileDescription, TChar aDrive, TBool aIsStub = EFalse);

	/**
	 * Creates a copy of an existing CSisRegistryFileDescription.
	 *
	 * @param aFileDescription the file description to base this object on
	 * @return the new object 
	 */
	IMPORT_C static CSisRegistryFileDescription* NewL(const CSisRegistryFileDescription& aFileDescription);

	/**
	 * This method creates a copy of an existing CSisRegistryFileDescription.
	 *
	 * @param aFileDescription the file description to base this object on
	 * @return the new object on the cleanup stack
	 */
	IMPORT_C static CSisRegistryFileDescription* NewLC(const CSisRegistryFileDescription& aFileDescription);

	/**
	 * This method creates a new CSisRegistryFileDescription from a stream
	 *
	 * @param aStream the stream to read from
	 * @return the new object 
	 */
	IMPORT_C static CSisRegistryFileDescription* NewL(RReadStream& aStream);

	/**
	 * This creates a new CSisRegistryFileDescription from a stream
	 *
	 * @param aStream the stream to read from
	 * @return the new object on the cleanup stack
	 */
	IMPORT_C static CSisRegistryFileDescription* NewLC(RReadStream& aStream);
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * This method creates a new CSisRegistryFileDescription from a stream
	 *	 
	 * @return the new object 
	 */
	IMPORT_C static CSisRegistryFileDescription* NewL();

	/**
	 * This creates a new CSisRegistryFileDescription from a stream
	 *	 
	 * @return the new object on the cleanup stack
	 */
	IMPORT_C static CSisRegistryFileDescription* NewLC();
#endif
	
	/** Create a file description based upon supplied parameters. This is used to create
	    a file description for removable media SIS stub files. So they can be removed
	    if an uninstall occurs
	@param aHash The hash of the file
	@param aTarget The location of hte file
	@param aMimeType The mime type of the file
	@param aOperation The operation performed on this file during installation
	@param aOperationOptions The options applied to the Operation
	@param aUncomressedLength Size of the file
	@param aIndex 
	@param aSid
	*/
	IMPORT_C static CSisRegistryFileDescription* NewL(CHashContainer& aHash,
														const TDesC& aTarget,
														const TDesC& aMimeType,
														const Sis::TSISFileOperation aOperation,
														const Sis::TSISFileOperationOptions aOperationOptions,
														const TInt64 aUncompressedLength,
														const TUint32 aIndex,
														const TUid aSid );

	IMPORT_C virtual ~CSisRegistryFileDescription();

	/**
	 * Write the object to a stream 
	 *
	 * @param aStream The stream to write to
	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	/**
	 * Read the object from a stream
	 *
	 * @param aStream the stream to read from
	 */
	IMPORT_C void InternalizeL(RReadStream& aStream);
	
	const TDesC& Target() const;

	const TDesC& MimeType() const;

	const CHashContainer& Hash() const;

	Sis::TSISFileOperation Operation() const;

	Sis::TSISFileOperationOptions OperationOptions() const;

	TInt64 UncompressedLength() const;

	TUint32 Index() const;
	
	TUid Sid() const;
	
	void SetSid(TUid aUid);
	const HBufC8* CapabilitiesData() const;

	
	IMPORT_C TUint32 SerializedSize() const;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
public: // Friend Functions
	friend void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryFileDescription*& aFileDescription, const TDesC& aFileName, TBool aWildcardedFile);
#endif

private:
	/**
	 * Constructor
	 */
	CSisRegistryFileDescription();
	
	/**
	 * The second-phase constructor.
	 *
	 * @param aFileDescription the file description.
	 * @param aDrive the user selected drive for this installation
	 */
	void ConstructL(const Sis::CFileDescription& aFileDescription, TChar aDrive, TBool aIsStub);

	/**
	 * The second-phase constructor.
	 *
	 * @param aFileDescription the file description.
	 */
	void ConstructL(const CSisRegistryFileDescription& aFileDescription);
	
	/**
	 * The second-phase constructor.
	 *
	 * @param aStream the stream to read the file description from.
	 */
	void ConstructL(RReadStream& aStream);
    

	void ConstructL(CHashContainer& aHash,
					const TDesC& aTarget,
					const TDesC& aMimeType,
					const Sis::TSISFileOperation aOperation,
					const Sis::TSISFileOperationOptions aOperationOptions,
					const TInt64 aUncompressedLength,
					const TUint32 aIndex,
					const TUid aSid);
private:
	HBufC* iTarget;
	
	HBufC* iMimeType;

	CHashContainer* iHash;
	
	Sis::TSISFileOperation iOperation;
	
	Sis::TSISFileOperationOptions iOperationOptions;
	
	TInt64 iUncompressedLength;
	
	TUint32 iIndex;
	
	TUid iSid;
	
	HBufC8* iCapabilitiesData;
	};

// inline functions from CSisRegistryFileDescription

inline const TDesC& CSisRegistryFileDescription::Target() const
	{
	return *iTarget;	
	}

inline const TDesC& CSisRegistryFileDescription::MimeType() const
	{
	return *iMimeType;	
	}
	
inline const CHashContainer& CSisRegistryFileDescription::Hash() const
	{
	return *iHash;	
	}

inline Sis::TSISFileOperation CSisRegistryFileDescription::Operation() const
	{
	return iOperation;	
	}

inline Sis::TSISFileOperationOptions CSisRegistryFileDescription::OperationOptions() const
	{
	return iOperationOptions;	
	}

inline TInt64 CSisRegistryFileDescription::UncompressedLength() const
	{
	return iUncompressedLength;
	}

inline TUint32 CSisRegistryFileDescription::Index() const
	{
	return iIndex;
	}

inline TUid CSisRegistryFileDescription::Sid() const
	{
	return iSid;
	}	

inline void CSisRegistryFileDescription::SetSid(TUid aSid)
	{
	iSid = aSid;
	}	
inline const HBufC8* CSisRegistryFileDescription::CapabilitiesData() const
 	{
 	return iCapabilitiesData;
 	}

} //namespace Swi
#endif
