/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CSWInstApplication 
*                class. 
*               
*                This class is a part of the standard application framework.
*                The application gets instantiated starting from this class.
*                Provides a factory method for instantiating the document object.
*
*
*/


#ifndef CSWINSTAPPLICATION_H
#define CSWINSTAPPLICATION_H

//  INCLUDES
#include <aknapp.h>

namespace SwiUI
{
        
// CLASS DECLARATION

/**
* This class is a part of the standard application framework.
* The application gets instantiated starting from this class.
* Provides a factory method for instantiating the document object.
*
*/
class CSWInstApplication : public CAknApplication
    {
    private:
        /**
        * From CApaApplication, Creates CSWInstApplication document object.
        * @since 3.0
        * @return A pointer to the created document object.
        */
	CApaDocument* CreateDocumentL();
	
	/**
        * From CApaApplication, Returns application's UID.
        * @since 3.0
        * @return The value of KUidSWInstSvr.
        */
	TUid AppDllUid() const;

        /**
        * From CAknApplication, Creates application server.
        * @since 3.0
        */
        void NewAppServerL( CApaAppServer*& aAppServer );
    };

}

#endif      // CSWINSTAPPLICATION_H   
            
// End of File
