/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SISSTRING_H__
#define __SISSTRING_H__


#include "field.h"

class CSISString : public CSISField <CSISFieldRoot::ESISString>
	{
public:
	/**
	 * Default constructor
	 */
	CSISString ();
	/**
	 * Creates an instance of the class from wstring objec.
	 */
	CSISString (const std::wstring& aValue);
	/**
	 * Creates an instance of this class.
	 * @param aRequired false if the object is optional else true.
	 */
	CSISString (const bool aRequired);
	/**
	 * Creates an instance of the class from wstring objec.
	 * @param aRequired false if the object is optional else true.
	 */
	CSISString (const std::wstring initialValue, const bool aRequired);
	/**
	 * Copy constructor.
	 */
	CSISString (const CSISString& aString);

	/**
	 * Internalize the class.
	 * @param aFile Stream from which the content should be read.
	 * @param aContainerSize Size of the content to be read.
	 * @param aArrayType type of the structure.
	 */
	virtual void Read (TSISStream& aFile, const TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType = CSISFieldRoot::ESISUndefined);
	/**
	 * Externalize the class.
	 * @param aFile stream to which the content needs to be written.
	 * @param aIsArrayElement Whether the instance is part of an array or not.
	 */
	virtual void Write (TSISStream& aFile, const bool aIsArrayElement) const;
	/**
	 * Actual size of the content in memory.
	 * @param aInsideArray Whethere the instance is part of an array or not.
	 */
	virtual TFieldSize ByteCount (const bool aInsideArray) const;
	/**
	 * Write the content into the stream.
	 * @param aStream Stream to which the data needs to be written.
	 * @param aLevel.
	 */
	virtual void Dump (std::ostream& aStream, const int aLevel) const;
	/**
	 * Check whether the content needs to be written into the stream or
	 * not. For string if it is empty there is no need in trying to write
	 * this content. 
	 */
	virtual bool WasteOfSpace () const;
	/**
	 * Calculate CRC of the data.
	 * @param aCRC CRC value of the string.
	 * @param aIsArrayElement Whether the instance is part of an array or not.
	 */
	virtual void CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const;
	/**
	 * Get a copy of the string.
	 */
	std::wstring GetString() const;

	/**
	 * Number of characters in the string.
	 */
	TUint32 size () const;
	/**
	 * Empty the string.
	 */
	bool empty () const;
	/**
	 * Assignment operator.
	 */
	CSISString& operator = (const std::wstring& aString);
	/**
	 * Assignment operator.
	 */
	CSISString& operator = (const wchar_t* aString);
	/**
	 * Assignment operator.
	 */
	CSISString& operator = (const CSISString& aString);

	/**
	 * == operator. Compare the content of the class and aString
	 */
	bool operator == (const std::wstring& aString) const;
	/**
	 * == operator. Compare the content of the class and aString
	 */
	bool operator == (const wchar_t* aString) const;
	/**
	 * == operator. Compare the content of the class and aString
	 */
	bool operator == (const CSISString& aString) const;

	/**
	 * != operator. Compare the content of the class and aString
	 */
	bool operator != (const std::wstring& aString) const;
	/**
	 * != operator. Compare the content of the class and aString
	 */
	bool operator != (const wchar_t* aString) const;
	/**
	 * != operator. Compare the content of the class and aString
	 */
	bool operator != (const CSISString& aString) const;
	
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;

	/**
	 * Delete the temporary buffer used for reading the string.
	 * Should be called after this class is not required anymore.
	 */
	static void ExitInstance ();

private:
	std::wstring iData;

	};




inline CSISString::CSISString ()
	{
	}


inline CSISString::CSISString (const std::wstring& aValue) :
			iData (aValue)
	{
	}


inline CSISString::CSISString (const bool aRequired) :
			CSISField<CSISFieldRoot::ESISString> (aRequired)
	{
	}


inline CSISString::CSISString (const std::wstring initialValue, const bool aRequired) :
			CSISField<CSISFieldRoot::ESISString> (aRequired),
			iData (initialValue)
	{
	}


inline CSISString::CSISString (const CSISString& aString) :
			CSISField<CSISFieldRoot::ESISString> (aString.Required ()),
			iData (aString.iData)
	{
	}


inline TUint32 CSISString::size () const
	{
	return static_cast <TUint32> (iData.size ()); 
	}


inline CSISString& CSISString::operator = (const std::wstring& aString)
	{
	iData = aString;
	return *this;
	}


inline CSISString& CSISString::operator = (const CSISString& aString)
	{
	iData = aString.iData;
	return *this;
	}

inline CSISString& CSISString::operator = (const wchar_t* aString)
	{
	if (aString)
		{
		iData = aString;
		}
	return *this;
	}


inline bool CSISString::operator == (const std::wstring& aString) const
	{
	return iData == aString;
	}

inline bool CSISString::operator == (const CSISString& aString) const
	{
	return iData == aString.iData;
	}

inline bool CSISString::operator == (const wchar_t* aString) const
	{
	if (aString == NULL)
		{
		return false;
		}
	return iData.compare (aString) == 0;
	}


inline bool CSISString::operator != (const std::wstring& aString) const
	{
	return iData != aString;
	}

inline bool CSISString::operator != (const CSISString& aString) const
	{
	return iData != aString.iData;
	}

inline bool CSISString::operator != (const wchar_t* aString) const
	{
	if (aString == NULL)
		{
		return true;
		}
	return wcscmp(iData.c_str(), aString) != 0;
	}


inline bool CSISString::empty () const
	{
	return iData.empty ();
	}

inline std::wstring CSISString::GetString() const
	{
	return iData;
	}

inline void CSISString::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	(void)aVerbose;
	aStream << iData.c_str();
	}

#endif // __SISSTRING_H__

