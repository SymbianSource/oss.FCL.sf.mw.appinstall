/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "CSWICertStoreTool.h"
#include <testconfigfileparser.h>
#include <cctcertinfo.h>
#include <x509cert.h>
#include <utf.h>

_LIT(KSWICertStoreToolName, "SWICertStoreTool");
_LIT8(KNewLine, "\r\n");

_LIT8(KDefaultsSection, "Defaults");

_LIT8(KLogErrorTemplate, "*** %S: Error: %d");
_LIT8(KLogOKTemplate, "    %S: OK");
_LIT8(KLogParameterTemplate, "  %S = %S");

_LIT8(KLogStartMessage, "SWICertStoreTool");
_LIT8(KLogEndMessage, "Done.");
_LIT8(KLogSectionName, "Processing: %S");
_LIT8(KLogIllegalCertLabel, "*** Section name is not a legal cert label - must be between 0 and 64 characters long");
_LIT8(KLogDuplicateCertLabel, "*** Duplicate cert label");
_LIT8(KLogNoFileSpecifiedError, "*** No certificate file spcified (use 'file = <cert filename>'");
_LIT8(KLogIllegalMandatoryValue, "*** Illegal value for mandatory flag (must be 0 or 1)");
_LIT8(KLogIllegalSystemUpgradeValue, "*** Illegal value for system upgrade flag (must be 0 or 1)");
_LIT8(KLogIllegalCapabilityValue, "*** Unrecognised capability name: %S");
_LIT8(KLogIllegalApplicationValue, "*** Unrecognised application (must be 'SWInstall' or 'SWInstallOCSP')");
_LIT8(KLogNoApplications, "*** No applications specified (must one or more of 'SWInstall' and 'SWInstallOCSP')");

_LIT8(KActionOpenInputFile, "Opening input file");
_LIT8(KActionOpenOutputFile, "Opening output file");
_LIT8(KActionParseSection, "Parsing section");
_LIT8(KActionLoadCertData, "Loading certficate file");
_LIT8(KActionParseX509Cert, "Parsing X509 certificate");
_LIT8(KActionWriteCertData, "Writing cert data to store");
_LIT8(KActionWriteInfoStream, "Writing info stream to store");
_LIT8(KActionWriteRootStream, "Writing root stream to store");

_LIT8(KItemFile, "file");
_LIT8(KItemMandatory, "mandatory");
_LIT8(KItemSystemUpgrade, "systemupgrade");
_LIT8(KItemCapability, "capability");
_LIT8(KItemApplication, "application");

_LIT8(KParamInputFile, "input file");
_LIT8(KParamOutputFile, "outut file");
_LIT8(KParamLabel, "label");

_LIT8(KApplicationSWInstall, "SWInstall");
_LIT8(KApplicationSWInstallOCSP, "SWInstallOCSP");

_LIT8(KString0, "0");
_LIT8(KString1, "1");
_LIT8(KFalse, "false");
_LIT8(KTrue, "true");

const TBool KTrueValue = 1;
const TBool KFalseValue = 0;

const TUid KApplicationSWInstallUid = { 0x100042ab };
const TUid KApplicationSWInstallOCSPUid = { 0x1000a8b6 };

const TInt KMaxLineLength = 200;

// Capability names
_LIT8(KCapabilityTCB,  				"TCB");
_LIT8(KCapabilityCommDD,  			"CommDD");
_LIT8(KCapabilityPowerMgmt,  		"PowerMgmt");
_LIT8(KCapabilityMultimediaDD,  	"MultimediaDD");
_LIT8(KCapabilityReadDeviceData,  	"ReadDeviceData");
_LIT8(KCapabilityWriteDeviceData,  	"WriteDeviceData");
_LIT8(KCapabilityDRM,  				"DRM");
_LIT8(KCapabilityTrustedUI,  		"TrustedUI");
_LIT8(KCapabilityProtServ,  		"ProtServ");
_LIT8(KCapabilityDiskAdmin,  		"DiskAdmin");
_LIT8(KCapabilityNetworkControl,  	"NetworkControl");
_LIT8(KCapabilityAllFiles,  		"AllFiles");
_LIT8(KCapabilitySwEvent,  			"SwEvent");
_LIT8(KCapabilityNetworkServices,  	"NetworkServices");
_LIT8(KCapabilityLocalServices,  	"LocalServices");
_LIT8(KCapabilityReadUserData,  	"ReadUserData");
_LIT8(KCapabilityWriteUserData,  	"WriteUserData");
_LIT8(KCapabilityLocation,  		"Location");
_LIT8(KCapabilitySurroundingsDD,  	"SurroundingsDD");
_LIT8(KCapabilityUserEnvironment,  	"UserEnvironment");

