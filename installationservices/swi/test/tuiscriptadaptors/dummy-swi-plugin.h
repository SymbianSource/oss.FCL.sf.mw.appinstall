/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @internalTechnology
* @test
*
*/


#ifndef DUMMY_SWI_PLUGIN_H
#define DUMMY_SWI_PLUGIN_H
#include <swidaemonplugin.h>
#include <e32cons.h>


class CSwiDaemonPluginImp : public CSwiDaemonPlugin
	{
public:
	/**
	Constructs a CSwiDaemonPluginImp which implements the CSwiDaemonPlugin interface.
	*/
	static CSwiDaemonPlugin *NewL();

	/**
	see interface spec
	CSwiDaemonPlugin::MediaProcessingStart
	*/
	virtual void MediaProcessingStart( TInt aDrive );
	
	/**
	see interface spec
	CSwiDaemonPlugin::RequestInstall
	*/
	virtual void RequestInstall( RFile& aFile );
	
	/**
	see interface spec
	CSwiDaemonPlugin::MediaRemoved
	*/
	virtual void MediaRemoved( TInt aDrive );
	
	/**
	see interface spec
	CSwiDaemonPlugin::MediaProcessingComplete
	*/	
	virtual void MediaProcessingComplete(); 


private:
	void ConstructL();
	virtual ~CSwiDaemonPluginImp();
	
	/**
	Called by MediaProcessingComplete to process each SIS file previously passed to RequestInstall.
	Intended to call apparch to install each SIS file, but currently this is disabled.
	@param aFile - RFile handle, valid until this function returns.
	*/
	void MediaProcessingCompleteInternalL(RFile &aFile); 
	
	/**
	@param stage	Identifies the callback
	@param fileName	Identifies the supplied filename
	
	This function creates tag files in the c:\\dummy-swi-plugin directory which are used by tswisstep.cpp
	to check that the correct callbacks have occured, in the correct order, and no extra callbacks happened.
	
	Each time this function is called, two empty tag files are created:-
	\\dummy-swi-plugin\\ddd
	\\dummy-swi-plugin\\ddd-stage-filename
	in system drive where driveddd is a decimal number which starts at 1 and is incremented 
	each time this function is called.If the filename argument is zero length, the "-filename" 
	part of the name is omitted.
	*/
	void tagStage(const TDesC &stage, const TDesC &fileName);
	
	/**
	@param stage    Identifies the callback
	@param fileName Identifies the supplied filename
	see tagStage above
	*/
	void tagStageL(const TDesC &stage, const TDesC &fileName);
	
	RFs iFs;
	TInt iCallbackNumber;
	CConsoleBase *iConsole;
	TUid iDtor_ID_Key;
	RArray<RFile> iRFiles;
	};

#endif // DUMMY_SWI_PLUGIN_H
