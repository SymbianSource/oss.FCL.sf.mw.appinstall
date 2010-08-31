/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif 

// System Includes
#include <openssl/sha.h>
#include <iostream>

// User Includes
#include "is_utils.h"
#include "errors.h"
#include "sisfile.h"
#include "controllerinfo.h"
#include "hashcontainer.h"
#include "expressionevaluator.h"

// SisX Library Includes
#include "sisfiledescription.h"
#include "sisfiledata.h"
#include "siscontents.h"
#include "siscontroller.h"
#include "sisinfo.h"
#include "sisinstallblock.h"
#include "sisarray.h"
#include "sissupportedlanguages.h"
#include "sissupportedoptions.h"
#include "sissupportedoption.h"
#include "sislanguage.h"
#include "sisdataunit.h"
#include "sisdata.h"
#include "sisstring.h"

const std::string Type2String(CSISInfo::TSISInstallationType aType)
{
	switch (aType)
	{
	case CSISInfo::EInstInstallation :
		return "SA";
	case CSISInfo::EInstAugmentation :
		return "SP";
	case CSISInfo::EInstPartialUpgrade :
		return "PU";
	case CSISInfo::EInstPreInstalledApp :
		return "PA";
		break;
	case CSISInfo::EInstPreInstalledPatch :
		return "PP";
	default:
		return "Invalid/Unknown Type";
	}
}


SisFile::~SisFile()
	{
	}

SisFile::SisFile(const std::wstring& aFilename)
	{
	iContents.Load(aFilename);
	}


TUint32 SisFile::GetPackageUid() const
	{
	return iContents.UID1();
	}


const CSISPrerequisites* SisFile::GetDependencies() const
	{
	return &iContents.Controller().Prerequisites();
	}


const CSISProperties* SisFile::GetProperties() const
	{
	return &iContents.Controller().Properties();
	}

std::wstring SisFile::GetVendorLocalName() const
	{
	return iContents.Controller().SISInfo().VendorName();
	}


void SisFile::GetControllerData(const char*& aData, int& aLen) const
	{
	aData = (char*)iContents.Controller().RawBuffer();
	aLen = iContents.Controller().RawBufferSize();
	}

const CSISController& SisFile::GetController()
	{
		return iContents.Controller();
	}

bool SisFile::GetInstallableFiles(InstallableFiles& aFiles, 
								  ExpressionEvaluator& aEvaluator,
								  const std::wstring& aDrivePath,
								  int aInstallingDrive) const
{
	bool success = true;
	
	CSISInfo::TSISInstallationType installType = iContents.Controller().SISInfo().InstallationType();
	int count = iContents.SisData().DataUnitCount();
	const CSISDataUnit* dataUnit = NULL;
	
	if (installType ==  CSISInfo::EInstInstallation || installType == CSISInfo::EInstAugmentation || installType == CSISInfo::EInstPartialUpgrade)
		{
		dataUnit = &(iContents.DataUnit(0));
		}

	// Logo
	if (!iContents.Controller().SISLogo().WasteOfSpace())
		{
		const CSISFileDescription& fileDes = iContents.Controller().SISLogo().FileDesc();

		// FT or empty target file names are not installed
		if(fileDes.Operation() != CSISFileDescription::EOpText && fileDes.Target().size() > 0)
			{
			// for PA stub sis files no file data present,
			// aFiles will be filled with only file descriptions of files, empty file data
			if (NULL != dataUnit && fileDes.Operation() != CSISFileDescription::EOpNull)
				{
				aFiles.push_back(new InstallableFile(fileDes, new CSISFileData(dataUnit->FileData(fileDes.FileIndex())), aDrivePath, aInstallingDrive));		
				}
			else 
				{
				// SIS files will not be present in PA stubs
				aFiles.push_back( new InstallableFile(fileDes,aDrivePath,aInstallingDrive));		
				}
			}
		}

	// process main controller - data unit 0
	const CSISInstallBlock& installBlock = iContents.Controller().InstallBlock();

	ProcessInstallBlock(installBlock, aFiles, aEvaluator, aDrivePath, aInstallingDrive);

	return success;
}