#define TRAP_AND_LOG(NAME, ACTION) \
    { \
	TRAPD(err, ACTION); \
	TrapAndLogL(NAME, err); \
	}

 CSWICertStoreTool* CSWICertStoreTool::NewLC(const TDesC& aInputFile,
											 const TDesC& aOutputFile,
											 const TDesC& aLogFile)
	{
	CSWICertStoreTool* self = new (ELeave) CSWICertStoreTool(aInputFile, aOutputFile);
	CleanupStack::PushL(self);
	self->ConstructL(aLogFile);
	return self;
	}

CSWICertStoreTool::CSWICertStoreTool(const TDesC& aInputFile, const TDesC& aOutputFile) :
	iInputFile(aInputFile), iOutputFile(aOutputFile)
	{
	}

void CSWICertStoreTool::ConstructL(const TDesC& aLogFile)
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iLogFile.Replace(iFs, aLogFile, EFileWrite | EFileShareExclusive));
	}

CSWICertStoreTool::~CSWICertStoreTool()
	{
	iLogFile.Close();
	iFs.Close();
	}

void CSWICertStoreTool::LogL(TRefByValue<const TDesC8> aFmt, ...)
	{
	TBuf8<KMaxLineLength> buf;
	VA_LIST args;
	VA_START(args, aFmt);
	buf.AppendFormatList(aFmt, args);
	VA_END(args);

	User::LeaveIfError(iLogFile.Write(buf));
	User::LeaveIfError(iLogFile.Write(KNewLine));
	User::LeaveIfError(iLogFile.Flush());
	}

void CSWICertStoreTool::TrapAndLogL(const TDesC8& aName, TInt err)
	{
	if (err != KErrNone)
		{
		LogL(KLogErrorTemplate, &aName, err); 
		User::Leave(err);										
		}
	else
		{ 
		LogL(KLogOKTemplate, &aName);
		}
	}

void CSWICertStoreTool::LogArgL(const TDesC8& aName, const TDesC& aValue)
	{
	TBuf8<KMaxLineLength> buf;
	buf.Copy(aValue);
	LogL(KLogParameterTemplate, &aName, &buf);
	}

void CSWICertStoreTool::LogParameterL(const TDesC8& aName, const TDesC8& aValue)
	{
	LogL(KLogParameterTemplate, &aName, &aValue);
	}

void CSWICertStoreTool::RunToolL()
	{
	TRAPD(err, DoRunToolL());
	
	iCerts.ResetAndDestroy();
	iCerts.Close();
	delete iStore;
	delete iScript;

	if (err != KErrNone)
		{
		iFs.Delete(iOutputFile); // ignore errors
		User::Leave(err);
		}
	}

