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

#ifndef __SISFILEDATA_H__
#define __SISFILEDATA_H__


#include "structure.h"
#include "siscompressed.h"
#include "raw.h"


class CSISFileData : public CStructure <CSISFieldRoot::ESISFileData>
	{
public:
	/**
	 * Default constructor.
	 */
	CSISFileData ();
	/**
	 * Copy constructor.
	 */
	CSISFileData (const CSISFileData& aInitialiser);
	/**
	 * Class name
	 */
	virtual std::string Name () const;
	/**
	 * Load the file into memory and store it in the class.
	 * @param aFile file name
	 * @param aSize if specified then it will return the size of the file read.
	 */
	void Load (const std::wstring& aFile, TUint64* aSize = NULL);
	/**
	 * Return compressed size of the file.
	 */
	TUint64 CompressedSize () const;
	/**
	 * Return un-compressed size of the file.
	 */
	TUint64 UncompressedSize () const;
	/**
	 * Return pointer to the file data.
	 */
	const TUint8* Data () const;
	/**
	 * Get SID of the file.
	 */
	TUint32 CSISFileData::GetSid() const;
	/**
	 * Check whether the file is an executable or not.
	 * An executable can be a dll or an exe.
	 */
	bool IsExecutable() const;
	/**
	 * Checks whether the file is an exe or not.
	 */
	bool IsExe() const;
	/**
	 * Checks whether the file is a dll or not.
	 */
	bool IsDll() const;
	/**
	 * Checks whether the file is an emulator based executable or not.
	 */
	bool IsEmulatorExecutable() const;

private:
	CSISCompressed <CRaw>	iData;
	};



inline CSISFileData::CSISFileData ()
	{
	InsertMember (iData); 
	}


inline CSISFileData::CSISFileData (const CSISFileData& aInitialiser) :
		CStructure <CSISFieldRoot::ESISFileData> (aInitialiser),
		iData (aInitialiser.iData)
	{
	InsertMember (iData); 
	}

inline std::string CSISFileData::Name () const
	{
	return "File Data";
	}

inline TUint64 CSISFileData::CompressedSize () const
	{
	return iData.CompressedSize ();
	}


inline TUint64 CSISFileData::UncompressedSize () const
	{
	return iData.UncompressedSize ();
	}


inline const TUint8* CSISFileData::Data () const
	{
	return iData.Content ().Data ();
	}

#endif // __SISFILEDATA_H__

