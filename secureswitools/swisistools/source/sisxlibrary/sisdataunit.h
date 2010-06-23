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
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef __SISDATAUNIT_H__
#define __SISDATAUNIT_H__


#include "structure.h"
#include "sisfiledata.h"
#include "sisarray.h"
#include "siscompressed.h"



class CSISDataUnit : public CStructure <CSISFieldRoot::ESISDataUnit>
	{
public:
	/**
	 * Default constructor
	 */
	CSISDataUnit ();
	/**
	 * Copy constructor
	 */
	CSISDataUnit (const CSISDataUnit& aInitialiser);
	/**
	 * Class name
	 */
	virtual std::string Name () const;
	/**
	 * Load the given file into memory. And add it into the list of
	 * file data.
	 * @param aFile file name of file.
	 * @param aSize size of data read from the file.
	 */
	TUint32 LoadFile (const std::wstring& aFile, TUint64* aSize = NULL);
	/**
	 * Compressed size of last file in the file list.
	 */
	TUint64 CompressedSize () const;
	/**
	 * Un-compressed size of last file in the file list.
	 */
	TUint64 UncompressedSize () const;
	/**
	 * Retrieve file data of the specified file.
	 * @param aFileIndex Index of the file to be accessed.
	 */
	const CSISFileData& FileData (const TUint32 aFileIndex) const;
	/**
	 * Number of files present in this data unit.
	 */
	TUint32 FileCount() const;

private:
	CSISArray <CSISFileData, CSISFieldRoot::ESISFileData> iFileData;

	};



inline CSISDataUnit::CSISDataUnit ()
	{
	InsertMember (iFileData); 
	}


inline CSISDataUnit::CSISDataUnit (const CSISDataUnit& aInitialiser) :
		CStructure <CSISFieldRoot::ESISDataUnit> (aInitialiser),
		iFileData (aInitialiser.iFileData)
	{
	InsertMember (iFileData); 
	}


inline TUint32 CSISDataUnit::LoadFile (const std::wstring& aFile, TUint64* aSize)
	{
	iFileData.Push ();
	iFileData.Last ().Load (aFile, aSize);
	return iFileData.size () - 1;
	}


inline std::string CSISDataUnit::Name () const
	{
	return "Data Unit";
	}


inline TUint64 CSISDataUnit::CompressedSize () const
	{
	return iFileData.Last ().CompressedSize ();
	}

inline TUint64 CSISDataUnit::UncompressedSize () const
	{
	return iFileData.Last ().UncompressedSize ();
	}


inline const CSISFileData& CSISDataUnit::FileData (const TUint32 aIndex) const
	{
	return iFileData [aIndex];
	}

inline TUint32 CSISDataUnit::FileCount () const
	{
	return iFileData.size();
	}

#endif // __SISDATAUNIT_H__