void CSWICertStoreTool::DoRunToolL()
	{
	LogL(KLogStartMessage);
	
	LogArgL(KParamInputFile, iInputFile);
	TRAP_AND_LOG(KActionOpenInputFile, OpenInputFileL(iInputFile));
	
	LogArgL(KParamOutputFile, iOutputFile);
	TRAP_AND_LOG(KActionOpenOutputFile, OpenStoreL(iOutputFile));

	const RPointerArray<CTestConfigSection>& sections = iScript->Sections();

	for (TInt i = 0 ; i < sections.Count() ; ++i)
		{
		const CTestConfigSection& section = *sections[i];
		const TDesC8& name = section.SectionName();

		// Skip 'Defaults' section
		if (name == KDefaultsSection)
			{
			continue;
			}

		if (name.Length() == 0 || name.Length() > KMaxCertLabelLength)
			{
			LogL(KLogIllegalCertLabel, &name);
			User::Leave(KErrArgument);
			}
		
		LogL(KLogSectionName, &name);
		TRAP_AND_LOG(KActionParseSection, ParseScriptSectionL(section));
		}

	TStreamId infoStreamId = KNullStreamId;
	TRAP_AND_LOG(KActionWriteInfoStream, infoStreamId = WriteInfoStreamL());
	TRAP_AND_LOG(KActionWriteRootStream, WriteRootStreamL(infoStreamId));

	LogL(KLogEndMessage);
	}

void CSWICertStoreTool::ParseScriptSectionL(const CTestConfigSection& aSection)
	{
	// Get cert label
	const TDesC8& sectionName = aSection.SectionName();
	TCertLabel label;
	label.Copy(sectionName);
	LogParameterL(KParamLabel, aSection.SectionName());
	CheckCertLabelL(label);
	
	// Parse file
	const CTestConfigItem* item = aSection.Item(KItemFile);
	if (item == NULL)
		{
		LogL(KLogNoFileSpecifiedError);
		User::Leave(KErrNotFound);
		}
	const TDesC8& certFile = item->Value();
	LogParameterL(KItemFile, certFile);

	// Load certificate data
	HBufC8* certData = NULL;
	TRAP_AND_LOG(KActionLoadCertData, certData = LoadCertDataL(certFile));
	CleanupStack::PushL(certData);

	// Parse the certificate
	CX509Certificate* cert = NULL;
	TRAP_AND_LOG(KActionParseX509Cert, cert = CX509Certificate::NewL(*certData));
	CleanupStack::PushL(cert);

	// Write cert to store
	TStreamId dataStreamId = KNullStreamId;
	TRAP_AND_LOG(KActionWriteCertData, dataStreamId = WriteCertDataL(*certData));

	// parse additional meta data for the certificate
	TCertMetaInfo certMetaInfo;
	// Parse mandatory flag
	certMetaInfo.iIsMandatory = 0;
	const CTestConfigItem* mandatory = aSection.Item(KItemMandatory);
	if (mandatory != NULL)
		{
		const TDesC8& mandatoryValue = mandatory->Value();
		if (mandatoryValue.CompareF(KTrue) == KErrNone || mandatoryValue.CompareF(KString1) == KErrNone)
			{
			certMetaInfo.iIsMandatory = KTrueValue;
			}
		else if (mandatoryValue.CompareF(KFalse) == KErrNone || mandatoryValue.CompareF(KString0) == KErrNone)
			{
			certMetaInfo.iIsMandatory = KFalseValue;
			}
		else
			{
			LogL(KLogIllegalMandatoryValue, &mandatoryValue);
			User::Leave(KErrArgument);	
			}
		}
	LogParameterL(KItemMandatory, certMetaInfo.iIsMandatory ? KString1 : KString0);	
	
	// Parse system upgrade flag
	certMetaInfo.iIsSystemUpgrade = 0;
	const CTestConfigItem* sysUpgrade = aSection.Item(KItemSystemUpgrade);
	if (sysUpgrade != NULL)
		{
		const TDesC8& sysUpgradeValue = sysUpgrade->Value();
		if (sysUpgradeValue.CompareF(KTrue) == KErrNone || sysUpgradeValue.CompareF(KString1) == KErrNone)
			{
			certMetaInfo.iIsSystemUpgrade = KTrueValue;
			}
		else if (sysUpgradeValue.CompareF(KFalse) == KErrNone || sysUpgradeValue.CompareF(KString0) == KErrNone)
			{
			certMetaInfo.iIsSystemUpgrade = KFalseValue;
			}
		else
			{
			LogL(KLogIllegalSystemUpgradeValue, &sysUpgradeValue);
			User::Leave(KErrArgument);	
			}
		}
	LogParameterL(KItemSystemUpgrade, certMetaInfo.iIsSystemUpgrade ? KString1 : KString0);


	// Parse capabilities
	certMetaInfo.iCapabilities = ParseCapabilitiesL(aSection);

	// Parse applications
	RArray<TUid> applications;
	CleanupClosePushL(applications);
	ParseApplicationsL(aSection, applications);

	CSWICertStoreToolCert* certInfo = CSWICertStoreToolCert::NewLC(certData->Length(),
																   label,
																   iCertificateId++,
																   cert->KeyIdentifierL(),
																   applications,
																   dataStreamId,
																   certMetaInfo);
	User::LeaveIfError(iCerts.Append(certInfo));
	CleanupStack::Pop(certInfo);
	CleanupStack::PopAndDestroy(3, certData); // applications, cert, certData
	}

