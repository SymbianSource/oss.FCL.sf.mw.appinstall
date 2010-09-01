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
* Description:   This file contains the header file of the CIAUpdateApplication
*                class 
*
*/



#ifndef __IAUPDATE_APPLICATION_H__
#define __IAUPDATE_APPLICATION_H__

// INCLUDES
#include <aknapp.h>

#include "iaupdateuids.h"

// CONSTANTS
// UID for the application, this should correspond to the uid defined in the mmp file
static const TUid KUidIAUpdateApp = { KIAUpdateUiUid };


/**
* CAppMngrApp application class.
* Provides factory to create concrete document object and
* application server object
*/
class CIAUpdateApplication : public CAknApplication
    {
    
private:  // from CAknApplication

    /**
    * Returns application's UID 
    * @return The UID value 
    */
    TUid AppDllUid() const;

    /**
    * From CApaApplication, creates CAppMngrDocument document object.
    * @return A pointer to the created document object.
    */
    CApaDocument* CreateDocumentL();
    
    /**
    * Creates application server.
    * @param aAppServer Instance of application server class
    */
    void NewAppServerL( CApaAppServer*& aAppServer );
    };
    

#endif // __IAUPDATE_APPLICATION_H__

// End of File