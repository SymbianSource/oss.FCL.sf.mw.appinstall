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


#pragma warning (disable: 4786)

#include <iostream>
#include <memory>

#include "interpretsis.h"
#include "uninstaller.h"
#include "parameterlist.h"

CInterpretSIS::CInterpretSIS(TParamPtr aParamList):
				iParamList(aParamList)
	{
	ConstructL();
	}

CInterpretSIS::~CInterpretSIS()
	{
	delete iInstaller;
	delete iSisRegistry;
	delete iConfigManager;
	}

void CInterpretSIS::ConstructL()
	{	
	iParamList->ValidateParam();
	iRomManager = TRomManagerPtr(RomManager::New(*iParamList));
	iConfigManager = new ConfigManager(*iParamList);
	iSisRegistry = new SisRegistry( *iParamList, *iRomManager, *iConfigManager);
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Validate the mutual exclusion of reg file version and origin verification status specification.
	// This is done here because reg file can be specified via the file (z:\system\data\sisregistry_*.*.txt) 
	// during SisRegistry object creation.
	if (iParamList->RegistryVersionExists() && !(iParamList->OriginVerificationStatus()))
		{
		// Both reg file version(either via -k or via file) and -f options are mutually exclusive
		throw CParameterList::EParamRegVersionOriginVerificationExclusive;
		}
	#endif
	
	iInstaller = new Installer(*iSisRegistry, *iParamList, *iRomManager, *iConfigManager);
	
	// And ensure that any language code specified via the command line
	// overrides a value in the config file.
	if (iParamList->IsFlagSet(CParameterList::EFlagsLanguageWasSet))
		{
		iConfigManager->SetValue( KVariableLanguage, iParamList->Language());
		}
	}

int CInterpretSIS::Install()
	{
	int retValue = 0;
	if (iParamList->FileNames().empty())
		{
		return retValue;
		}
	
	// There is at least one sis file to be installed
	const CParameterList::SISFileList& files = iParamList->FileNames();

	// Install the sis files
	retValue = iInstaller->Install(files);

	if (!iInstaller->GetMissing().empty())
		{
		const Installer::MissingDeps& missing =	iInstaller->GetMissing();

		for (Installer::MissingDeps::const_iterator dependenciesIt = missing.begin();
			 dependenciesIt != missing.end();
			 ++dependenciesIt)
			{
			if (!dependenciesIt->second.empty())
				{
				LERROR(L"Package " << dependenciesIt->first.c_str() << L" requires:");
				for (std::vector<SisRegistryDependency>::const_iterator dependantsIt = dependenciesIt->second.begin();
					dependantsIt != dependenciesIt->second.end() ;
					++dependantsIt)
					{
					LERROR(L"\t" << *dependantsIt);
					}
				}
			}
		}
	return retValue;
	}

void CInterpretSIS::Uninstall()
	{
	// Check if uninstallation is required or not
	if (!iParamList->PkgUidsToRemove().empty())
		{
		// Uninstall the sis files
		Uninstaller uninstall = Uninstaller(*iSisRegistry, *iParamList);
		uninstall.Uninstall();
		}
	}

const SisRegistry* CInterpretSIS::GetSisRegistry() const
	{
	return iSisRegistry;
	}
// End of File
