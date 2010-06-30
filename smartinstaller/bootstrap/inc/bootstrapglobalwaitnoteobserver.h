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
*     Declaration of CGlobalWaitNoteObserver class.
*
*
*/


#include <e32base.h>

class CBootstrapAppUi;

class CGlobalWaitNoteObserver : public CActive
	{
	public:
		CGlobalWaitNoteObserver(CBootstrapAppUi* aADMAppUiObj);
		virtual ~CGlobalWaitNoteObserver();
		void Start();
	protected:
		void RunL() ;
		TInt RunError(TInt aError);
		void DoCancel();
	private:
		CBootstrapAppUi* iBsAppUiObj;
	};
