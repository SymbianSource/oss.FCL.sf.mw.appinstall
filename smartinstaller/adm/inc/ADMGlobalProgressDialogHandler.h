/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     Declares CGlobalProgressDialogHandler class for application.
*
*
*/


#ifndef __ADMGlobalProgressDialogHandler_h__
#define __ADMGlobalProgressDialogHandler_h__

#include <e32base.h>
#include "ADMGlobalProgressDialogObserver.h"

class CGlobalProgressDialogHandler : public CActive
	{
public:
	~CGlobalProgressDialogHandler();
	static CGlobalProgressDialogHandler* NewL(MGlobalProgressDialogObserver& aObserver);
	static CGlobalProgressDialogHandler* NewLC(MGlobalProgressDialogObserver& aObserver);
public:
	void StartHandler();
private:
	CGlobalProgressDialogHandler(MGlobalProgressDialogObserver& aObserver)
			: CActive (EPriorityStandard), iObserver(aObserver){};
	void ConstructL();
private: // from CActive
	void RunL();
	void DoCancel();
private: // data
	MGlobalProgressDialogObserver& iObserver;
	};
#endif //__ADMGlobalProgressDialogHandler_h__
//EOF