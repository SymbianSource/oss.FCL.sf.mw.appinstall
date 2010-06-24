/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateFirstTimeInfo
*                class 
*
*/


#ifndef __IAUPDATE_FIRSTTIMEINFO_H__
#define __IAUPDATE_FIRSTTIMEINFO_H__

// INCLUDES
#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;

// CLASS DECLARATION
/**
*
*/

class CIAUpdateFirstTimeInfo : public CBase
    {
public:

    /**
    * Construct a CIAUpdateFirstTimeInfo using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateFirstTimeInfo
    */
    static CIAUpdateFirstTimeInfo* NewL();
    
    /**
    * Construct a CIAUpdateFirstTimeInfo using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateFirstTimeInfo
    */
    static CIAUpdateFirstTimeInfo* NewLC();
    
    /**
    * Destructor
    */
    ~CIAUpdateFirstTimeInfo();

public: // new functions
    
    /**
    * Set Nokia agreement as accepted by an user
    */
    void SetAgreementAcceptedL();
          
    /**
    * Set automatic update checks as asked (prompted) to an user
    */
    void SetAutomaticUpdatesAskedL();
            
    /**
    * Is Nokia agreement of Application Update accepted by an user
    *
    * @return True value if agreement accepted 
    */
    TBool AgreementAcceptedL();
    
    /**
    * Is activation for automatic update cheks from network already asked 
    *
    * @return True value if agreement already asked 
    */
    TBool AutomaticUpdateChecksAskedL();  
        
 
private:

    /**
    * C++ constructor 
    */
    CIAUpdateFirstTimeInfo();
    
    /**
    * Perform the second phase construction of a CIAUpdateFirstTimeInfo object
    */
    void ConstructL();
    
    void ReadDataL();
    
    void WriteDataL();
    
    void InternalizeL( RReadStream& aStream );
    
    void ExternalizeL( RWriteStream& aStream );
 
private: //data
    	
	RFs iFsSession;
		
	TFileName iPath;
	
	TBool iAgreementAccepted;
	
	TBool iAutomaticUpdateChecksAsked;   
   
    };


#endif // __IAUPDATE_FIRSTTIMEINFO_H__
