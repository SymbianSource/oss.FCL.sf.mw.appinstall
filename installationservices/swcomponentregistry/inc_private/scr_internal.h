/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* SCR Internal client which should be used to implement all internal functions
* to the SCR server.
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SCR_INTERNAL_H
#define SCR_INTERNAL_H

namespace Usif {

NONSHARABLE_CLASS(RScrInternalClient) : public RScsClientBase
    {
    /**
     * This client should be used to implement all internal functions to the SCR server.
     */
public:
    
    IMPORT_C RScrInternalClient();
        
    IMPORT_C TInt Connect();

    IMPORT_C void Close();
    
    /**
         Generates a new Application Uid which can be used to register a non native application.
                  
         @return An unused UID if available, else a NULL TUid object is returned. 
     */
    IMPORT_C TUid GenerateNewAppUidL() const;

    };

} // namespace Usif

#endif // SCR_INTERNAL_H
