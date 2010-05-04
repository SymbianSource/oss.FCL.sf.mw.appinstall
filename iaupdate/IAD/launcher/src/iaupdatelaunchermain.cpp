/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of main function
*
*/

#include <QApplication>
#include "iaupdatelauncherclient.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    IAUpdateLauncherClient launcherClient;
    launcherClient.launch();
    return app.exec();
}
