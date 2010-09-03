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
* Description:   Handles the launching of silent installation 
*
*/



#ifndef IAUPDATERSILENTLAUNCHER_H
#define IAUPDATERSILENTLAUNCHER_H

#include <e32base.h>
#include <f32file.h>

#include <usif/sif/sif.h>


/**
 * Class that handles the launching of silent installation.
 *
 */ 
class CIAUpdaterSilentLauncher : public CBase
    {
public:  
    
    /**
     * Two-phased constructor.
     */
    static CIAUpdaterSilentLauncher* NewL( RFs& aFs );
    
    /**
     * C++ Destructor.
     */
    virtual ~CIAUpdaterSilentLauncher();

public:  

    /**
     * Performs installation.
     *
     * @since 
     * @param aFile Name of the file to install
     * @param aStatus Request status
     * @param aResults Installation results 
     */
    void CIAUpdaterSilentLauncher::InstallL ( const TDesC& aFile, 
            TRequestStatus& aStatus, Usif::COpaqueNamedParams* aResults  );
    
    /**
     * Cancel the current installation.
     *
     * @since
     */
    void Cancel();        
    
private:

    /**
     * C++ Constructor.
     */  
    CIAUpdaterSilentLauncher( RFs& aFs );
     
    /**
     * Constructor.
     */
    void ConstructL();

    /**
     * @return SwiUI::TInstallOptions Options that are used for
     * silent installation.
     */
    void  UsifSilentInstallOptionsL( Usif::COpaqueNamedParams * aOptions );

private: //  Data
    
    Usif::RSoftwareInstall iLauncher;
    Usif::COpaqueNamedParams* iOptionsPckg; 

    RFs& iFs;        
    
    TBool iConnected;
    };

#endif  // IAUPDATERSILENTLAUNCHER_H   
            
// EOF