bool SisFile::HasEmbedded() const
{
	const CSISInstallBlock& blk = iContents.Controller().InstallBlock();	
	TControllerMap embeddedCtls;
	iContents.Controller().InstallBlock().GetEmbeddedControllers(embeddedCtls, false);
	
	return (embeddedCtls.size() != 0);	
}

void SisFile::CheckValid() const
{
	std::string error;

	CSISInfo::TSISInstallationType installType = iContents.Controller().SISInfo().InstallationType();
	// Allow SA, SP, PU and PA(stub) installations only
	bool success = 	(installType == CSISInfo::EInstInstallation) 	|| 
					(installType == CSISInfo::EInstPreInstalledApp)	||
					(installType == CSISInfo::EInstAugmentation) 	|| 
					(installType == CSISInfo::EInstPartialUpgrade);

	if (installType == CSISInfo::EInstPreInstalledApp)
		{
		LWARN(L"Installation of PA type SIS files may cause problems on upgrade and/or restoration. Please see the documentation for details." );
		}

	if (!success)
		{
		std::string type = Type2String(installType);
		error = "Invalid package type (" + type + ")";
		}
	bool failed = !success;

    
	success = iContents.Controller().SupportedOptionCount() == 0;
	if (!success)
		error += "SIS File contains user options";
	failed = failed || !success;

	const CSISInstallBlock& blk = iContents.Controller().InstallBlock();
	success = ProcessInstallOptionsWarning( blk, error);

	failed = failed || !success;

	if (failed)
		{
		std::string x = wstring2string(this->GetPackageName());
		throw InvalidSis(x, error, SIS_NOT_SUPPORTED);
		}

	}

bool SisFile::ProcessInstallOptionsWarning(const CSISInstallBlock& aInstallBlock, std::string& aError)
	{
	bool success = true;

	int fileCount = aInstallBlock.FileCount();
	for(int i = 0; i < fileCount; ++i)
		{
		const CSISFileDescription& fD = aInstallBlock.FileDescription(i);
        const CSISFileDescription::TSISFileOperation operation = fD.Operation();
		std::wstring target(fD.Target().GetString().c_str());
        //
        switch( operation )
            {
        case CSISFileDescription::EOpInstall:
            success = true;
            break;
        case CSISFileDescription::EOpRun:
        	{
				const CSISFileDescription::TSISInstOption operationOptions = fD.OperationOptions();
				if(operationOptions & CSISFileDescription::EInstFileRunOptionByMimeType)
				{
					LWARN(L"File " << target << L" contains \"Run-Using-MIME\" option that will be ignored.");
				}
				if((operationOptions & CSISFileDescription::EInstFileRunOptionInstall) \
						&& (operationOptions & CSISFileDescription::EInstFileRunOptionUninstall))
				{
					LWARN(L"File " << target << L" contains \"RUN-BOTH\" option that will be ignored.");			
				}
				else if(operationOptions & CSISFileDescription::EInstFileRunOptionInstall)
				{
					LWARN(L"File " << target << L" contains \"Run-On-Install\" option that will be ignored.");			
				}
				else if(operationOptions & CSISFileDescription::EInstFileRunOptionUninstall)
				{
					LWARN(L"File " << target << L" contains \"Run-On-Uninstall\" option that will be ignored.");			
				}
				if(operationOptions & CSISFileDescription::EInstFileRunOptionBeforeShutdown)
				{
					LWARN(L"File " << target << L" contains \"Run-Before-Shutdown\" option that will be ignored.");			
				}
                if(operationOptions & CSISFileDescription::EInstFileRunOptionAfterInstall)
				{
					LWARN(L"File " << target << L" contains \"Run-After-Install\" option that will be ignored.");			
				}
				LWARN(L"File " << target << L" contains \"File-Run\" option that will be ignored.");			
			}
            success = true;
            break;
        case CSISFileDescription::EOpText:
        	LWARN(L"File " << target << L" contains \"Display Text\" option that will be ignored." );
            success = true;
            break;
        case CSISFileDescription::EOpNull:
		case CSISFileDescription::EOpNone:
            success = true;
            break;
        default:
            success = false;
            break;
            }
		//
        if (!success)
    		{
			aError += "SIS File contains install options : "+operation;
			break;
	    	}
		}
		return success;
	}


