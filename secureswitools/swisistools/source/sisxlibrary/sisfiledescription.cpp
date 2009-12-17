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
*
*/


/**
 @file 
 @internalComponent
 @released
*/
#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER
//
// Note: This file may contain code to generate corrupt files for test purposes.
// Such code is excluded from production builds by use of compiler defines;
// it is recommended that such code should be removed if this code is ever published publicly.
//

#include "sisfiledescription.h"
#include "sisdataunit.h"
#include <string>
#include "utility_interface.h"
#include "siscontents.h"

static std::wstring aOption1;
// This table must remain sorted by identifier name, since the search routine
// assumes that it is sorted.
static const SIdentifierTable KOptions [] = 
	{
		{	L"FA",					CSISFileDescription::EOpText,		CSISFileDescription::EInstFileTextOptionForceAbort	},
		{	L"FF",					CSISFileDescription::EOpInstall,	CSISFileDescription::EInstFileNone	},
		{	L"FILE",				CSISFileDescription::EOpInstall,	CSISFileDescription::EInstFileNone	},
		{	L"FILEMIME",			CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionByMimeType	},
		{	L"FILENULL",			CSISFileDescription::EOpNull,		CSISFileDescription::EInstFileNone	},
		{	L"FILERUN",				CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileNone	},
		{	L"FILETEXT",			CSISFileDescription::EOpText,		CSISFileDescription::EInstFileNone	},
		{	L"FM",					CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionByMimeType	},
		{	L"FN",					CSISFileDescription::EOpNull,		CSISFileDescription::EInstFileNone	},
		{	L"FORCEABORT",			CSISFileDescription::EOpText,		CSISFileDescription::EInstFileTextOptionForceAbort	},	
		{	L"FR",					CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileNone	},
		{	L"FT",					CSISFileDescription::EOpText,		CSISFileDescription::EInstFileNone	},
		{	L"MF",					0,	0			},
		{	L"MODIFIABLE",			0,	0			},
		{	L"RA",					CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionAfterInstall },
		{	L"RB",					CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionInstall | CSISFileDescription::EInstFileRunOptionUninstall	},
		{	L"RBS",					CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionBeforeShutdown	},
		{	L"RI",					CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionInstall		},
		{	L"RR",					CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionUninstall	},
		{	L"RS",					CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionSendEnd		},
		{	L"RUNAFTERINSTALL",		CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionAfterInstall },
		{	L"RUNBEFORESHUTDOWN",	CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionBeforeShutdown	},
		{	L"RUNBOTH",				CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionInstall | CSISFileDescription::EInstFileRunOptionUninstall	},
		{	L"RUNINSTALL",			CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionInstall		},
		{	L"RUNREMOVE",			CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionUninstall	},
		{	L"RUNSENDEND",			CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionSendEnd		},
		{	L"RUNWAITEND",			CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionWaitEnd		},
		{	L"RW",					CSISFileDescription::EOpRun,		CSISFileDescription::EInstFileRunOptionWaitEnd		},
		{	L"TA",					CSISFileDescription::EOpText,		CSISFileDescription::EInstFileTextOptionAbortIfNo	},
		{	L"TC",					CSISFileDescription::EOpText,		CSISFileDescription::EInstFileNone	},
		{	L"TE",					CSISFileDescription::EOpText,		CSISFileDescription::EInstFileTextOptionExitIfNo	},
		{	L"TEXTABORT",			CSISFileDescription::EOpText,		CSISFileDescription::EInstFileTextOptionAbortIfNo	},
		{	L"TEXTCONTINUE",		CSISFileDescription::EOpText,		CSISFileDescription::EInstFileNone	},
		{	L"TEXTEXIT",			CSISFileDescription::EOpText,		CSISFileDescription::EInstFileTextOptionExitIfNo	},
		{	L"TEXTSKIP",			CSISFileDescription::EOpText,		CSISFileDescription::EInstFileTextOptionSkipIfNo	},
		{	L"TS",					CSISFileDescription::EOpText,		CSISFileDescription::EInstFileTextOptionSkipIfNo	},
		{	L"VERIFY",				CSISFileDescription::EOpInstall,	CSISFileDescription::EInstVerifyOnRestore			},
		{	L"VR",					CSISFileDescription::EOpInstall,	CSISFileDescription::EInstVerifyOnRestore			},
		{	NULL,					0,									0	}

	};

