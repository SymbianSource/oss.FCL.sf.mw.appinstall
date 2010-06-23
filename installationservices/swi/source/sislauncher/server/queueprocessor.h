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
* CQueueProcessor - declaration
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef QUEUEPROCESSOR_H_
#define QUEUEPROCESSOR_H_

#include <e32base.h>
#include <f32file.h>
#include <ecom/ecom.h>

namespace Swi
{
class CSisLauncherServer;


class CQueueProcessor : public CActive
{
public:
	static CQueueProcessor* NewL(CSisLauncherServer& aServer);
	void ConstructL();
	
	~CQueueProcessor();
	
	void  AddToQueueL(const RMessage2& aMessage);
	void  ResetQueue();
	TInt  ExecuteQueue();
	TBool ItemsQueued();

    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);

private:
	
	struct CItem : public CBase
	{
		CItem();
		~CItem();
		
		static CItem* NewL(TInt aFunction, TFileName& aFilename, RFile& aFile, TBool wait, HBufC8* aMimeType);
		
		TInt  	iType;
		HBufC*  iFilename;
		RFile   iFile;
		TBool   iWait;
		HBufC8 *iMime;
	};

	CQueueProcessor(CSisLauncherServer& aServer);
	void  DoExecuteQueue();
	void Cleanup();
	
	RPointerArray<CItem> iQueue;
	CSisLauncherServer*  iServer;
	REComSession*        iEcomSession;
};

}

#endif /*QUEUEPROCESSOR_H_*/
