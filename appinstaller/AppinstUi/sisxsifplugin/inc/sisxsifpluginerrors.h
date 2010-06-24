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
* Description:  Detailed error codes for KErrPermissionDenined errors.
*
*/

#ifndef SISXSIFPLUGINERRORS_H
#define SISXSIFPLUGINERRORS_H

enum TSisxSifPluginErrors
    {
    ETrustedUICapabilityRequired = 1,       // TrustedUI capability required
    ENeedsAllowUntrustedParameter = 2,      // AllowUntrusted parameter needed
    ENeedsGrantCapabilitiesParameter = 3,   // GrantCapabilities parameter needed
    EAllFilesCapabilityRequired = 4         // AllFiles capability required
    };

#endif // SISXSIFPLUGINERRORS_H

