/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements server-side classes which are used to keep observed SWI events.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#include "swiobservedinfo.h"

using namespace Swi;


//
//CObservationData
//		

CObservationData::CObservationData()
/**
	Constructs an empty observation data object.
 */	
 		{
 		//empty
 		}
 		
CObservationData::CObservationData(const TSecureId& aSid, const TUint8 aFileFlag)
/**
	Constructor.
 */	
 	: iSid(aSid), iFileFlag(aFileFlag)
 		{
 		//empty
 		}
 		
 
CObservationData::~CObservationData()
/**
	Destructor.
 */
 	{
 	delete iFileName;
 	}

	
EXPORT_C CObservationData* CObservationData::NewLC()
/**
	Factory function allocates a new, empty CObservationData object which
	is left on the cleanup stack.
	
	@return	The newly created observation record object. 
 */
	{
	CObservationData *od = new(ELeave) CObservationData();
	CleanupStack::PushL(od);
	return od;
	}
	 	

EXPORT_C CObservationData* CObservationData::NewL(const TDesC& aFileName, const TSecureId& aSid, const TUint8 aFileFlag)
/**
	Factory function allocates a new, initialized CObservationData object.
	
	@param aFileName 	The fully qualified file path.
	@param aSid      	Secure Id of the application.
	@param aFileFlag	Various information about the file. @see	TFileFlag
	@return				The newly created observation record object. 
 */
	{
	CObservationData *od = CObservationData::NewLC(aFileName, aSid, aFileFlag);
	CleanupStack::Pop(od);
	return od;
	}
	
EXPORT_C CObservationData* CObservationData::NewLC(const TDesC& aFileName, const TSecureId& aSid, const TUint8 aFileFlag)
/**
	Factory function allocates a new, initialized CObservationData object which
	is left on the cleanup stack.
	
	@see CObservationData::NewL
 */
	{
	CObservationData *od = new(ELeave) CObservationData(aSid, aFileFlag);
	CleanupStack::PushL(od);
	od->ConstructL(aFileName);
	return od;
	}
	
	
void CObservationData::ConstructL(const TDesC& aFileName)
/**
	Second phase constructor.
 */
	{
	iFileName = aFileName.AllocL(); 
	}


EXPORT_C void CObservationData::InternalizeL(RReadStream& aStream)
/**
	Internalises the observation data object.
	
	@param aStream Stream to read from
 */
	{
	iSid =aStream.ReadInt32L();
	iFileName = HBufC::NewL(aStream,KMaxFileName);	
	iFileFlag = aStream.ReadUint8L();
	}



EXPORT_C void CObservationData::ExternalizeL(RWriteStream& aStream) const
/**
	Externalises the observation data object.
	
	@param aStream Stream to write to
 */
	{
	//First write data field type
	aStream.WriteUint8L(KDataField);
	//Secondly write the secure id
	aStream.WriteInt32L(iSid.iId);
	//Third data to write is fully qualified file path
	aStream<<*iFileName;
	//Finally write whether the file has been deleted
	aStream.WriteUint8L(iFileFlag);
	}
	
		
//
//CObservationHeader
//

CObservationHeader::CObservationHeader()
/**
	Constructs an empty observation header object.
 */
 		{
 		//empty
 		}		


CObservationHeader::CObservationHeader(const TUid& aPckgUid, const TPackageType aPckgType, const TOperationType aOpType)
/**
	Constructor.
 */
 	: iPckgUid(aPckgUid), iPckgType(aPckgType), iOpType(aOpType)
 		{
 		//empty
 		}
 		

CObservationHeader::~CObservationHeader()
/**
	Destructor.
 */
 	{
 	//empty
 	}
 	
 	
EXPORT_C CObservationHeader* CObservationHeader::NewLC()
/**
	Factory function allocates a new, initialized CObservationHeader object which
	is left on the cleanup stack.
	
	@see CObservationHeader::NewL
 */
	{
	CObservationHeader *op = new(ELeave) CObservationHeader();
	CleanupStack::PushL(op);
	return op;
	}
	
	 