static const SKeyword1 KEscSymbols [] = 
	{
		{ L"?"},
		{ L"*"},
		{ L"\""},
		{ L":"},
		{ L"|"},
		{ L"/"},
		{L"<"},
		{L">"},
		{NULL}
	};

void CSISFileDescription::InsertMembers ()
	{
	InsertMember (iTarget);
	InsertMember (iMimeType);
	InsertMember (iCapabilities);
	InsertMember (iHash);
	InsertMember (iOperation);
	InsertMember (iOperationOptions);
	InsertMember (iLength);
	InsertMember (iUncompressedLength);
	InsertMember (iFileIndex);
	}


CSISFileDescription::CSISFileDescription (const CSISFileDescription& aInitialiser)	:
		CStructure <CSISFieldRoot::ESISFileDescription> (aInitialiser),
		iTarget (aInitialiser.iTarget),
		iMimeType (aInitialiser.iMimeType),
		iCapabilities (aInitialiser.iCapabilities),
		iOperation (aInitialiser.iOperation),
		iOperationOptions (aInitialiser.iOperationOptions),
		iHash (aInitialiser.iHash),
		iLength (aInitialiser.iLength),
		iUncompressedLength (aInitialiser.iUncompressedLength),
		iFileIndex (aInitialiser.iFileIndex)
	{ 
	InsertMembers (); 
	}

CSISFileDescription::TSISInstOption CSISFileDescription::InterpretOption (const std::wstring& aOption)
	{
	aOption1 = aOption; 
	int index = SearchSortedUCTable (KOptions, aOption);
	CSISException::ThrowIf (index < 0, CSISException::ESyntax, L"unknown option: " + aOption);
	if (KOptions [index].iDeprecated)
		{
		SISLogger::Log(L"Option ");
		SISLogger::Log(aOption);
		SISLogger::Log(L" ignored.\n");
		}
	CSISException::ThrowIf ((iOperation > EOpNone) && (iOperation != KOptions [index].iValue),
							CSISException::ESyntax,
							L"incompatible option: " + aOption);
	iOperation |= KOptions [index].iValue;
	iOperationOptions |= KOptions [index].iSubValue;
    
    
    // in order to be approximately backwards compatible - RUNAFTERINSTALL implies RUNINSTALL. SWI
	// will run after install if it understands the new flag, or at install if it doesn't.
    if (KOptions [index].iSubValue &  CSISFileDescription::EInstFileRunOptionAfterInstall)
        {
        iOperationOptions |= CSISFileDescription::EInstFileRunOptionInstall;
        }
    
	return static_cast <CSISFileDescription::TSISInstOption> (iOperationOptions.Value ());
	}

void CSISFileDescription::MakeNeat ()
	{
	
	CStructure <CSISFieldRoot::ESISFileDescription>::MakeNeat ();
		
	std::wstring strTemp = iTarget.GetString();

	// The hashes of files in \sys and \resource MUST be checked at restore time.
	// N.B. SWI checks the hashes of these files regardless of the value of this flag. This
	// is just set for consistency.
	if ((strTemp.find(L"\\sys\\",0) == 2) || (strTemp.find(L"\\resource\\",0) == 2))
		{
		iOperationOptions = iOperationOptions |= EInstVerifyOnRestore;
		}	

	// If the package file specifies run by mime type but doesn't specify whether
	// it should be run on install or uninstall, then default to install as per
	// the old installer.
	if ((iOperationOptions & EInstFileRunOptionByMimeType) &&
		! (iOperationOptions & (EInstFileRunOptionInstall | EInstFileRunOptionUninstall)))
		{
		iOperationOptions |= EInstFileRunOptionInstall;
		}


	}

