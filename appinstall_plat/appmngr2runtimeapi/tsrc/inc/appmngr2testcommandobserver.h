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
* Description:   
*
*/


#ifndef C_APPMNGR2TESTCOMMANDOBSERVER_H
#define C_APPMNGR2TESTCOMMANDOBSERVER_H

class CAppMngr2TestCommand;

class MAppMngr2TestCommandObserver
    {
public:
    virtual void CommandResult( CAppMngr2TestCommand& aCommand, TInt aCompletionCode ) = 0;
    };

#endif  // C_APPMNGR2TESTCOMMANDOBSERVER_H

