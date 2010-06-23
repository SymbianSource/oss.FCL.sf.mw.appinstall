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


#include <usif/scr/scr.h>
#include <usif/sif/sif.h>
#include <e32cons.h>
#include "command.h"

using namespace Usif;

_LIT(KTxtOptionPrefix, "--");
_LIT(KTxtDeleteCommand, "delete");
_LIT(KTxtListCommand, "list");
_LIT(KTxtListOptName, "name");
_LIT(KTxtListOptVendor, "vendor");
_LIT(KTxtListOptType, "type");
_LIT(KTxtListOptActivated, "activated");
_LIT(KTxtListOptDeactivated, "deactivated");
_LIT(KTxtListOptRemovable, "removable");
_LIT(KTxtListOptProperty, "property");
_LIT(KTxtListOptIntProperty, "intproperty");
_LIT(KTxtListOptDrives, "drives");
_LIT(KTxtListOptLocale, "locale");
_LIT(KTxtActivated, "Activated");
_LIT(KTxtDeactivated, "Deactivated");
_LIT(KTxtPressToContinue, "\nPress any key to continue...");
_LIT(KTxtSifConnectionFailure, "\nFailed to connect to the SIF server\n");
_LIT(KTxtDeleting, "Deleting...\n");

// ==================================================================================

CConsoleCommand::CConsoleCommand()
	{
	}

CConsoleCommand::~CConsoleCommand()
	{
	}

// ==================================================================================

CListCommand* CListCommand::NewLC()
	{

	CListCommand* self = new (ELeave) CListCommand;
	CleanupStack::PushL(self);
	self->iFilter = CComponentFilter::NewL();
	return self;
	}

CListCommand::CListCommand(): iLocale(KUnspecifiedLocale)
	{
	}

CListCommand::~CListCommand()
	{
	delete iFilter;
	}

const TDesC& CListCommand::Name()
	{
	return KTxtListCommand;
	}

namespace
	{
	// Leave if the number of values differs from aCount
	void AssertNumValuesL(const RArray<TPtrC>& aValues, TInt aCount)
		{
		if (aValues.Count() != aCount)
			{
			User::Leave(KErrArgument);
			}
		}
	}

void CListCommand::OptionHandlerL(const TPtrC& aName, const RArray<TPtrC>& aValues)
	{
	// Configure the component query filter:
	
	// Component name
	if (aName == KTxtListOptName)
		{
		AssertNumValuesL(aValues, 1);
		iFilter->SetNameL(aValues[0]);
		}

	// Vendor
	else if (aName == KTxtListOptVendor)
		{
		AssertNumValuesL(aValues, 1);
		iFilter->SetVendorL(aValues[0]);
		}

	// Software type
	else if (aName == KTxtListOptType)
		{
		AssertNumValuesL(aValues, 1);
		iFilter->SetSoftwareTypeL(aValues[0]);
		}

	// SCOMO state - Activated
	else if (aName == KTxtListOptActivated)
		{
		AssertNumValuesL(aValues, 0);

		// The activated and deactivated options are mutually exclusive
		if (iScomoStateSet)
			{
			User::Leave(KErrArgument);
			}
		iScomoStateSet = ETrue;

		iFilter->SetScomoStateL(EActivated);
		}

	// SCOMO state - Deactivated
	else if (aName == KTxtListOptDeactivated)
		{
		AssertNumValuesL(aValues, 0);

		// The activated and deactivated options are mutually exclusive
		if (iScomoStateSet)
			{
			User::Leave(KErrArgument);
			}
		iScomoStateSet = ETrue;

		iFilter->SetScomoStateL(EDeactivated);
		}

	// Removable
	else if (aName == KTxtListOptRemovable)
		{
		AssertNumValuesL(aValues, 0);
		iFilter->SetRemovable(ETrue);
		}

	// String property
	else if (aName == KTxtListOptProperty)
		{
		const TInt numValues = aValues.Count();
		if (numValues < 2 || numValues > 3)
			{
			User::Leave(KErrArgument);
			}

		// Convert language from string to int if specified
		TInt language = KUnspecifiedLocale;
		if (numValues == 3)
			{
			TLex lex(aValues[2]);
			if (lex.Val(language) != KErrNone || language < ELangEnglish || language >= ELangMaximum)
				{
				User::Leave(KErrArgument);
				}
			}

		// Add the property to the filter
		iFilter->AddPropertyL(aValues[0], aValues[1], static_cast<TLanguage>(language));
		}

	// Integer property
	else if (aName == KTxtListOptIntProperty)
		{
		AssertNumValuesL(aValues, 2);
		TLex lex(aValues[1]);
		TInt64 val;
		if (lex.Val(val) != KErrNone)
			{
			User::Leave(KErrArgument);
			}
		iFilter->AddPropertyL(aValues[0], val);
		}

	// Drives
	else if (aName == KTxtListOptDrives)
		{
 	 	AssertNumValuesL(aValues, 1);
 	 	TPtrC drives = aValues[0];
 	 	TDriveList driveList;
 	 	driveList.FillZ(KMaxDrives);
 	 	for (TInt i=0; i<drives.Length(); ++i)
 	 	   	{
 	 	  	TChar driveLetter = static_cast<TChar>(drives[i]);
 	 	   	TInt driveNumber = 0;
 	 	   	User::LeaveIfError(RFs::CharToDrive(driveLetter, driveNumber));
 	 	   	driveList[driveNumber] = ETrue;
 	 	   	}
 	 	iFilter->SetInstalledDrivesL(driveList);
		}

	// Locale
	else if (aName == KTxtListOptLocale)
		{
		AssertNumValuesL(aValues, 1);
		TLex lex(aValues[0]);
		TInt val;
		if (lex.Val(val) != KErrNone || val < ELangEnglish || val >= ELangNone)
			{
			User::Leave(KErrArgument);
			}
		iLocale = static_cast<TLanguage>(val);
		}
	
	// Unknown
	else
		{
		User::Leave(KErrArgument);
		}
	}