std::wstring SisFile::GetVendorName() const
	{
	return iContents.Controller().SISInfo().UniqueVendorName();
	}

std::wstring SisFile::GetPackageName() const
	{
	return iContents.Controller().SISInfo().PackageName(0);
	}

TUint32 SisFile::GetIndex() const
	{
	return iContents.Controller().DataIndex();
	}

TUint32 SisFile::GetSigned() const
{
	return true;
}

TUint32 SisFile::GetInstallType() const
	{
	return iContents.Controller().SISInfo().InstallationType();
	}

TUint32 SisFile::GetInstallFlags() const
	{
	return iContents.Controller().SISInfo().InstallationFlag();
	}

void SisFile::AddInstallFlags(const TUint8 aFlag) const
	{
	iContents.Controller().AddInstallFlag(aFlag);
	}

TUint32 SisFile::GetLanguage() const
	{
	return iContents.Controller().Language(0);
	}

Version SisFile::GetVersion() const
	{
	const CSISVersion& v =	iContents.Controller().SISInfo().SISVersion();
	return Version(v.Major(), v.Minor(), v.Build());
	}

const Controllers SisFile::GetControllerInfo(const TUint16 aRegistryFileMajorVersion, 
											 const TUint16 aRegistryFileMinorVersion) const
	{
	ControllerInfo* ci = new ControllerInfo();
	ci->SetVersion(GetVersion());
	ci->CalculateAndSetHash(iContents.Controller(),aRegistryFileMajorVersion,aRegistryFileMinorVersion);
	ci->SetOffset(0);
	Controllers c;

	c.push_back(ci);

	// Embedded controllers
	
	TControllerMap embeddedCtls;
	iContents.Controller().InstallBlock().GetEmbeddedControllers(embeddedCtls, false);
	for (TControllerMapConstIter iter = embeddedCtls.begin(); iter != embeddedCtls.end(); ++iter)	
		{
		const CSISController& ctrl = *iter->second;
		ControllerInfo* ci = new ControllerInfo();

		ci->SetVersion(GetVersion());
		ci->CalculateAndSetHash(ctrl,aRegistryFileMajorVersion,aRegistryFileMinorVersion);
		ci->SetOffset(0);
		c.push_back(ci);
		}

	return c;
	}


std::vector<TInt> SisFile::GetAllInstallChainIndices() const
	{
	int signatureCount = iContents.Controller().SignatureCount();

	std::vector<TInt> result;
	for(int index = 0; index < signatureCount; ++index)
		{
		result.push_back(index);
		}
	return result;
	}

void SisFile::GetInstallableFiles(	InstallableFiles& aFiles, 
									const CSISInstallBlock& aInstallBlock, 
									const std::wstring& aDrivePath,
									int aInstallingDrive) const
	{
	CSISInfo::TSISInstallationType installType = iContents.Controller().SISInfo().InstallationType();
	const CSISDataUnit* dataUnit = NULL;
	
	if (installType ==  CSISInfo::EInstInstallation || installType == CSISInfo::EInstAugmentation || installType == CSISInfo::EInstPartialUpgrade)
		{
		dataUnit = &(iContents.DataUnit(0));
		}

	int fileCount = aInstallBlock.FileCount();

	for (int i = 0; i < fileCount; ++i)
		{
		const CSISFileDescription& fileDes = aInstallBlock.FileDescription(i);
		
		// FT or empty target file names are not installed
		if(fileDes.Operation() == CSISFileDescription::EOpText && fileDes.Target().size() == 0)
			{
			continue;
			}

		// for PA stub sis files no file data present,
		// aFiles will be filled with only file descriptions of files, empty file data
		if (NULL != dataUnit && fileDes.Operation() != CSISFileDescription::EOpNull)
			{
			const CSISFileData& filedataref = dataUnit->FileData(fileDes.FileIndex());
			aFiles.push_back(new InstallableFile(fileDes, new CSISFileData(dataUnit->FileData(fileDes.FileIndex())), aDrivePath, aInstallingDrive));
			}
		else // for stubs, no file data
			{
			aFiles.push_back(new InstallableFile(fileDes, aDrivePath, aInstallingDrive));
			}
		}
	}