void CSISFileDescription::Verify (const TUint32 aLanguages) const
	{
	CStructure <CSISFieldRoot::ESISFileDescription>::Verify (aLanguages);
	CSISException::ThrowIf (iOperation >= EOpIllegal, CSISException::EVerification, "unsupported installation operation");
	CSISException::ThrowIf (((iOperation & EOpInstall) != 0) && (iTarget.size () == 0),
							CSISException::EVerification,
							"installation requires a target file");
	CSISException::ThrowIf (((iOperationOptions & EInstFileRunOptionByMimeType) != 0) && (iMimeType.size () == 0),
							CSISException::EVerification,
							"run by MIME Type requires a MIME type");
	std::wstring destinationFile (iTarget.GetString());

	if (iTarget.size () > 0)
		{
		if(destinationFile[1] != ':')
			{
			throw CSISException (CSISException::EInvalidDestination, "Invalid destination path check : is missing");		
			}
		
		if(destinationFile[2] != '\\')
			{
			throw CSISException (CSISException::EInvalidDestination, "Invalid destination path check \\ is missing");		
			}
		
		// Check whether the first character is either an alphabet or '!' or '$'(represents system drive).
		if(!(isalpha(destinationFile[0])) && (destinationFile[0] !='!') && (destinationFile[0] != SYSTEMDRIVE ))
			{
			throw CSISException (CSISException::EInvalidDestination, "Invalid destination path check drive or ! is missing");					
			}
			
		// Check for double dot paths in filename 
			
		CSISException::ThrowIf ((-1 != destinationFile.find(L"\\..\\")),
			CSISException::EInvalidDestination, "File paths may not contain '..'");	
		
		unsigned int temp1 = destinationFile.find_last_of(L"\\" ,iTarget.size ());
		std::wstring destinationFileName(destinationFile.substr(temp1 + 1,iTarget.size ()));
		
		// Check if install-file (exe or dll)  contain non ASCII characters
		int size = iTarget.size();
		
		if(iTarget.size() >= 10)
			{
			std::wstring destinationFilePath (destinationFile.substr(0,10));
			// Check for :\sys\bin after converting array in uppercase
			for(int i = 1; i <= 9; i++)
				{
				destinationFilePath[i] = toupper(destinationFilePath[i]);
				}
			if(destinationFilePath.find(L":\\SYS\\BIN") != std::string::npos )
				{
				//For ROM stub file , if makesis encounters a wildcard in the file under \\sys\\bin directoy,
				//throw a warning as described below
				if (CSISContents::IsStub() && ((destinationFileName.find(L"*") != std::string::npos) || (destinationFileName.find(L"?") != std::string::npos)))
					{
					//iWarningFlag is added so that makesis can generate the warning at the first occurance of wildcard,
					// if there are many executables in the ROM stub sis file with wildcards.
					
					static bool aWarningFlag = false;
					if (!aWarningFlag)
						{
						SISLogger::Log(L"Warning: Executables should be included explicitly (without wildcards),to enable SID checks to work as expected.\n");
						aWarningFlag = true;
						}
					}

				int temp = 0;
				for (int x = 0; x< iTarget.size () ;x++)
					{
					if(destinationFile[x] == '\\')
					temp = x;
					}
				for(x = temp; x < iTarget.size (); x++)
					{
					if(!(isascii(destinationFile[x])))
						{
						throw CSISException (CSISException::EInvalidDestination, "File name contain non ascii characters");		
						}
					}
				}
			}

		std::wstring destinationFile1 (destinationFile.substr(2,temp1-1));
		const SKeyword1* index = NULL; 
		for (index = &KEscSymbols[0]; index -> iName != NULL ; index++)
			{
			for (int x = 0; x <= temp1-2 ;x++)
				{
				if ((destinationFile1.find(index -> iName) != std::string::npos)) 
					{
					throw CSISException (CSISException::EInvalidDestination, "File paths may not contain \"<>'/");		
					}
				}
			}
		
				
		for (index = &KEscSymbols[0]; index -> iName != NULL ; index++)
			{
			for (int x = 0; x< (iTarget.size() - (temp1+1)) ;x++)
				{
				if (destinationFileName.find(index -> iName) != std::string::npos) 
					{
#ifdef SIGNSIS
					if(index -> iName == (L"*"))
						{
						continue;
						}
#else
					// Allow wildchars in ROM stub sis files
					if (CSISContents::IsStub() && ((index -> iName == (L"*")) || (index -> iName == (L"?"))) )
						{
						continue;
						}	
					// Allow * for FN case, ex: ""-"c:\*", FN
					else if((index -> iName == (L"*")) && (iOperation & EOpNull))
						{
						continue;
						}
#endif
					else
						{
						throw CSISException (CSISException::EInvalidDestination, "File paths may not contain \"<>'/");
						break;
						}
					}
				}
			}
		}
	int nNo = 0;
	if ((iOperationOptions & EInstFileTextOptionSkipIfNo) != 0) 
		{
		nNo++;
		}
	if ((iOperationOptions & EInstFileTextOptionAbortIfNo) != 0) 
		{
		nNo++;
		}
	if ((iOperationOptions & EInstFileTextOptionExitIfNo) != 0) 
		{
		nNo++;
		}
	if ((iOperationOptions & EInstFileTextOptionForceAbort) != 0) 
		{
		nNo++;
		}
	
	CSISException::ThrowIf (nNo > 1,
							CSISException::EVerification,
							"Skip If No, Abort If No, Exit If No and ForceAbort options incompatible");
	CSISException::ThrowIf (((iOperationOptions & EInstFileRunOptionWaitEnd) != 0) && ((iOperationOptions & EInstFileRunOptionSendEnd) != 0),
							CSISException::EVerification,
							"Wait End and Send End are incompatible");
	}



