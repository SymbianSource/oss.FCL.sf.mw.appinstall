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
* Description:   MIAUpdateRefreshObserver interface
*
*/



#ifndef IAUPDATEREFRESHOBSERVER_H
#define IAUPDATEREFRESHOBSERVER_H

#include <e32base.h>

class MIAUpdateRefreshObserver
{
public: // Observer API
    /**
    * Called by CIAUpdateRefreshHandler when UI is needed to refresh 
    */ 
    virtual void HandleUiRefreshL() = 0;
};
	
#endif // IAUPDATEREFRESHOBSERVER_H