void SisFile::ProcessInstallBlock(const CSISInstallBlock& aInstallBlock, 
								  InstallableFiles& aFiles, 
								  ExpressionEvaluator& aEvaluator, 
								  const std::wstring& aDrivePath,
								  int aInstallingDrive) const
	{
	GetInstallableFiles(aFiles, aInstallBlock, aDrivePath, aInstallingDrive);

	const CSISArray<CSISIf, CSISFieldRoot::ESISIf>& ifs = aInstallBlock.Ifs();
	for (int i = 0; i < ifs.size(); ++i)
		{
		const CSISIf& ifBlock = ifs[i];

		if (ifBlock.WasteOfSpace())
			{
			std::string x;
			std::string error = "corrupt SIS file";
			throw InvalidSis(Ucs2ToUtf8(this->GetPackageName(),x), error, INVALID_SIS);
			}

		// Main expression
		const ExpressionResult expressionResult = aEvaluator.Evaluate( ifBlock.Expression() );
		const bool processBlock = expressionResult.BoolValue();
		if ( processBlock )
			{
			ProcessInstallBlock(ifBlock.InstallBlock(), aFiles, aEvaluator, aDrivePath, aInstallingDrive);
			continue;
			}
		
		int elseCount = ifBlock.ElseIfCount();
		for (int i = 0; i < elseCount; ++i)
			{
			const CSISElseIf& ifElseBlock = ifBlock.ElseIf(i) ;
			if ( aEvaluator.Evaluate(ifElseBlock.Expression()).BoolValue())
				{
				ProcessInstallBlock(ifElseBlock.InstallBlock(), aFiles, aEvaluator, aDrivePath, aInstallingDrive);
				break;	// Stop processing else if blocks
				}
			
			}
		} 
	}

PackageUids SisFile::GetEmbeddedPackageUids() const
{
	// Embedded controllers
	PackageUids pkgs;
	TControllerMap embeddedCtls;
	iContents.Controller().InstallBlock().GetEmbeddedControllers(embeddedCtls, false);
	for (TControllerMapConstIter iter = embeddedCtls.begin(); iter != embeddedCtls.end(); ++iter)
		{
			const CSISController* ctrl = iter->second;
			const CSISInfo& info = ctrl->SISInfo();
			TUint32 uid = info.UID1();
			pkgs.push_back(uid);
		 }
	return pkgs;
}

void SisFile::ProcessEmbeddedFileWarning(const CSISInstallBlock& aInstallBlock) const
	{
	TControllerMap embeddedCtls;
	aInstallBlock.GetEmbeddedControllers(embeddedCtls, false);
	for (TControllerMapConstIter iter = embeddedCtls.begin(); iter != embeddedCtls.end(); ++iter)
		{
		const CSISController* ctrl = iter->second;
		const CSISInfo& info = ctrl->SISInfo();			
		LWARN(L" Embedded Package not installed: UID " << std::hex << info.UID1() );
		}
	}

bool SisFile::IsSupportedLanguage(TUint32 aLanguage) const
{
	bool result = false;
	int langCount = iContents.Controller().LanguageCount();
	for (int i = 0; i < langCount; ++i)
		{
		if (iContents.Controller().Language(i) == aLanguage)
			{
			result = true;
			break;
			}
		}	
	return result;
}

void SisFile::MakeSISStub(std::wstring& aFileName)
	{
	CSISContents contents = iContents;
	contents.SetStub(CSISContents::EStubPreInstalled);
	contents.WriteSIS(aFileName);
	}

