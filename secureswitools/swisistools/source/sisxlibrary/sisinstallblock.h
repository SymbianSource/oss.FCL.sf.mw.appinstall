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
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef __SISINSTALLBLOCK_H__
#define __SISINSTALLBLOCK_H__

#include <vector>
#include <map>

#include "structure.h"
#include "sisarray.h"
#include "sisfiledescription.h"


class CSISIf;
class CSISController;
class CSISData;
class CSISDataUnit;

// Typedefs
typedef std::map<TUint32, const CSISController*> TControllerMap;
typedef std::map<TUint32, const CSISController*>::const_iterator TControllerMapConstIter;
typedef std::vector<const CSISFileDescription*> TFileDescList;
typedef std::vector<const CSISFileDescription*>::const_iterator TFileDescListConstIter;

class CSISInstallBlock : public CStructure <CSISFieldRoot::ESISInstallBlock>
	{
public:
	/**
	 * Default constructor
	 */
	CSISInstallBlock ();
	/**
	 * Copy constructor
	 */
	CSISInstallBlock (const CSISInstallBlock& aInitialiser);
	/**
	 * Name of the class
	 */
	virtual std::string Name () const;

	/**
	 * Adds a new file description class into the existing list.
	 */
	void AddFileDescription ();
	/**
	 * Adds this new file description class into the existing list.
	 * @param aFileDescription new file description
	 */
	void AddFileDescription (const CSISFileDescription& aFileDescription);
	/**
	 * @return the last file description object from the list
	 */
	CSISFileDescription& FileDescription ();
	/**
	 * @return the last file description object from the list
	 */
	const CSISFileDescription& FileDescription () const;
	/**
	 * Retrieves the file description pointed by index
	 * @param aIndex index of CSISFileDescription
	 * @return file description object pointed by aIndex.
	 */
	const CSISFileDescription& FileDescription (TUint32 aIndex) const;
	/**
	 * Retirieves the number file present in this install block.
	 * @return file count
	 */
	inline TUint32 FileCount() const;
	/**
	 * Converts the string into installation option (only if its valid) and sets the
	 * option to last file of installblock.
	 * @param aOption installation option string.
	 * @return returns the installation option
	 */
	CSISFileDescription::TSISInstOption InterpretOption (const std::wstring& aOption);
	/**
	 * Set MIME type of the last file of the install block.
	 * @param aMimeType MIME type
	 */
	void SetMimeType (const std::wstring& aMimeType);
	/**
	 * Set compressed and uncompressed length of the last file present in the install block.
	 * @param aUncompressed uncompressed length
	 * @param aCompressed compressed length
	 */
	void SetLengths (const TUint64 aUncompressed, const TUint64 aCompressed);
	/**
	 * Sets the file index of the last file present in the install block.
	 * @param aFileIndex new file index.
	 */
	void SetFileIndex (const TUint32 aFileIndex);
	/**
	 * Sets the file target of the last file present in the install block.
	 * @param aTarget new target
	 */
	void SetTarget (const std::wstring& aTarget);
	/**
	 * Sets the file operation of the last file present in the install block.
	 * @param aTarget new operation
	 */
	void SetOperation (const TUint32 aOperation);
	/**
	 * Sets the file options of the last file present in the install block.
	 * @param aTarget new option
	 */
	void SetOptions (const TUint32 aOperationOptions);
	/**
	 * Sets the file hash of the last file present in the install block.
	 * @param aTarget new hash
	 */
	void SetHash (const CSISHash& aHash);

	/**
	 * Add a new if block. 
	 */
	void AddIf ();
	/**
	 * returns the last if block
	 * @return CSISIf
	 */
	CSISIf& If ();
	/**
	 * returns the last if block
	 * @return CSISIf
	 */
	const CSISIf& If () const;
	/**
	 * returns the install block of the last if block
	 * @return CSISInstallBlock
	 */
	CSISInstallBlock& IfInstall ();
	/**
	 * returns the install block of the last if block
	 * @return CSISInstallBlock
	 */
	const CSISInstallBlock& IfInstall () const;
	/**
	 * Embeds the controller into the install block.
	 * @param aController controller to be inserted.
	 */
	void Embed (class CSISController& aController, const TUint32 aIndex);
	/**
	 * Extract capabilities of the last file of the install block.
	 */
	void ExtractCapabilities(const std::wstring& aFileName);
	/**
	 * Add package entry related to this install block.
	 * @param aStream stream where the details need to be written.
	 * @param aVerbose if true detail description will be written.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * Get the list of files present in the sis file. It can also return all
	 * the files present in the nested if blocks and else blocks.
	 * @param aFileList file list which will be filled by this function.
	 * @param aRecursive if true it will check all the nested if blocks and else
	 * 		blocks for files.
	 */
	void GetFileList(TFileDescList& aFileList, bool aRecursive = true) const;
	/**
	 * Get the list of embedded sis files present in the sis file. It can also 
	 * return all the embedded sis files present in the nested if blocks 
	 * and else blocks.
	 * @param aControllers file list which will be filled by this function.
	 * @param aRecursive if true it will check all the nested if blocks and else
	 * 		blocks for files.
	 */
	void GetEmbeddedControllers(TControllerMap& aControllers, bool aRecursive = true) const;
	/**
	 * Retrieves the list of if blocks.
	 * @return sis array of CSISIf class.
	 */
	inline const CSISArray <CSISIf, CSISFieldRoot::ESISIf>& Ifs() const;

private:
	void InsertMembers ();

private:
	CSISArray <CSISFileDescription, CSISFieldRoot::ESISFileDescription>	iFiles;
	CSISArray <CSISController, CSISFieldRoot::ESISController>			iEmbeddedFiles;
	CSISArray <CSISIf, CSISFieldRoot::ESISIf>							iIfs;

	};



