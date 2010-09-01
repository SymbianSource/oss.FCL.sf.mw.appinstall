/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include <AknGlobalNote.h>

// FORWARD DECLARATIONS
class MIAUpdaterCancelObserver;

/**
* Class for showing the global note dialog when installing self update.
* 
*  @code   
*  @endcode
*  @lib 
*  @since S60 
*/
class CIAUpdaterDialog : public CActive
    {
public:  
        
    /**
     * Two-phased constructor.
     */
    static CIAUpdaterDialog* NewL( RFs& aFs, MIAUpdaterCancelObserver& aObserver ); 

    /**
     * Two-phased constructor.
     */
    static CIAUpdaterDialog* NewLC( RFs& aFs, MIAUpdaterCancelObserver& aObserver );

            
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


private:

    /**
     * Constructor.
     */  
    CIAUpdaterDialog( RFs& aFs, MIAUpdaterCancelObserver& aObserver );
     
    /**
     * 2nd phase constructor.
     */
    void ConstructL();


    /**
     * Read resource string.
     * @since 
     * @param aResourceId Id of the resource.
     * @return Resource buffer.
     */
    HBufC* ReadResourceLC( TInt aResourceId );    
        
private: // from CActive

    /**
     * Cancels async request 
     * @see CActive::DoCancel
     *
     */
    void DoCancel();

    /**
     * When the server side has finished operation, the CActive object will
     * be informed about it, and as a result RunL will be called. This function
     * well inform the observer that the operation has been completed.
     * @see CActive::RunL
     *
     */
    void RunL();

private: // data

    // Resource file.
    RResourceFile iResourceFile;

    // File server handle.        
    RFs& iFs;        
                   
    CAknGlobalNote* iNote;
    
    // Dialog id for canceling dialog. 
    TInt iNoteId;
        
    MIAUpdaterCancelObserver* iObserver; //not owned
        
    };

#endif      // CIAUPDATERDIALOG_H   
            
// End of File