std::string CSISFileDescription::Name () const
	{
	return "File Description";
	}

void CSISFileDescription::SetHash(const TUint8* aHash, TUint32 aHashSize)
	{
	iHash.SetHash(aHash, aHashSize);
	}

void CSISFileDescription::SetHash (const CSISHash& aHash)
	{
	iHash = aHash;
	}

void CSISFileDescription::ExtractCapabilities(const std::wstring& aFileName)
	{
#ifdef GENERATE_ERRORS
	if (CSISFieldRoot::IsBugToBeCreated(CSISFieldRoot::EBugEmptyCaps))
		{
		// don't store exe capabilities in the controller
		return;
		}
#endif 
	iCapabilities.ExtractCapabilities(aFileName);
	
	}

void CSISFileDescription::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	aStream <<L";";
	iTarget.AddPackageEntry(aStream, aVerbose);
	aStream << std::endl << L"; File length " << iLength << L" (" << iUncompressedLength << L")" << std::endl;
	iCapabilities.AddPackageEntry(aStream, aVerbose);
	iHash.AddPackageEntry(aStream, aVerbose);
	
	const wchar_t* fileName = GetFileName(); 
	
	aStream <<std::endl <<L"\"" << fileName << L"\"";
	delete[] const_cast<wchar_t*>(fileName);
	aStream << L"-";
	aStream << L"\"";
	iTarget.AddPackageEntry(aStream, aVerbose);
	aStream << L"\"";

	TUint32 operation = (TUint32) iOperation;
	TUint32 options = (TUint32) iOperationOptions;

	if (((operation & EOpInstall) || (operation & EOpRun)) && (options & EInstVerifyOnRestore))
		{
		options &= ~EInstVerifyOnRestore;
		aStream << L", " << (aVerbose?L"VERIFY":L"VR");
		}			

	if (operation & EOpInstall)
		{
		operation &= ~EOpInstall;
		aStream << L", " << (aVerbose?L"FILE":L"FF");
		}

	if (operation & EOpRun)
		{
		operation &= ~EOpRun;
		if (options & EInstFileRunOptionByMimeType)
			{
			options &= ~EInstFileRunOptionByMimeType;
			aStream << L", " << (aVerbose?L"FILEMIME, ":L"FM,\" ");
			iMimeType.AddPackageEntry(aStream, aVerbose);
			aStream<<L"\"";
			}
		else
			{
			aStream << L", " << (aVerbose?L"FILERUN":L"FR");
			}
		if (options & (EInstFileRunOptionInstall&EInstFileRunOptionUninstall))
			{
			options &= ~(EInstFileRunOptionInstall&EInstFileRunOptionUninstall);
			aStream << L", " << (aVerbose?L"RUNBOTH":L"RB");
			}
		if (options & EInstFileRunOptionInstall)
			{
			options &= ~EInstFileRunOptionInstall;
			aStream << L", " << (aVerbose?L"RUNINSTALL":L"RI");
			}
		if (options & EInstFileRunOptionUninstall)
			{
			options &= ~EInstFileRunOptionUninstall;
			aStream << L", " << (aVerbose?L"RUNREMOVE":L"RR");
			}
		if (options & EInstFileRunOptionBeforeShutdown)
			{
			options &= ~EInstFileRunOptionBeforeShutdown;
			aStream << L", " << (aVerbose?L"RUNBEFORESHUTDOWN":L"RBS");
			}
		if (options & EInstFileRunOptionWaitEnd)
			{
			options &= ~EInstFileRunOptionWaitEnd;
			aStream << L", " << (aVerbose?L"RUNWAITEND":L"RW");
			}
		if (options & EInstFileRunOptionSendEnd)
			{
			options &= ~EInstFileRunOptionSendEnd;
			aStream << L", " << (aVerbose?L"RUNSENDEND":L"RS");
			}
		if (options & EInstFileRunOptionAfterInstall)
			{
			options &= ~EInstFileRunOptionAfterInstall;
			aStream << L", " << (aVerbose?L"RUNAFTERINSTALL":L"RA");
			}
		}
	if (operation & EOpText)
		{
		operation &= ~EOpText;
		aStream << L", " << (aVerbose?L"FILETEXT":L"FT");
		if (options & EInstFileTextOptionContinue)
			{
			options &= ~EInstFileTextOptionContinue;
			aStream << L", " << (aVerbose?L"TEXTCONTINUE":L"TC");
			}
		if (options & EInstFileTextOptionSkipIfNo)
			{
			options &= ~EInstFileTextOptionSkipIfNo;
			aStream << L", " << (aVerbose?L"TEXTSKIP":L"TS");
			}
		if (options & EInstFileTextOptionAbortIfNo)
			{
			options &= ~EInstFileTextOptionAbortIfNo;
			aStream << L", " << (aVerbose?L"TEXTABORT":L"TA");
			}
		if (options & EInstFileTextOptionExitIfNo)
			{
			options &= ~EInstFileTextOptionExitIfNo;
			aStream << L", " << (aVerbose?L"TEXTEXIT":L"TE");
			}
		if (options & EInstFileTextOptionForceAbort)
			{
			options &= ~EInstFileTextOptionForceAbort;
			aStream << L", " << (aVerbose?L"FORCEABORT":L"FA");
			}
		}
	if (operation & EOpNull) 
		{ 
		operation &=~EOpNull; 
		aStream << L", " << (aVerbose?L"FILENULL":L"FN"); 
		} 

	aStream << std::endl;
	if (aVerbose && operation)
		{
		aStream << L"; (warning: unparsed operation(s) on preceding line" << std::endl;
		}
	if (aVerbose && options)
		{
		aStream << L"; (warning: unparsed option(s) on preceding line" << std::endl;
		}
	}

const wchar_t* CSISFileDescription::GetFileName() const
	{
	// Using actual iFileIndex rather than iFileNum because source file might be
	// the same for several targets and hence has an index different from simple
	// sequentual ordinal value
	wchar_t* fileName = new wchar_t[30];
#ifdef _MSC_VER
	swprintf(fileName, L"file%d", iFileIndex.Value());
#else
	swprintf(fileName, 30, L"file%d", iFileIndex.Value());
#endif //_MSC_VER
	return fileName;
	}


#ifdef GENERATE_ERRORS
void CSISFileDescription::CreateDefects ()
	{
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iOperation = rand ();
		}
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iOperationOptions = rand ();
		}
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iFileIndex = rand ();
		}
	}
#endif // GENERATE_ERRORS