void CListCommand::ExecuteL(CConsoleBase& aConsole)
	{
	// Connect to the SCR server
	RSoftwareComponentRegistry scr;
	TInt err = scr.Connect();
	if (err != KErrNone)
		{
		aConsole.Printf(_L("\nFailed to connect to the SCR server"));
		User::Leave(err);
		}
	CleanupClosePushL(scr);

	// Create an SCR view
	RSoftwareComponentRegistryView scrView;
	scrView.OpenViewL(scr, iFilter);
	CleanupClosePushL(scrView);

	// Iterate over the matching components
	CComponentEntry* entry = CComponentEntry::NewLC();
	TBool first(ETrue);
	while (scrView.NextComponentL(*entry, iLocale))
		{
		if (first)
			{
			first = EFalse;
			}
		else
			{
			aConsole.Printf(KTxtPressToContinue);
			aConsole.Getch();
			aConsole.ClearScreen();
			}

		aConsole.Printf(_L("\n============= Component Info =============\n"));
		aConsole.Printf(_L("\nComponent ID     : %d"), entry->ComponentId());
		aConsole.Printf(_L("\nComponent name   : %S"), &entry->Name());
		aConsole.Printf(_L("\nVendor name      : %S"), &entry->Vendor());
		aConsole.Printf(_L("\nSoftware type    : %S"), &entry->SoftwareType());
		aConsole.Printf(_L("\nSCOMO state      : %S"), entry->ScomoState() == EActivated ? &KTxtActivated : &KTxtDeactivated );
		aConsole.Printf(_L("\nComponent size   : %d"), entry->ComponentSize());
		aConsole.Printf(_L("\nInstalled drives : %S"), &entry->InstalledDrives());
		aConsole.Printf(_L("\nVersion          : %S"), &entry->Version());
		aConsole.Printf(_L("\n\n=========================================\n"));
		}

	// Disconnect from the SCR server and cleanup the entry
	CleanupStack::PopAndDestroy(3, &scr);
	}

// ==================================================================================

CDeleteCommand* CDeleteCommand::NewLC()
	{
	CDeleteCommand* self = new (ELeave) CDeleteCommand;
	CleanupStack::PushL(self);
	return self;
	}