void CSWICertStoreTool::CheckCertLabelL(const TDesC& aLabel)
	{
	for (TInt i = 0 ; i < iCerts.Count() ; ++i)
		{
		if (iCerts[i]->Label().CompareF(aLabel) == 0)
			{
			LogL(KLogDuplicateCertLabel);
			User::Leave(KErrArgument);
			}
		}
	}

HBufC8* CSWICertStoreTool::LoadCertDataL(const TDesC8& aPath)
	{
	HBufC* aBuf = HBufC::NewLC(aPath.Length());
	TPtr path(aBuf->Des());
	CnvUtfConverter::ConvertToUnicodeFromUtf8(path,aPath);

	RFile file;
	User::LeaveIfError(file.Open(iFs, path, EFileRead | EFileShareExclusive));
	CleanupClosePushL(file);

	TInt size;
	User::LeaveIfError(file.Size(size));

	HBufC8* buf = HBufC8::NewLC(size);
	TPtr8 ptr = buf->Des();
	User::LeaveIfError(file.Read(ptr, size));
	
	CleanupStack::Pop(buf);
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(aBuf);
	return buf;
	}

TStreamId CSWICertStoreTool::WriteCertDataL(const TDesC8& aData)
	{
	RStoreWriteStream stream;
	TStreamId streamId = stream.CreateLC(*iStore);
	stream.WriteL(aData);
	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream);
	return streamId;
	}

TCapabilitySet CSWICertStoreTool::ParseCapabilitiesL(const CTestConfigSection& aSection)
	{
	TCapabilitySet capabilities;
	capabilities.SetEmpty();
	for (TInt index = 0 ; ; ++index)
		{
		const CTestConfigItem* item = aSection.Item(KItemCapability, index);
		if (item == NULL)
			{
			break;
			}
		const TDesC8& capName = item->Value();
		TCapability capability = ECapability_None;
		TRAPD(err, capability = ParseCapabilityNameL(capName));
		if (err != KErrNone)
			{
			LogL(KLogIllegalCapabilityValue, &item->Value());
			User::Leave(KErrArgument);		
			}
		capabilities.AddCapability(capability);
		LogParameterL(KItemCapability, capName);
		}

	return capabilities;
	}

void CSWICertStoreTool::ParseApplicationsL(const CTestConfigSection& aSection, RArray<TUid>& aApplications)
	{
	for (TInt index = 0 ; ; ++index)
		{
		const CTestConfigItem* item = aSection.Item(KItemApplication, index);
		if (item == NULL)
			{
			break;
			}
		const TDesC8& appName = item->Value();
		TUid application;
		TRAPD(err, application = ParseApplicationNameL(appName));
		if (err != KErrNone)
			{
			LogL(KLogIllegalApplicationValue, &item->Value());
			User::Leave(KErrArgument);		
			}
		User::LeaveIfError(aApplications.Append(application));
		LogParameterL(KItemApplication, appName);
		}
	
	if (aApplications.Count() == 0)
		{
		LogL(KLogNoApplications);
		User::Leave(KErrArgument);
		}
	}

void CSWICertStoreTool::OpenInputFileL(const TDesC& aInputFile)
	{
	iScript = CTestConfig::NewLC(iFs, KSWICertStoreToolName, aInputFile);
	CleanupStack::Pop(iScript);
	}

