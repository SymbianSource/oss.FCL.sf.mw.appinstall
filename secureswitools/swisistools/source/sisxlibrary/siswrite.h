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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
* Acts as layer between old MakeSIS and new SISX file format.
* Handles creation of a SISX file
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __SISWRITE_H_
#define __SISWRITE_H_

#include "siscontents.h"
#include <iostream>



class CSISXWriter
	{
public:
	CSISXWriter ();
	void WriteSIS (const std::wstring aTargetFileName);
	void Dump (std::ostream& aStream);
   	void SetVersionInfo (	const CSISUid::TUid aUID, const TVersion& aVersion,
							const TUint32 aType, const TUint32 aFlags);
	TUint32 GetNoLanguages () const;
	bool AreLanguagesSpecified () const;
	void AddLanguage (const CSISLanguage::TDialect aLan);
	void AddProperty (const CSISProperty::TKey aKey, const CSISProperty::TValue aValue);
	CSISUid::TUid UID1 () const;
	void AddName (const std::wstring& aLan);
	void AddVendorName (const std::wstring& aLan);
	void SetVendorUniqueName (const std::wstring& aVendorUniqueName);
	void AddDependencyName (const std::wstring& aName);
	void AddTargetName (const std::wstring& aName);
	void AddOptionName (const std::wstring& aName);
	void AddOption ();
	CSISFileDescription::TSISInstOption InterpretOption (const std::wstring& aOption);
	CSISInfo::TSISInstallationType InterpretType (const std::wstring& aOption);
	void SetMimeType (const std::wstring& aMimeType);
	void AddIf ();
	const CSISIf& If () const;
	CSISIf& If ();
	const CSISInstallBlock& InstallBlock () const;
	CSISInstallBlock& InstallBlock ();
	void SetLogo (const std::wstring& aFile, const std::wstring& aMime, const std::wstring& aTarget);
	void AddDependency (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo);
	void AddTarget (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo);
	void EmbedFile (const std::wstring& aFile, const CSISUid::TUid& aUid, CSISInstallBlock& aInstall);
	void EmbedPackage (CSISInstallBlock& aInstallBlock, const std::wstring& aFile, const CSISUid::TUid& aUid);
	void SetDefaultContent ();
	TUint32 LoadFile (const std::wstring& aFile, TUint64* aSize = NULL);
	TUint64 CompressedSize () const;
	TUint64 UncompressedSize () const;
	const CSISDataUnit& DataUnit () const;
	const CSISDataUnit& DataUnit (const TUint32 aUnit) const;
	CSISLanguage::TLanguage Language (const TUint32 aIndex) const;
	void AddInstallFlag(const TUint8 aFlag);
	const CSISContents& SISContent() const;

private:
	CSISContents		iContent;
	};

inline CSISXWriter::CSISXWriter ()
	{
	}

inline const CSISContents& CSISXWriter::SISContent() const	
	{
	return iContent;
	}

inline void CSISXWriter::Dump (std::ostream& aStream)
	{
	aStream <<  std::endl << iContent.Name () << std::endl << "  ";
	iContent.Dump (aStream, 2);
	aStream << std::endl;
	}

inline void CSISXWriter::SetVersionInfo (	const CSISUid::TUid aUID, const TVersion& aVersion,
											const TUint32 aType, const TUint32 aFlags)
	{
 	iContent.SetVersionInfo (aUID, aVersion, aType, aFlags);
	}

inline TUint32 CSISXWriter::GetNoLanguages () const
	{
	TUint32 count (iContent.LanguageCount ());
	return (count == 0) ? 1 : count; 
	}

inline bool CSISXWriter::AreLanguagesSpecified () const
	{
	return iContent.AreLanguagesSpecified ();
	}

inline void CSISXWriter::AddLanguage (const CSISLanguage::TDialect aLan)
	{
	iContent.AddLanguage (aLan);
	}

inline void CSISXWriter::AddProperty (const CSISProperty::TKey aKey, const CSISProperty::TValue aValue)
	{
	iContent.AddProperty (aKey, aValue);
	}

inline CSISUid::TUid CSISXWriter::UID1 () const
	{
	return iContent.UID1 (); 
	}