inline std::string CSISInstallBlock::Name () const
	{
	return "Install Block";
	}


inline void CSISInstallBlock::SetMimeType (const std::wstring& aMimeType)
	{
	iFiles.Last ().SetMimeType (aMimeType);
	}


inline void CSISInstallBlock::SetLengths (const TUint64 aUncompressed, const TUint64 aCompressed)
	{
	iFiles.Last ().SetLengths (aUncompressed, aCompressed);
	}


inline void CSISInstallBlock::AddFileDescription ()
	{
	iFiles.Push ();
	}

inline void CSISInstallBlock::AddFileDescription (const CSISFileDescription& aFileDescription)
	{
	iFiles.Push (aFileDescription);
	}


inline CSISFileDescription::TSISInstOption CSISInstallBlock::InterpretOption (const std::wstring& aOption)
	{
	return iFiles.Last ().InterpretOption (aOption);
	}


inline void CSISInstallBlock::SetFileIndex (const TUint32 aFileIndex)
	{
	iFiles.Last ().SetFileIndex (aFileIndex);
	}


inline void CSISInstallBlock::SetTarget (const std::wstring& aTarget)
	{
	iFiles.Last ().SetTarget (aTarget);
	}


inline void CSISInstallBlock::SetOperation (const TUint32 aOperation)
	{
	iFiles.Last ().SetOperation (aOperation);
	}


inline void CSISInstallBlock::SetOptions (const TUint32 aOperationOptions)
	{
	iFiles.Last ().SetOptions (aOperationOptions);
	}

inline CSISFileDescription& CSISInstallBlock::FileDescription () 
	{
	return iFiles.Last ();
	}

inline const CSISFileDescription& CSISInstallBlock::FileDescription () const
	{
	return iFiles.Last ();
	}

inline const CSISFileDescription& CSISInstallBlock::FileDescription (TUint32 aIndex) const
	{
	return iFiles[aIndex];
	}

inline TUint32 CSISInstallBlock::FileCount() const
	{
	return iFiles.size();
	}

inline const CSISArray <CSISIf, CSISFieldRoot::ESISIf>& CSISInstallBlock::Ifs() const
	{
	return iIfs;
	}

// Due to circular dependency of InstallBlock SISController and SISIf following
// header inclusion is done. 
#include "siscontroller.h"
#include "sisif.h"

#endif // __SISINSTALLBLOCK_H__

