/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateStarter
*                class 
*
*/


#ifndef __IAUPDATE_STARTER_H__
#define __IAUPDATE_STARTER_H__

// INCLUDES
#include <e32base.h>
#include "iaupdateuitimerobserver.h"

// FORWARD DECLARATIONS
class CApaWindowGroupName;
class MIAUpdateStarterObserver;
class CIAUpdateUITimer;

// CLASS DECLARATION
/**
*
*/

class CIAUpdateStarter :public CActive, public MIAUpdateUITimerObserver
    {
public:

    /**
    * Construct a CIAUpdateStarter using two phase construction,
    * and return a pointer to the created object
    * @param aCommandLineExecutable Name of executable to be started
    * @param aCommandLineArguments Command line arguments of an application 
    * @return A pointer to the created instance of CIAUpdateStarter
    */
    static CIAUpdateStarter* NewL( const TDesC& aCommandLineExecutable,
                                   const TDesC8& aCommandLineArguments );
    
    /**
    * Destructor
    */
    ~CIAUpdateStarter();

public: // new functions
    /**
    * Starts an executable
    *
    * @param aStarterObserver  Observer
    */
    void StartExecutableL( MIAUpdateStarterObserver& aStarterObserver );
    
    /**
    * Checks weather startable executable exists in installation package
    *
    * @param aPUid  Package Uid of installation
    */
    void CheckInstalledPackageL( const TUid& aPUid );    
    
    /**
    * Bring started application to foreground
    */
    void BringToForegroundL() const;
    
private: // from CActive

    /**
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
           
    /**
     * Just trap leave error and pass it to observer
     *
     * @see CActive::RunError
     *
     */
    TInt RunError( TInt aError );
    
private: // from MIAUpdateUITimerObserver
      
     void BackgroundDelayComplete( TInt aError );   
         
     void ForegroundDelayComplete( TInt aError );
     
     void ProcessStartDelayComplete( TInt aError );  

private:  //constructors

    /**
    * Perform the second phase construction of a CIAUpdateStarter object
    * @param aCommandLineExecutable Name of executable to be started
    * @param aCommandLineArguments Command line arguments of an application 
    */
    void ConstructL( const TDesC& aCommandLineExecutable,
                     const TDesC8& aCommandLineArguments );

    /**
    * C++ constructor 
    */
    CIAUpdateStarter();

private:  //new functions
    
    /**
    * Starts executable
    */
    void LaunchExeL();
    
    /**
    * Close executable
    */
    void CloseExecutableL();
    
    /**
    * Finds UID of executable
    * @param aUid UID 
    */
    void UidForExecutableL( TUid& aUid ) const;
    
    /**
    * Starts process
    */
    void StartProcessL();

private: //data

    HBufC* iCommandLineExecutable;
    
    HBufC8* iCommandLineArguments;
    
    TBool iStartNeeded;
    
    TBool iExeStarted;
    
    TUid iUid;
    
    TInt iPrevWgId;
        
    RThread iThread;
	
	RProcess iProcess;
	
	MIAUpdateStarterObserver* iStarterObserver; //not owned
	
	CEikonEnv* iEikEnv; //not owned
	
	CIAUpdateUITimer* iProcessStartTimer;
	
    };


#endif // __IAUPDATE_STARTER_H__