void CSWICertStoreTool::OpenStoreL(const TDesC& aOutputFile)
	{
 	iStore = CPermanentFileStore::ReplaceL(iFs, aOutputFile, EFileRead | EFileWrite | EFileShareExclusive);
	iStore->SetTypeL(KPermanentFileStoreLayoutUid);
	}

TStreamId CSWICertStoreTool::WriteInfoStreamL()
	{
	// Create info stream
	RStoreWriteStream infoStream;
	TStreamId streamId = infoStream.CreateLC(*iStore);

	infoStream.WriteInt32L(iCerts.Count());
	for (TInt i = 0 ; i < iCerts.Count() ; ++i)
		{
		iCerts[i]->ExternalizeL(infoStream);
		}
	
	// Commit info stream
	infoStream.CommitL();
	CleanupStack::PopAndDestroy(&infoStream);

	return streamId;
	}

void CSWICertStoreTool::WriteRootStreamL(TStreamId aInfoStreamId)
	{
	// Create root stream - just contains id of info stream
	ASSERT(aInfoStreamId != KNullStreamId);
	RStoreWriteStream rootStream;
	TStreamId rootStreamId = rootStream.CreateLC(*iStore);
	iStore->SetRootL(rootStreamId);
	rootStream << aInfoStreamId;
	rootStream.CommitL();
	CleanupStack::PopAndDestroy(&rootStream);
	iStore->Commit();
	}

TUid CSWICertStoreTool::ParseApplicationNameL(const TDesC8& aName)
	{
	TUid result = KNullUid;
	if (aName == KApplicationSWInstall)
		{
		result = KApplicationSWInstallUid;
		}
	else if (aName == KApplicationSWInstallOCSP)
		{
		result = KApplicationSWInstallOCSPUid;
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	return result;
	}

TCapability CSWICertStoreTool::ParseCapabilityNameL(const TDesC8& aName)
	{
	TCapability result = ECapability_None;

	if (aName == KCapabilityTCB) 					result = ECapabilityTCB;
	else if (aName == KCapabilityCommDD) 			result = ECapabilityCommDD;
	else if (aName == KCapabilityPowerMgmt) 		result = ECapabilityPowerMgmt;
	else if (aName == KCapabilityMultimediaDD) 		result = ECapabilityMultimediaDD;
	else if (aName == KCapabilityReadDeviceData) 	result = ECapabilityReadDeviceData;
	else if (aName == KCapabilityWriteDeviceData) 	result = ECapabilityWriteDeviceData;
	else if (aName == KCapabilityDRM) 				result = ECapabilityDRM;
	else if (aName == KCapabilityTrustedUI) 		result = ECapabilityTrustedUI;
	else if (aName == KCapabilityProtServ) 			result = ECapabilityProtServ;
	else if (aName == KCapabilityDiskAdmin) 		result = ECapabilityDiskAdmin;
	else if (aName == KCapabilityNetworkControl) 	result = ECapabilityNetworkControl;
	else if (aName == KCapabilityAllFiles) 			result = ECapabilityAllFiles;
	else if (aName == KCapabilitySwEvent) 			result = ECapabilitySwEvent;
	else if (aName == KCapabilityNetworkServices) 	result = ECapabilityNetworkServices;
	else if (aName == KCapabilityLocalServices) 	result = ECapabilityLocalServices;
	else if (aName == KCapabilityReadUserData) 		result = ECapabilityReadUserData;
	else if (aName == KCapabilityWriteUserData) 	result = ECapabilityWriteUserData;
	else if (aName == KCapabilityLocation) 			result = ECapabilityLocation;
	else if (aName == KCapabilitySurroundingsDD) 	result = ECapabilitySurroundingsDD;
	else if (aName == KCapabilityUserEnvironment) 	result = ECapabilityUserEnvironment;

	if (result == ECapability_None)
		{
		User::Leave(KErrArgument);
		}
	
	return result;
	}

