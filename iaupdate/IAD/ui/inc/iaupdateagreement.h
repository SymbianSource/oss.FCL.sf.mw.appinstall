/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateAgreement
*                class 
*
*/


#ifndef __IAUPDATE_AGREEMENT_H__
#define __IAUPDATE_AGREEMENT_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION
/**
*
*/

class CIAUpdateAgreement : public CBase
    {
public:

    /**
    * Construct a CIAUpdateAgreement using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateAgreement
    */
    static CIAUpdateAgreement* NewL();
    
    /**
    * Construct a CIAUpdateAgreement using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateAgreement
    */
    static CIAUpdateAgreement* NewLC();
    
    /**
    * Destructor
    */
    ~CIAUpdateAgreement();

public: // new functions
    
    /**
    * Checks that Nokia agreement of Application Update is accepted by an user
    * 
    * @return True value if agreement accepted 
    */
    TBool AcceptAgreementL();
    
    /**
    * Shows Nokia agreement of Application Update 
    */
    void ShowAgreementL();
    
    /**
    * Is Nokia agreement of Application Update accepted by an user
    *
    * @return True value if agreement accepted 
    */
    TBool AgreementAcceptedL();

private:

    /**
    * C++ constructor 
    */
    CIAUpdateAgreement();
    
    /**
     * Perform the second phase construction of a CIAUpdateAgreement object
     */
    void ConstructL();
    
    /**
    * Shows agreement dialog
    * 
    * @param aCbaResource Resource id of CBA
    *
    * @return Value of user selection 
    */
    TInt ShowDialogL( TInt aCbaResourceId );
    
    
private: //data
    
    };


#endif // __IAUPDATE_AGREEMENT_H__
