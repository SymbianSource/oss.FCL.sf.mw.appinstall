/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the RIAUpdateLauncherClient class 
*
*/



#ifndef IA_UPDATE_LAUNCHER_CLIENT_H
#define IA_UPDATE_LAUNCHER_CLIENT_H
#include <QObject>

class XQServiceRequest;

//class IAUpdateLauncherClient 
class IAUpdateLauncherClient : public QObject
{
     Q_OBJECT
public:

     
     IAUpdateLauncherClient();
     ~IAUpdateLauncherClient();

     void launch();

protected slots:
     void requestCompleted(const QVariant& value);
     void requestError(int err); 

                               
private: 

     XQServiceRequest* mServiceRequest;
     };

#endif // IA_UPDATE_LAUNCHER_CLIENT_H
