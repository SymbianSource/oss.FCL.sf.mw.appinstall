/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CDialogWrapper class.
*
*/


#ifndef DIALOGWRAPPER_H
#define DIALOGWRAPPER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <barsc.h>
#include "dialogwatcher.h"

namespace Swi
{

/**
* This class wraps functions for showing some global dialogs.
* 
* @since 3.0 
*/
class CDialogWrapper : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDialogWrapper* NewL( RFs& aFs );
        
        /**
        * Destructor.
        */
        virtual ~CDialogWrapper();

    public:  // New functions

        /**
        * Show global result dialog (untrusted case).
        * @since 3.0
        */
        void ShowUntrustedResultL();

        /**
        * Show global result dialog (error case).
        * @since 3.0
        */
        void ShowErrorResultL();
        /**
        * Show global waiting note (during installing).
        * @since 3.0
        */
        void ShowWaitingNoteL();
        
        /**
        * Cancel global waiting note (after installing).
        * @since 3.0
        */
        void CancelWaitingNoteL();
        
        /**
         * Sets dialog control flag.
         * @since 3.2
         * @parm TInt Flag for dialog control.
         */       
        void SetUIFlag( TInt aUIFlag );
        
        /**
          * Show global waiting note (during uninstalling).
          * @since 3.2         
          */              
        void ShowWaitingNoteForUninstallerL();
        
        /**
          * Disable installer's notes. This is used in first boot
          * to disable wait note and error notes. 
          * This flag do not affect to uninstaller's wait note.
          * @since 9.2  
          * @parm TBool Flag for dialog contorl. If True = disabled.       
          */                   
        void DisableInstallNotes( TBool aValue );

    private:

        /**
        * Constructor.
        */  
        CDialogWrapper( RFs& aFs );
         
        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Read resource string.
        * @since 3.0
        * @param aResourceId - Id of the resource.
        * @return Read resource buffer.
        */
        HBufC* ReadResourceLC( TInt aResourceId );        

    private: //  Data

        RResourceFile iResourceFile;        
        RFs& iFs;        
        TInt iNoteId;  
        TInt iDisableAllNotes;
        CDialogWatcher* iWatcher;
        // Disable install notes in first boot if device has emmc.
        TBool iDisableInstallNotes;        
    };
}

#endif      // DIALOGWRAPPER_H   
            
// End of File
