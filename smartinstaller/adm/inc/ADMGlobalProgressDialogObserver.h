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
*     Declares MGlobalProgressDialogObserver class for application.
*
*
*/


#ifndef __ADMGLOBALPROGRESSDIALOGOBSERVER_H__
#define __ADMGLOBALPROGRESSDIALOGOBSERVER_H__

class MGlobalProgressDialogObserver
	{
public:
	//keep these methods short running because they are called
	//from within an active object
	virtual void ProcessCancelOptionL() = 0;
	virtual void ProcessDoneOptionL() = 0;
	};

#endif  __ADMGLOBALPROGRESSDIALOGOBSERVER_H__
//EOF