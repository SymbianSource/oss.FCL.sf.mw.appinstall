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
* Interface definition for Reference SWI Observer ECOM plug-in.
*
*/


/**
 @file
 @test
*/

#ifndef REFSWIOBSPLUGIN_H
#define REFSWIOBSPLUGIN_H

#include <ecom/implementationproxy.h>
#include "swiobserverplugin.h"
#include <s32file.h>

using namespace Swi;

NONSHARABLE_CLASS(CRefSwiObsPlugin) : public CSwiObserverPlugin
	{
public:
	static CSwiObserverPlugin* CreateRefSwiObserverL();
		
	void StartL(RFs& aFs);
 	void NextObservedHeaderL(const CObservationHeader& aHeader, TRequestStatus& aStatus);	
 	void NextObservedDataL(const CObservationData& aData, TInt aFilterIndex, TRequestStatus& aStatus);	
 	void FinalizeL(TRequestStatus& aStatus);
 	CObservationFilter* GetFilterL() const;
 		
protected:
	// From CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
		
	~CRefSwiObsPlugin();
		
private:
	CRefSwiObsPlugin();
		
private:
	/** The request status object owned by the SWI Observer. */
	TRequestStatus* iClientStatus;
	/** The stream handle used to write data into the log file. */
	RFileWriteStream iStream;	
	};

#endif
