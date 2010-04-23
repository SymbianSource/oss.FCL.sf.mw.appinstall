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
* Definition of the install dialog command handler and command request object.
*
*/


/**
 @file 
 @internalComponent
 @released
*/
 
#ifndef __INSTALLDIALOG_H__
#define __INSTALLDIALOG_H__

#include "uisscommand.h"

class CApaMaskedBitmap;

namespace Swi
{
class MUiHandler;

class CActiveListener : public CActive
    {
public:
    CActiveListener() : CActive(CActive::EPriorityIdle) 
    	{ CActiveScheduler::Add(this); }
    void InitialiseActiveListener() 
    	{ iStatus = KRequestPending; SetActive(); }
    // From CActive
    virtual void RunL() { CActiveScheduler::Stop(); }
    virtual void DoCancel() {};
    };

/**
 * Command handler for the install dialog, which is displayed at the beginning of the 
 * installation.
 */
class CInstallDialogCmdHandler : public CUissCmdHandler
	{
	static void CleanupFbsSession(TAny*);	

public:
	virtual void HandleMessageL(const TDesC8& aInBuf, TDes8& aOutBuf);
	CInstallDialogCmdHandler(MUiHandler& aUiHandler);
	~CInstallDialogCmdHandler();

private:
	CApaMaskedBitmap* DecodeBitmapL(const TDesC8& aBitmapData);

private:
	CActiveListener* iListener;
	};

} // namespace Swi

#endif // #ifndef __INSTALLDIALOG_H__
