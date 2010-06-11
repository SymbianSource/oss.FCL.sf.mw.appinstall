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
* Description:   ?Description
*
*/



#ifndef IA_UPDATE_SELF_UPDATER_INITIALIZER_H
#define IA_UPDATE_SELF_UPDATER_INITIALIZER_H

#include <e32base.h>
#include <f32file.h>

class CIAUpdateNode;
class MIAUpdateSelfUpdaterInitObserver;
class CIAUpdaterFileList;
class MNcdNode;


/**
 * CIAUpdateSelfUpdaterInitializer is used to set file paths for the
 * self updater. Self updater will install these content files when
 * is is started.
 *
 */
class CIAUpdateSelfUpdaterInitializer : public CActive
    {

public:

    /**
     * @param aNode Node that has content file paths for the self update content.
     * @return CIAUpdateSelfUpdaterInitializer* Created object.
     *
     */
    static CIAUpdateSelfUpdaterInitializer* NewL(
                      CIAUpdateNode& aNode, 
                      MIAUpdateSelfUpdaterInitObserver& aObserver );

    /**
     * @see CIAUpdateSelfUpdaterInitializer:: NewL
     *
     */
    static CIAUpdateSelfUpdaterInitializer* NewLC( 
                      CIAUpdateNode& aNode,
                      MIAUpdateSelfUpdaterInitObserver& aObserver );


    /**
     * Destructor
     *
     */
    ~CIAUpdateSelfUpdaterInitializer();
    

    /**
     * Starts the operation.
     *
     * @exception Leaves with KErrInUse if operation is already going on.
     */
    void StartL();


protected: // CActive
    

    /**
     * @see CActive::DoCancel
     *
     */
	virtual void DoCancel();

    /**
     * @see CActive::RunL
     *
     */
	virtual void RunL();

    /**
     * @see CActive::RunError
     *
     */
     virtual TInt RunError( TInt aError );


private:

    /**
     * TUpdaterState defines possible states of this active object.
     *
     */
    enum TUpdaterState
        {
        /**
         * No operation is going on.
         *
         */
        ENotStarted,

        /**
         * Initializes everything for the actual self updating.
         * Sets the content file paths for the file list.
         * 
         */
        EInitialize
        };


    /**
     * @see CIAUpdateSelfUpdaterInitializer::NewL
     *
     */
    CIAUpdateSelfUpdaterInitializer( CIAUpdateNode& aNode,
                                     MIAUpdateSelfUpdaterInitObserver& aObserver );

    /**
     * 2nd. phase constructor
     *
     */
    virtual void ConstructL();


    /**
     * @return const TUpdaterState& Current state of this active object.
     *
     */
    const TUpdaterState& State() const;


    /**
     * @return CIAUpdateNode& Node that has content file paths 
     * for the self update content.
     *
     */
    CIAUpdateNode& Node();


    /**
     * @return MIAUpdateSelfUpdaterInitObserver& Operation observer.
     *
     */
    MIAUpdateSelfUpdaterInitObserver& Observer();


    /**
     * @return RFs& File server session for this class object.
     *
     */    
    RFs& FileServer();


    /**
     * Sets the content files to the array from the purchase history.
     *
     */    
    void SetContentFileListL();


    /**
     * This function is called when the initialization has been done.
     * This function will inform the observing node about the completion.
     *
     */    
    virtual void InitCompleteL();


private:

    // Prevent these if not implemented
    CIAUpdateSelfUpdaterInitializer( const CIAUpdateSelfUpdaterInitializer& aObject );
    CIAUpdateSelfUpdaterInitializer operator =( const CIAUpdateSelfUpdaterInitializer& aObject );


private: // data    
    
    // Node whose content will be installed.
    CIAUpdateNode& iNode;

    // Operation observer.
    MIAUpdateSelfUpdaterInitObserver& iObserver; 

    // State of the updater operation.
    TUpdaterState iState;

    // File server session.
    RFs iFs;
    
    // List of the content files that should be installed by the self updater.
    // Ownership will be transferred. Owned temporarily.    
    CIAUpdaterFileList* iFileList;
    
    };

#endif // IA_UPDATE_SELF_UPDATER_INITIALIZER_H
