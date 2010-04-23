/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef DIALOGWATCHER_H_
#define DIALOGWATCHER_H_

#include <e32base.h>
#include <f32file.h>
#include <e32property.h>
#include <SWInstallerInternalPSKeys.h>

namespace Swi
{
class CDialogWrapper;
/**
 * This active object waits for an indication that the PS key 
 * has changed. 
 */
class CDialogWatcher : public CActive
    {
public:
    
    static CDialogWatcher* NewL( CDialogWrapper* aDialog );    
    static CDialogWatcher* NewLC( CDialogWrapper* aDialog );    
    virtual ~CDialogWatcher();    
    
    void CancelNoteRequest();
    
    void RequestToDisplayNote();
    
    void StartWatcher();
   
    void StopWatcher();
    
    TInt GetPSKeyForUI( TInt& aPSKeyValue );
    
private:
    
    CDialogWatcher();

    void ConstructL( CDialogWrapper* aDialog );    
   
    void DoCancel();  // from CActive

    void RunL();   // from CActive

    TInt RunError( TInt aError );  // from CActive
 
private:
    
    RProperty iProperty;    
    CDialogWrapper* iDialogWrapper;
    // Defines ID number for active note.
    TBool iNoteActive;
    // Defines need for showing the note dialog if dialog was closed. 
    TBool iRequestToDisplayNote;
    // Defines which note should be shown, instaler or uninstaller.
    TBool iShowInstallNote;
    };
    
} //namespace Swi

#endif /* DIALOGWATCHER_H_ */

//EOF