inline void CSISXWriter::AddName (const std::wstring& aLan)
	{
	iContent.AddName (aLan); 
	}

inline void CSISXWriter::AddVendorName (const std::wstring& aLan)
	{
	iContent.AddVendorName (aLan);
	}

inline void CSISXWriter::AddDependencyName (const std::wstring& aName)
	{
	iContent.AddDependencyName (aName);
	}

inline void CSISXWriter::AddTargetName (const std::wstring& aName)
	{
	iContent.AddTargetName (aName); 
	}

inline void CSISXWriter::AddOptionName (const std::wstring& aName)
	{
	iContent.AddOptionName (aName); 
	}

inline void CSISXWriter::AddOption ()
	{
	iContent.AddOption ();
	}

inline CSISFileDescription::TSISInstOption CSISXWriter::InterpretOption (const std::wstring& aOption)
	{
	return iContent.InterpretOption (aOption); 
	}

inline CSISInfo::TSISInstallationType CSISXWriter::InterpretType (const std::wstring& aOption)
	{
	return iContent.InterpretType (aOption); 
	}

inline void CSISXWriter::SetMimeType (const std::wstring& aMimeType)
	{
	iContent.SetMimeType (aMimeType); 
	}

inline void CSISXWriter::AddIf ()
	{
	iContent.AddIf ();
	}

inline const CSISIf& CSISXWriter::If () const
	{
	return iContent.If (); 
	}

inline CSISIf& CSISXWriter::If ()
	{
	return iContent.If (); 
	}

inline const CSISInstallBlock& CSISXWriter::InstallBlock () const
	{
	return iContent.InstallBlock (); 
	}

inline CSISInstallBlock& CSISXWriter::InstallBlock ()
	{
	return iContent.InstallBlock ();
	}

inline void CSISXWriter::SetLogo (const std::wstring& aFile, const std::wstring& aMime, const std::wstring& aTarget)
	{
	iContent.SetLogo (aMime, aTarget, aFile, DataUnit()); 
	}

inline void CSISXWriter::AddDependency (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo)
	{
	iContent.AddDependency (aUID, aFrom, aTo);
	}

inline void CSISXWriter::AddTarget (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo)
	{
	iContent.AddTarget (aUID, aFrom, aTo);
	}

inline void CSISXWriter::SetDefaultContent ()
	{
	iContent.SetDefaultContent ();
	}

inline TUint32 CSISXWriter::LoadFile (const std::wstring& aFile, TUint64* aSize)
	{
	return iContent.LoadFile (aFile, aSize);
	}

inline TUint64 CSISXWriter::CompressedSize () const
	{
	return iContent.CompressedSize ();
	}

inline TUint64 CSISXWriter::UncompressedSize () const
	{
	return iContent.UncompressedSize ();
	}

inline const CSISDataUnit& CSISXWriter::DataUnit () const
	{
	return iContent.DataUnit ();
	}

inline const CSISDataUnit& CSISXWriter::DataUnit (const TUint32 aUnit) const
	{
	return iContent.DataUnit (aUnit);
	}

inline void CSISXWriter::SetVendorUniqueName (const std::wstring& aVendorUniqueName)
	{
	iContent.SetVendorUniqueName (aVendorUniqueName);
	}

inline CSISLanguage::TLanguage CSISXWriter::Language (const TUint32 aIndex) const
	{
	return iContent.Language (aIndex);
	}

inline void CSISXWriter::EmbedFile (const std::wstring& aFile, const CSISUid::TUid& aUid, CSISInstallBlock& aInstall)
	{
	iContent.EmbedFile (aFile, aUid, aInstall);
	}

inline void CSISXWriter::EmbedPackage (CSISInstallBlock& aInstallBlock, const std::wstring& aFile, const CSISUid::TUid& aUid)
	{
	iContent.EmbedPackage (aInstallBlock, aFile, aUid);
	}

inline void CSISXWriter::WriteSIS (const std::wstring aTargetFileName)
	{
	iContent.WriteSIS (aTargetFileName);
	}

inline void CSISXWriter::AddInstallFlag(const TUint8 aFlag)
	{
	iContent.AddInstallFlag(aFlag);
	}

#endif // __SISWRITE_H_
