/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef	__DUMPSIS_H__
#define	__DUMPSIS_H__

#include "siscontents.h"
#include "sisinstallblock.h"

/**
 * Structure to hold file description and the capability
 * verification result. File description of an executable
 * will have a capability, the iCapVerified field is filled
 * after verifying this capability against the files actual
 * capability. Actual capability is extracted from the file 
 * itself/
 */
typedef struct
	{
	const CSISFileDescription*	iFileDesc;
	int							iActualCap;
	bool						iCapVerified;
	} TFileCapTest;

typedef std::vector<TFileCapTest> TFileCapTestList;

class CDumpSis
	{
public:
	enum TExtractionLevel
		{
		ENone,
		EOnlyPackage,		// Extracts package file for the SIS file.
		ECertificates,		// Exracts all the certificates present in the SIS.
		EBaseFiles,			// Extract all files present in the SIS (excluding files of embedded SIS)
		EAllDataFiles,		// Extract all files including those present in the embedded SIS.
		EAllButCerts,		// EAllFiles + Embedded SIS files and their packages
		EEverything,		// EAllButCerts + certificates
		};
public:
	/**
	 * Constructor will take sis file name as input. It will load the file 
	 * in memory for future operation. In case of an invalid SIS the 
	 * constructor will throw an exception.
	 */
	explicit CDumpSis(const std::wstring& aSISFileName, bool aVerbose = false);
	/**
	 * Desctructor. Will delete or free the owned resources.
	 */
	~CDumpSis();
	
public:
	/**
	 * This function will create the package file for the SIS file.
	 * @param Name of the package file to be created.
	 */
	void CreatePackage(const std::wstring& aPkgFileName);
	/**
	 * This function will generate package file for a specific controller.
	 * @param aSisController controller for which the package file needs to be generated.
	 * @param aPkgFileName Name of the package file to be created.
	 */
	void CreatePackage(const CSISController& aSisController, const std::wstring& aPkgFileName);
	/**
	 * This will extract contents of the SIS file based on the extraction level provided.
	 * Please refer TExtractionLevel for details.
	 * @param aTargetDir Directory to which the contents to be extracted.
	 * @param aLevel Tells the function how much content needs to be extracted.
	 */
	void ExtractFiles(const std::wstring& aTargetDir, TExtractionLevel aLevel);
	/**
	 * This function will extract files belonging to a specified controller.
	 * @param aSisController Controller whose files need to be extracted.
	 * @param aTargetDir Directory to which the files need to be extracted.
	 * @param aBaseIndex Absolute DataUnit index of the parent controller.
	 */
	void ExtractFiles(const CSISController& aSisController, const std::wstring& aTargetDir, int aBaseIndex = 0);
	/**
	 * Extract the package to a specified directory.
	 * @param aTargetDir Directory to which the package file needs to be extracted.
	 */ 
	void ExtractBasePackageFile(const std::wstring& aTargetDir);
	/**
	 * Extract all the certificate chains present in the sis file.
	 * @param aTargetDir Destination folder.
	 */ 
	void ExtractAllCertificates(const std::wstring& aTargetDir);
	/**
	 * Extract all the certificate chains present in the passed controller
	 * @param aSisController Controller whose ceritificates needs to be extracted.
	 * @param aTargetDir Destination folder.
	 */
	void ExtractCertificates(const CSISController& aSisController, const std::wstring& aTargetDir);
	/**
	 * Verify the capabilites stored in file descrition agains the capability extracted
	 * from the file content. Return the list of all files will capability verificaton.
	 * @param aFileList List of files.
	 */
	void GetCapVerifiedFileList(TFileCapTestList& aFileList);
	
private:
	void CreateTargetDir(std::wstring& aTargetDir);
	void CreateDirectoriesRecursively(std::wstring aTargetDir);
	void SisFileNameToPkgFileName(std::wstring& aFileName);
	void CreateEmbeddedSis(const std::wstring& aFileName, CSISController& aController, int aStart, int aEnd);
	void ExtractNestedSisFile(	const std::wstring& aTargetDir, 
								const CSISController& aController, 
								bool aExtractSis,
								int aStartIndex, 
								int aEndIndex);

private:
	CDumpSis(const CDumpSis&){}
	CDumpSis& operator=(const CDumpSis&){return *this;}
	
private: //Private Member Variables
	CSISContents	iSisContents;
	CSISController*	iController;
	std::wstring	iSisFileName;
	TBool			iVerbose;
	TBool			iIsStub;
	};

#endif	// __DUMPSIS_H__
