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



#ifndef IA_UPDATE_SELF_UPDATER_CTRL_H
#define IA_UPDATE_SELF_UPDATER_CTRL_H


#include <e32base.h>

#include "iaupdaterendezvousobserver.h"

class MIAUpdateSelfUpdaterObserver;
class CIAUpdaterFileListFile;
class CIAUpdaterFileList;
class CIAUpdateRestartInfo;
class CIAUpdateRendezvous;
class MIAUpdateNode;


/**
 * @note If install files are moved and deleted from the private-folders,
 * then these actions require AllFiles capablility.
 *
 * @since S60 v3.2
 */
class CIAUpdateSelfUpdaterCtrl : public CBase,
                                 public MIAUpdateRendezvousObserver
    {

public:

    /**
     *
     * @since S60 v3.2
     */
    static CIAUpdateSelfUpdaterCtrl* NewL( MIAUpdateSelfUpdaterObserver& aObserver );

    /**
     *
     * @since S60 v3.2
     */
    static CIAUpdateSelfUpdaterCtrl* NewLC( MIAUpdateSelfUpdaterObserver& aObserver );


    /**
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateSelfUpdaterCtrl();
    

    /**
     * @see CIAUpdateController::SelfUpdateDataExists
     * 
     * @since S60 v3.2
     */
    TBool DataExists() const;


    /**
     * @see CIAUpdateController::StartL
     * 
     * @since S60 v3.2
     */
    TBool StartL( TInt aIndex, 
                  TInt aTotalCount, 
                  const RPointerArray< MIAUpdateNode >& aPendingNodes,
                  TBool aSilent );


    /**
     * Resets the information in the file list file.
     * 
     * @since S60 v3.2
     */
    void Reset();


    /**
     * @param aFileList Ownership is transferred.
     * @exception Leaves with system wide error code.
     * 
     * @since S60 v3.2
     */
    void AddFileListL( CIAUpdaterFileList* aFileList );


    /**
     * @see CIAUpdateController::SelfUpdateRestartInfo
     * 
     * @since S60 v3.2
     */
    CIAUpdateRestartInfo* SelfUpdateRestartInfo();


public: // MIAUpdateRendezvousObserver

    /**
     * @see MIAUpdateRendezvousObserver::RendezvousComplete
     */
    virtual void RendezvousComplete( TInt aErrorCode );


private:

    // Prevent these if not implemented
    CIAUpdateSelfUpdaterCtrl( const CIAUpdateSelfUpdaterCtrl& aObject );
    CIAUpdateSelfUpdaterCtrl operator =( const CIAUpdateSelfUpdaterCtrl& aObject );


    /**
     * @see CIAUpdateSelfUpdaterCtrl::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdateSelfUpdaterCtrl( MIAUpdateSelfUpdaterObserver& aObserver );

    /**
     *
     * @since S60 v3.2
     */
    void ConstructL();


    /**
     *
     * @since S60 v3.2
     */    
    MIAUpdateSelfUpdaterObserver& Observer();


    /**
     *
     * @since S60 v3.2
     */    
    CIAUpdaterFileListFile& File();


    /**
     *
     * @since S60 v3.2
     */    
    CIAUpdateRendezvous& RendezvousListener();


    /**
     *
     * @since S60 v3.2
     */    
    void StartUpdaterProcessL();


private: // data    

    // This observer will be informed when operations progress.
    MIAUpdateSelfUpdaterObserver& iObserver;
    
    // Files that will be installed.
    CIAUpdaterFileListFile* iFile;

    // This will listen the rendezvous of the self updater.
    CIAUpdateRendezvous* iRendezvous;
    
    };

#endif // IA_UPDATE_SELF_UPDATER_CTRL_H