EXPORT_C CObservationHeader* CObservationHeader::NewL(const TUid& aPckgUid, const TPackageType aPckgType, const TOperationType aOpType)
/**
	Factory function allocates a new, initialized CObservationHeader object.
	
	@param aPckgUid 	The UID of the package.
	@param aPckgType    The type of the package.
	@param aOpType		The type of the SWI operation.
	@return				The newly created observation package object. 
 */
	{
	CObservationHeader *op = CObservationHeader::NewLC(aPckgUid,aPckgType,aOpType);
	CleanupStack::Pop(op);
	return op;
	}

EXPORT_C CObservationHeader* CObservationHeader::NewLC(const TUid& aPckgUid, const TPackageType aPckgType, const TOperationType aOpType)
/**
	Factory function allocates a new, initialized CObservationHeader object which
	is left on the cleanup stack.
	
	@see CObservationHeader::NewL
 */
	{
	CObservationHeader *op = new(ELeave) CObservationHeader(aPckgUid,aPckgType,aOpType);
	CleanupStack::PushL(op);
	return op;
	}
	
		
EXPORT_C void CObservationHeader::InternalizeL(RReadStream& aStream)
/**
	Internalises the observation header object.
	
	@param aStream Stream to read from
 */
 	{
	iPckgUid  = TUid::Uid(aStream.ReadInt32L());
	iOpType   = static_cast<TOperationType>(aStream.ReadInt32L());
	iPckgType = static_cast<TPackageType>(aStream.ReadInt32L()); 	
 	}


EXPORT_C void CObservationHeader::ExternalizeL(RWriteStream& aStream) const
/**
	Externalises the observation header object.
	
	@param aStream Stream to write to
 */
	{
	//First write header field type
	aStream.WriteUint8L(KHeaderField);
	//Second data to write is package UID
	aStream.WriteInt32L(iPckgUid.iUid);
	//Then write the operation type (install, uninstall or restore).
	aStream.WriteInt32L(iOpType);
	//Finally write the package type (SA, SP, PU etc.).
	aStream.WriteInt32L(iPckgType);
	}
 
 
 //
 //CObservationFilter
 //
 
CObservationFilter::CObservationFilter()
/**
	Constructor.
 */
	{
	//empty
	}

CObservationFilter::~CObservationFilter()
/**
	Destructor.
 */
	{
	iFilterList.ResetAndDestroy();
	}
	

EXPORT_C CObservationFilter* CObservationFilter::NewL()
/**
	Factory function allocates a new CObservationFilter object.
	@return he newly created observation filter object.
 */
	{
	CObservationFilter * of = CObservationFilter::NewLC();
	CleanupStack::Pop(of);
	return of;
	}

	
EXPORT_C CObservationFilter* CObservationFilter::NewLC()
/**
	Factory function allocates a new CObservationFilter object which
	is left on the cleanup stack.
	
	@see CObservationFilter::NewL
 */
	{
	CObservationFilter * of = new(ELeave) CObservationFilter();
	CleanupStack::PushL(of);
	return of;
	}
	
	
EXPORT_C void CObservationFilter::AddFilterL(const HBufC* aFilter)
/**
	Add a new filter into the filter object.
	
	@param aFilter The new filter which will be added (e.g. a folder path).
 */
	{
	iFilterList.AppendL(aFilter);
	}


static TBool Compare(const HBufC& f1, const HBufC& f2)
/** 
	Searches for the first occurrence of the second filename sequence within 
	the first filename sequence. 
		
	@return ETrue, if the data sequence can be found; EFalse, otherwise.
 */	
	{
	TInt r = f1.FindF(f2);
	return (r == KErrNotFound) ? EFalse : ETrue;
	}

	
EXPORT_C TBool CObservationFilter::FindF(const HBufC* aValue) const
/**
	Searches the observation filter list for a given value.
	
	@param aValue   The value searched for.
	@return 		Whether a given filter exits in the filter list
 */
	{
	return iFilterList.Find(aValue, TIdentityRelation<HBufC>(Compare));
	}