CDeleteCommand::CDeleteCommand(): iComponentId(EInvalidComponentId)
	{
	}

CDeleteCommand::~CDeleteCommand()
	{
	}

const TDesC& CDeleteCommand::Name()
	{
	return KTxtDeleteCommand;
	}

void CDeleteCommand::OptionHandlerL(const TPtrC& aName, const RArray<TPtrC>& aValues)
	{
	// The delete command takes only the id of a component
	if (aName.Length() || aValues.Count() != 1)
		{
		User::Leave(KErrArgument);
		}

	// Convert the id of the component from string to int
	TLex lex(aValues[0]);
	if (lex.Val(iComponentId) != KErrNone)
		{
		User::Leave(KErrArgument);
		}
	}

void CDeleteCommand::ExecuteL(CConsoleBase& aConsole)
	{
	// Connect to the SIF server
	RSoftwareInstall sif;
	TInt err = sif.Connect();
	if (err != KErrNone)
		{
		aConsole.Printf(KTxtSifConnectionFailure);
		User::Leave(err);
		}
	CleanupClosePushL(sif);

	// Delete the component
	TRequestStatus status;
	sif.Uninstall(iComponentId, status);
	aConsole.Printf(KTxtDeleting);
	User::WaitForRequest(status);
	if (status.Int() != KErrNone)
		{
		User::Leave(status.Int());
		}

	// Disconnect from the SIF server
	CleanupStack::PopAndDestroy(&sif);
	}

// ==================================================================================

CCommandLineParser* CCommandLineParser::NewLC()
	{
	CCommandLineParser* self = new (ELeave) CCommandLineParser;
	CleanupStack::PushL(self);
	self->iCmdLineArgs = CCommandLineArguments::NewL();
	return self;
	}

CCommandLineParser::CCommandLineParser()
	{
	}

CCommandLineParser::~CCommandLineParser()
	{
	delete iCmdLineArgs;
	iCommands.Close();
	}
	
void CCommandLineParser::RegisterCommandL(CConsoleCommand& aCommand)
	{
	iCommands.Append(&aCommand);
	}

CConsoleCommand& CCommandLineParser::ParseL()
	{
	const TInt numArgs = iCmdLineArgs->Count();

	// There must be at least two arguments: the name of the program and the name of a command
	if (numArgs <= 1)
		{
		User::Leave(KErrArgument);
		}

	// Extract the name of the command
	TPtrC arg0 = iCmdLineArgs->Arg(1);
	if (arg0.Left(KTxtOptionPrefix.iTypeLength) != KTxtOptionPrefix)
		{
		User::Leave(KErrArgument);
		}
	TPtrC cmdName = arg0.Mid(KTxtOptionPrefix.iTypeLength);
	
	// Look for the command in iCommands
	CConsoleCommand* cmd = NULL;
	for (TInt i=0; i<iCommands.Count(); ++i)
		{
		if (cmdName == iCommands[i]->Name())
			{
			cmd = iCommands[i];
			}
		}
	if (cmd == NULL)
		{
		User::Leave(KErrArgument);
		}

	// Iterate over the command's options and build pairs of optName and optValues
	TPtrC optName;
	RArray<TPtrC> optValues;
	CleanupClosePushL(optValues);
	for (TInt i=2; i<numArgs; ++i)
		{
		TPtrC arg = iCmdLineArgs->Arg(i);
		// Option name
		if (arg.Left(KTxtOptionPrefix.iTypeLength) == KTxtOptionPrefix)
			{
			// Process the previous option
			if (optName.Length())
				{
				cmd->OptionHandlerL(optName, optValues);
				}

			// Set new option
			optName.Set(arg.Mid(KTxtOptionPrefix.iTypeLength));
			if (optName.Length() == 0)
				{
				User::Leave(KErrArgument);
				}
			optValues.Reset();
			}
		else
			{
			// Store the option's value
			optValues.AppendL(arg);
			}
		}

	// Process the last option
	if (optName.Length() || optValues.Count())
		{
		cmd->OptionHandlerL(optName, optValues);
		}

	CleanupStack::PopAndDestroy(&optValues);

	// Return the current command
	return *cmd;
	}
