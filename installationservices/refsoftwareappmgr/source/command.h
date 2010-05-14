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
* This file defines a parser and console commands for the reference Application Manager
*
*/


/**
 @file
 @internalComponent
 exampleCode
*/

#ifndef COMMAND_H
#define COMAMND_H

#include <e32base.h>
#include <f32file.h>
#include <bacline.h>

namespace Usif
	{
	class CComponentFilter;

	/**
	The CConsoleCommand class defines a pure virtual interface for console commands.
	Each software management operation provided by the Reference Application Manager
	must implement this interface.
	*/
	class CConsoleCommand: public CBase
		{
	public:
		/**
		Returns the name of the command. The CCommandLineParser class uses this method
		to find an appropriate command object whilst parsing the console input.
		*/
		virtual const TDesC& Name() = 0;

		/**
		CCommandLineParser uses this method to hand over the recognition of command's options
		to an appropriate command object.
		
		@param aName The name of an option.
		@param aValues The values of option's parameters.
		*/
		virtual void OptionHandlerL(const TPtrC& aName, const RArray<TPtrC>& aValues) = 0;

		/**
		CCommandLineParser uses this method to execute a command.
		
		@param aConsole The console the command writes its output to.
		*/
		virtual void ExecuteL(CConsoleBase& aConsole) = 0;

		virtual ~CConsoleCommand();

	protected:
		CConsoleCommand();
		CConsoleCommand(const CConsoleCommand&);
		CConsoleCommand& operator=(const CConsoleCommand&);
		};

	/**
	A 'list' command. It displays to the user a list of installed software components.
	Please @see appmanager.cpp for the list of supported filtering options.
	*/
	class CListCommand: public CConsoleCommand
		{
	public:
		static CListCommand* NewLC();

		// Implement CConsoleCommand
		virtual const TDesC& Name();
		virtual void OptionHandlerL(const TPtrC& aName, const RArray<TPtrC>& aValues);
		virtual void ExecuteL(CConsoleBase& aConsole);
		virtual ~CListCommand();

	private:
		CListCommand();
		CComponentFilter* iFilter;
		TLanguage iLocale;
		TBool iScomoStateSet;
		};

	/**
	A 'delete' command. It deletes a component specified by id from the system. The id of a component
	to be deleted can be retrieved with the aid of the 'list' command.
	*/
	class CDeleteCommand: public CConsoleCommand
		{
	public:
		static CDeleteCommand* NewLC();

		// Implement CConsoleCommand
		virtual const TDesC& Name();
		virtual void OptionHandlerL(const TPtrC& aName, const RArray<TPtrC>& aValues);
		virtual void ExecuteL(CConsoleBase& aConsole);
		virtual ~CDeleteCommand();

	private:
		CDeleteCommand();
		
		enum { EInvalidComponentId = -1 };
		TInt iComponentId;
		};

	/**
	A user input parser. The Reference Application Manager uses this class to recognize
	and execute console commands entred by the user. A command must be registered with
	the RegisterCommandL() method so the parser may use it.
	The parser uses the CCommandLineArguments class to read user input from the console.
	*/
	class CCommandLineParser: public CBase
		{
	public:
		/**
		Creates a new instance of the Parser.
		*/
		static CCommandLineParser* NewLC();

		/**
		Registers a command to the parser.
		
		@param aCommand A command to be registered.
		*/
		void RegisterCommandL(CConsoleCommand& aCommand);

		/**
		Parses user input from the console and returns a command that has been found.
		*/
		CConsoleCommand& ParseL();

		virtual ~CCommandLineParser();

	private:
		CCommandLineParser();
		CCommandLineParser(const CCommandLineParser&);
		CCommandLineParser& operator=(const CCommandLineParser&);

		CCommandLineArguments* iCmdLineArgs;
		RPointerArray<CConsoleCommand> iCommands;
		};
	}

#endif //  COMAMND_H
