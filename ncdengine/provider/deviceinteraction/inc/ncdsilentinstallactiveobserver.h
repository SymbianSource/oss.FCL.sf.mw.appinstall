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
* Description:   CNcdSilentInstallActiveObserver
*
*/


#ifndef NCD_SILENT_INSTALL_ACTIVE_OBSERVER_H
#define NCD_SILENT_INSTALL_ACTIVE_OBSERVER_H


#include <e32base.h>
#include <f32file.h>
// For silent install
#include <usif/sif/sif.h>



class MNcdAsyncSilentInstallObserver;


/**
 * CNcdSilentInstallActiveObserver
 *
 * This is a CActive class that observes when another
 * operation has finished. When this observer is informed
 * about the completion of another operation 
 * this class object informs the MNcdAsyncSilentInstallObserver
 * about the completion.
 *
 * @see MNcdAsyncSilentInstallObserver
 */
class CNcdSilentInstallActiveObserver : public CActive
{

public:

    /**
     * @param aObserver The information about the completion of
     * an operation is forwarded to this observer by calling its callback
     * function.
     * @return CNcdSilentInstallActiveObserver* Newly created object.
     */
    static CNcdSilentInstallActiveObserver* NewL( MNcdAsyncSilentInstallObserver& aObserver );

    /**
     * @see NewL
     */
    static CNcdSilentInstallActiveObserver* NewLC( MNcdAsyncSilentInstallObserver& aObserver );


    /**
     * Destructor
     */
    virtual ~CNcdSilentInstallActiveObserver();


    /**
     * Sets this object active for observing a silent install operation.
     * 
     * @param aFileName Installation file name.
     * @param aSilentInstallOptions Silent installation options.
     */
    void StartToObserveL( const TDesC& aFileName,
                          const Usif::COpaqueNamedParams* aSilentInstallOptions );


    /**
     * Sets this object active for observing a silent install operation.
     * 
     * @param aFile Installation file handle.
     * @param aSilentInstallOptions Silent installation options.
     */
    void StartToObserveL( RFile& aFile,
                          const Usif::COpaqueNamedParams* aSilentInstallOptions );


    /**
     * Cancels the asynchronous silent install operation.
     * This will also close the silent launcher.
     *
     * @note Call this function instead of normal active object
     * Cancel. This function will use the Cancel to cancel the
     * ongoing asynchronous operation. And, after cancel operation
     * is finished, the silent launcher connection will be closed.
     *
     * @return TInt System wide error code about the cancellation.
     * KErrCancel if the cancellation was success.
     * If the cancellation could not cancel the operation but the
     * operation continued synchronously to the end, the result
     * of the finished operation will be returned here.
     */
    TInt CancelAsyncOperation();

    
protected: // CActive

    /**
     * @see CActive::DoCancel
     *
     * Cancels the silent install operation.
     */
    virtual void DoCancel();

    /**
     * @see CActive::RunL
     *
     * When the silent install operation finishes, this function calls
     * the callback function of the observer.
     */
    virtual void RunL();


protected:

    /**
     * @see NewL
     */ 
    CNcdSilentInstallActiveObserver( MNcdAsyncSilentInstallObserver& aObserver );

    /**
     * @see NewL
     */ 
    virtual void ConstructL();
    
    
    /**
     * @return MNcdAsyncSilentInstallObserver& The information 
     * about the completion of an operation is forwarded to 
     * this observer by calling its callback function.
     * Ownership is not transferred.
     */
    MNcdAsyncSilentInstallObserver& AsyncObserver() const;


    /**
     * @return Usif::RSoftwareInstall& The launcher
     * that handles the installation operations that this
     * class object observes. Ownerhsip is not transferred.
     */
    Usif::RSoftwareInstall& SilentLauncher();

    /**
     * Converts the given error code if necessary.
     * This can be used to simplify error codes for responses.
     *
     * @param aErrorCode Original error code.
     * @return TInt New error code.
     */
    TInt ConvertErrorCode( TInt aErrorCode );


private:

    // Prevent these if not implemented
    CNcdSilentInstallActiveObserver( const CNcdSilentInstallActiveObserver& aObject );
    CNcdSilentInstallActiveObserver& operator =( const CNcdSilentInstallActiveObserver& aObject );


private: // data

    // The information about the completion of
    // an operation is forwarded to this observer 
    // by calling its callback function.
    MNcdAsyncSilentInstallObserver& iObserver;

    // The silent launcher handles the install operations that
    // this class object observes. 
    Usif::RSoftwareInstall iInstaller;
    Usif::COpaqueNamedParams* iResults;
    Usif::COpaqueNamedParams* iArguments;
    
};

#endif // NCD_SILENT_INSTALL_ACTIVE_OBSERVER_H
