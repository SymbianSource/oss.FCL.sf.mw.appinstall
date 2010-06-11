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

#ifndef __SISDATAINDEX_H__
#define __SISDATAINDEX_H__


#include "structure.h"
#include "numeric.h"


class CSISDataIndex : public CStructure <CSISFieldRoot::ESISDataIndex>
	{
public:
	/**
	 * Default constructor
	 */
	CSISDataIndex ();
	/**
	 * Copy constructor
	 */
	CSISDataIndex (const CSISDataIndex& aInitialiser);
	/**
	 * Class Name
	 */
	virtual std::string Name () const;
	/**
	 * Externalize the class.
	 * @param aFile Stream to which the content to be written.
	 * @param aIsArrayElement true if this structure is part of an array else false.
	 */
	virtual void Write (TSISStream& aFile, const bool aIsArrayElement) const;  

	/**
	 * Set a new value to data index.
	 * @param aIndex new data index value.
	 */
	void SetIndex (const TUint32 aIndex);
	/**
	 * Get the data index value.
	 */
	TUint32 Index () const;

private:
	CSISUInt32 iIndex;
	};


inline CSISDataIndex::CSISDataIndex ()
	{
	InsertMember (iIndex);
	}

inline CSISDataIndex::CSISDataIndex (const CSISDataIndex& aInitialiser) :
		CStructure <CSISFieldRoot::ESISDataIndex> (aInitialiser),
		iIndex (aInitialiser.iIndex)
	{
	InsertMember (iIndex);
	}


inline std::string CSISDataIndex::Name () const
	{
	return "Data Index";
	}


inline void CSISDataIndex::SetIndex (const TUint32 aIndex)
	{
	iIndex = aIndex;
	}


inline TUint32 CSISDataIndex::Index () const
	{
	return iIndex;
	}


inline void CSISDataIndex::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	CStructure <CSISFieldRoot::ESISDataIndex>::Write (aFile, aIsArrayElement);
	}

#endif // __SISDATE_H__

