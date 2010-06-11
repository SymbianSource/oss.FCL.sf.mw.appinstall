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
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#include "raw.h"
#include "siswrite.h"

CRaw::CRaw (const CRaw& aInitialiser) :
		iData (NULL),
		iSize (0)
	{
	assert ((aInitialiser.iData == NULL) == (aInitialiser.iSize == 0));
	if (aInitialiser.iSize)
		{
		Alloc (aInitialiser.iSize);
		memcpy (iData, aInitialiser.iData, aInitialiser.iSize);
		}
	}


void CRaw::Alloc (const TFieldSize& aSize)
	{
	assert (iSize <= UINT_MAX);		// you'll need to rewrite CSISFieldRoot::DoTheCrc and stream I/O to take large sizes
	Dispose ();
	if (aSize)
		{
		try
			{
			iData = new TUint8 [aSize];
			}
		catch(std::exception& e)
			{
			SISLogger::Log(L"Error : File Size is too big to process...\n");
			throw CSISException (CSISException::EMemory, "File Size is too big to process...");
			}

		iSize = aSize;
		}
	}


void CRaw::Dispose ()
	{
	delete iData;
	iData = NULL;
	iSize = 0;
	}

	
void CRaw::Read (TSISStream& aFile, const TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType)
	{
	assert (aArrayType == CSISFieldRoot::ESISUndefined);	// typing invalid
	Alloc (aContainerSize);
	if (iSize)
		{
		aFile.read (iData, static_cast <TSISStream::size_type> (aContainerSize));
		}
	}


void CRaw::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	assert (! aIsArrayElement);	// typing invalid
	assert ((iData == NULL) == (iSize == 0));
	if (iSize)
		{
		aFile.write (iData, iSize);
		}
	}


void CRaw::Skip (TSISStream& aFile, const TFieldSize& aContainerSize) const
	{
	assert (aContainerSize <= aFile.MaxBufferSize ());
	if (aContainerSize)
		{
		aFile.seek (static_cast <TSISStream::pos_type> (aContainerSize), std::ios_base::cur);
		}
	}


CSISFieldRoot::TFieldSize CRaw::ByteCount (const bool aInsideArray) const
	{
	return iSize;
	}


void CRaw::SetByteCount (const TFieldSize aSize)
	{
	Alloc (aSize);
	}


void CRaw::Dump (std::ostream& aStream, const int aLevel) const
	{
	aStream << iSize << " bytes:" << std::hex;
	TUint64 size = 20;
	for (int index = 0; index < std::min (iSize, size); index++)
		{
		aStream << " " << static_cast <unsigned int> (iData [index]);
		}
	aStream << std::dec;
	}


void CRaw::CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const
	{
	assert ((iData == NULL) == (iSize == 0));
	if (iSize)
		{
		DoPaddedCrc (aCRC, iData, static_cast <unsigned int> (iSize));
		}
	}


std::string CRaw::Name () const
	{
	return "raw";
	}
