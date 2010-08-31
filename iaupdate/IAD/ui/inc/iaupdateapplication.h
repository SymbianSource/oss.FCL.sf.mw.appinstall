/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the IAUpdateApplication
*                class 
*
*/

#ifndef IAUPDATEAPPLICATION_H_
#define IAUPDATEAPPLICATION_H_

#include <QSharedPointer>
#include <hbapplication.h>

// forward declarations
class IAUpdateEngine;
class IAUpdateMainWindow;

class IAUpdateApplication : public HbApplication
    {
public:
    IAUpdateApplication(int argc, char* argv[]);
    virtual ~IAUpdateApplication();
    
private:
    IAUpdateEngine *mEngine;         // owned
    QSharedPointer<IAUpdateMainWindow> mMainWindow; // owned
    };

#endif /* IAUPDATEAPPLICATION_H_ */
