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

#ifndef __SISDATA_H__
#define __SISDATA_H__


#include "sisdataunit.h"
#include "sisarray.h"


/**
 * CSISData is collection of CSISDataUnit class. And CSISData is
 * part of CSISContents.
 */
class CSISData : public CStructure <CSISFieldRoot::ESISData>
	{
private:
	typedef CSISArray <CSISDataUnit, CSISFieldRoot::ESISDataUnit> TDataUnit;

public:
	typedef TDataUnit::TMemberCount TDataUnitCount;

public:
	/**
	 * Default constructor
	 */
	CSISData ();
	/**
	 * Copy Constructor
	 */
	CSISData (const CSISData& aInitialiser);
	/**
	 * Class Name
	 */
	virtual std::string Name () const;
	/**
	 * Reads the stream and internalize the class.
	 * @param aFile Stream to be read.
	 * @param aContainerSize size of the content to be read.
	 * @param aArrayType Type of array. ESISUndefined if its not an array  
	 */
	virtual void Read (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType = CSISFieldRoot::ESISUndefined);

	/**
	 * Read the file and add the content into the last sisdataunit's last file.
	 * @param aFile file to be read.
	 */
	TUint32 LoadFile (const std::wstring& aFile, TUint64* aSize = NULL);
	/**
	 * Get the CSISDataUnit object specified by the index.
	 * @param aUnit - index of the data unit required.
	 * @param const reference to CSISDataUnit class.
	 */
	const CSISDataUnit& GetDataUnit (const TDataUnitCount aUnit) const;
	/**
	 * Get the CSISDataUnit object specified by the index.
	 * @param aUnit - index of the data unit required.
	 * @param reference to CSISDataUnit class.
	 */
	CSISDataUnit& GetDataUnit (const TDataUnitCount aUnit);
	/**
	 * Add a new data unit into CSISData.
	 */
	TDataUnitCount AddDataUnit (const CSISDataUnit& aUnit);
	/**
	 * Append DataUnits present the passed CSISData.
	 */
	TDataUnitCount AppendData (const CSISData& aData);
	/**
	 * Add a new SISDataUnit into the existing list.
	 */
	void SetDefaultContent ();
	/**
	 * Checks if there is any data in the sis. i.e. it checks whether
	 * there any SISDataUnit present in the sis.
	 * @return true if data is present else false.
	 */
	bool DataPresent() const;
	/**
	 * Returns the compressed size of the last data unit's last file.
	 */
	TUint64 CompressedSize () const;
	/**
	 * Returns the un-compressed size of the last data unit's last file.
	 */
	TUint64 UncompressedSize () const;
	/**
	 * Retrievs a specific data unit. In case of invalid index the function
	 * will throw an exception.
	 * @param aUnit Index into SISData which uniquely selects a SISDataUnit
	 * @return return the reference to the SISDataUnit obect.
	 */
	const CSISDataUnit& DataUnit (const TDataUnitCount aUnit) const;
	/**
	 * Retrievs a specific data unit. In case of invalid index the function
	 * will throw an exception.
	 * @param aUnit Index into SISData which uniquely selects a SISDataUnit
	 * @return return the reference to the SISDataUnit obect.
	 */
	CSISDataUnit& DataUnit (const TDataUnitCount aUnit);

	/**
	 * Set the state of SISData to Stub. And clears the data unit present in it.
	 */
	void SetStub ();
	/**
	 * Checks if the SISData is of a stub or not.
	 */
	bool Stub () const;
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * Retrieve the data unit count present in this SISData.
	 */
	inline TDataUnitCount DataUnitCount() const;

private:
	void InsertMembers ();

private:
	TDataUnit	iDataUnit;
	bool		iStub;
	};


inline void CSISData::InsertMembers ()
	{
	InsertMember (iDataUnit);
	}

inline CSISData::CSISData () :
		iStub (false)
	{ 
	InsertMembers (); 
	}


inline CSISData::CSISData (const CSISData& aInitialiser) :
		CStructure <CSISFieldRoot::ESISData> (aInitialiser),
		iDataUnit (aInitialiser.iDataUnit),
		iStub (aInitialiser.iStub)
	{ 
	InsertMembers (); 
	}


inline TUint32 CSISData::LoadFile (const std::wstring& aFile, TUint64* aSize)
	{
	return iDataUnit.Last ().LoadFile (aFile, aSize); 
	}


inline const CSISDataUnit& CSISData::DataUnit (const TDataUnitCount aUnit) const
	{
	return iDataUnit [aUnit];
	}


inline CSISDataUnit& CSISData::DataUnit (const TDataUnitCount aUnit)
	{
	return iDataUnit [aUnit];
	}


inline CSISData::TDataUnitCount CSISData::AddDataUnit (const CSISDataUnit& aUnit)
	{ 
	iDataUnit.Push (aUnit); 
	return iDataUnit.size () - 1; 
	}


inline std::string CSISData::Name () const
	{
	return "Data";
	}


inline void CSISData::SetDefaultContent ()
	{
	iDataUnit.Push (CSISDataUnit ());
	}

inline bool CSISData::DataPresent () const
	{
	return (iDataUnit.size() != 0);
	}
	
inline TUint64 CSISData::CompressedSize () const
	{
	return iDataUnit.Last ().CompressedSize ();
	}


inline TUint64 CSISData::UncompressedSize () const
	{
	return iDataUnit.Last ().UncompressedSize ();
	}

inline void CSISData::SetStub ()
	{
	iStub = true;
	iDataUnit.clear ();
	SetByteCount (0);
	}

inline bool CSISData::Stub () const
	{
	return iStub;
	}

inline CSISData::TDataUnitCount CSISData::DataUnitCount() const
	{
	return iDataUnit.size();
	}

#endif // __SISDATA_H__

