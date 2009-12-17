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
* Description:   This file contains the header file of the CIAUpdateDocument
*                class 
*
*/


#ifndef __IAUPDATE_DOCUMENT_H__
#define __IAUPDATE_DOCUMENT_H__

// INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class CIAUpdateAppUi;
class CEikApplication;


// CLASS DECLARATION
/**
*  CIAUpdateDocument is the Document part of the AVKON
*  application framework for the IAUpdate example application
*
*/

class CIAUpdateDocument : public CAknDocument
    {
public:

    /**
    * Construct a CIAUpdateDocument for the AVKON application aApp 
    * using two phase construction, and return a pointer to the created object
    * @param aApp Application creating this document
    * @return A pointer to the created instance of CIAUpdateDocument
    */
    static CIAUpdateDocument* NewL( CEikApplication& aApp );

    /**
    * Construct a CIAUpdateDocument for the AVKON application aApp 
    * using two phase construction, and return a pointer to the created object
    * @param aApp Application creating this document
    * @return A pointer to the created instance of CIAUpdateDocument
    */
    static CIAUpdateDocument* NewLC( CEikApplication& aApp );

    /**
    * Destructor
    */
    ~CIAUpdateDocument();

public: // from CAknDocument
    /**
    *  Create a CIAUpdateAppUi object and return a pointer to it
    * @return a pointer to the created instance of the AppUi created
    */
    CEikAppUi* CreateAppUiL();

private:

    /**
    * Perform the second phase construction of a CIAUpdateDocument object
    */
    void ConstructL();

    /**
    * Perform the first phase of two phase construction
    @param aApp Application creating this document
    */
    CIAUpdateDocument( CEikApplication& aApp );

    };


#endif // __IAUPDATE_DOCUMENT_H__
