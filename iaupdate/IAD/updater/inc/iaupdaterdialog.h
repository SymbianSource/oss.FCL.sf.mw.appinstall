/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Wraps functions for showing some global dialogs.
*
*/



#ifndef CIAUPDATERDIALOG_H
#define CIAUPDATERDIALOG_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <barsc.h>

#include <hbdeviceprogressdialogsymbian.h>

// FORWARD DECLARATIONS
class MIAUpdaterCancelObserver;
/**
* Class for showing the global note dialog when installing self update.
*/
class CIAUpdaterDialog : public CBase,
                         public MHbDeviceProgressDialogObserver
    {
public:  
        
    /**
     * Two-phased constructor.
     */
    static CIAUpdaterDialog* NewL( MIAUpdaterCancelObserver& aObserver );
    /**
     * Two-phased constructor.
     */
    static CIAUpdaterDialog* NewLC( MIAUpdaterCancelObserver& aObserver );

            
    /**
     * Destructor.
     */
    ~CIAUpdaterDialog();


    /**
     * Show global waiting note (during installing).
     *
     * @param Current pkg's name for wait dialog.
     * @param Current pkg's index number for wait dialog.
     * @param aTotalCount Max pkg count for wait dialog.
     */
    void ShowWaitingNoteL( const TDesC& aName, 
                           TInt aIndex, 
                           TInt aTotalCount );

    /**
     * Cancel global waiting note (after installing).
     */
    void CancelWaitingNoteL();

    /**
     * From base class MHbDeviceProgressDialogObserver
     */
    void ProgressDialogCancelled(
        const CHbDeviceProgressDialogSymbian* aProgressDialog);
    
    /**
    * From base class MHbDeviceProgressDialogObserver
    */
    void ProgressDialogClosed(
         const CHbDeviceProgressDialogSymbian* aProgressDialog);



private:

    /**
     * Constructor.
     */  
    CIAUpdaterDialog( MIAUpdaterCancelObserver& aObserver );
     
    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    
    /**
     * Destroy wait note.
     */
    void CIAUpdaterDialog::DestroyGlobalWaitNote();
    

private: // data

    /**
     * Not owned, install observer.
     */
    MIAUpdaterCancelObserver* iObserver; 
    /**
    * Own, Global wait note.
    */
    CHbDeviceProgressDialogSymbian* iGlobalWaitNote;
    
    /**
    * Own, resouce of global note.
    */
    HBufC* iGlobalResource;
    
    /**
     * Check Load Success .ts file successfully or not
     */
    TBool iIsResolverSuccess;
        
    };

#endif      // CIAUPDATERDIALOG_H   
            
// End of File
