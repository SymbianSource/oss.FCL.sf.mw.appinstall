/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements self update process.
*
*/



#ifndef IAUPDATERMNGR_H
#define IAUPDATERMNGR_H

#include <e32base.h>
#include <f32file.h>
#include <w32std.h>

#include "iaupdaterparams.h"
#include "iaupdatercancelobserver.h"


class CIAUpdaterFileList;
class CIAUpdaterResultsFile;
class CIAUpdaterInstaller;


/**
 *  This class implements self update manager.
 *
 *  @code   
 *  @endcode
 *  @lib 
 *  @since S60 
 */ 
class CIAUpdaterMngr : public CBase, 
                       public MIAUpdaterCancelObserver 
	{
public:

    /**
     * Two-phased constructor.
     */    
	static CIAUpdaterMngr* NewL();
	
    /**
     * Two-phased constructor.   
     */	
    static CIAUpdaterMngr* NewLC();
     
    /**
     * Destructor.
     */
    ~CIAUpdaterMngr();
	
    /**
     * Funtion gets sis pkgs and starts silent install process.
     *
     * @since S60 
     * @param aStartParams Start parametrs       
     */
	void ProcessInstallingL( TIAUpdaterStartParams& aStartParams );
	
    /**
     * Function sets parameters for IAD process.
     *
     * @since S60 
     * @param aParams       
     */
    void SetProcessParametersL( TInt aErr );
    
    /**
     * Start IAD processes after installation.
     *
     * @since S60 
     */
    void StartIADProcessesL();    	

    /**
     * Close IAD processes after successfull installation.
     *
     * @since S60 
     */
    void CloseIADProcessesL();
			
private:

    /**
     * Constructor.
     */    
	void ConstructL();

private: //From MIAUpdateCancelObserver

     /**
     * Called when global progress note cancelled by an user
     *
     */
     void UserCancel();
     
     /**
     * Called when global progress note removed by end key
     *
     */
     void UserExit();


private: // new functions
	
    /**
     * Function gives paths to install process.
     *
     * @since S60 
     * @param aList Contains information about the content files.          
     */
	void ProcessSISFilesL( CIAUpdaterFileList& aList );

    /**
     * Waits until ECOM has updated its index list.
     */ 
    void EcomWaiterL() const;

    /**
     * Checks if the given UID matches 
     * any known ECOM plugin package UID.
     */
    TBool IsEcomPlugin( const TUid& aUid ) const;
    
    /**
     * Returns full name of currently installed iaupdate.exe 
     */
    void InstalledIAUpdateL( TFileName& aFullName ) const;


private: // data

    // File server session.
	RFs iFs; 		
		
	// Installer 
	CIAUpdaterInstaller* iIAInstaller; 

    // This will contain parameters that iaupdate uses when it
    // is started after self updater.
    CIAUpdaterResultsFile* iResultsFile;

    // This is used to identify what is the current file list
    // that will be installed. Not owned.
    CIAUpdaterFileList* iCurrentFileList;

    // This is used for the application task list.
    RWsSession iWsSession;

    // This error value is used to recognize if one or more
    // of the installed packets have failed.
    TInt iInstallError;
    
    // User exit with end key
    TBool iUserExit;
    
    // Informs if the installation package has been an ECOM plugin.
    // If so, then ECOM indexing will be waited in the end of the flow.
    TBool iEcomPluginInstalled;
    
	};

#endif //CIAUPDATERMNGR_H 

//EOF
