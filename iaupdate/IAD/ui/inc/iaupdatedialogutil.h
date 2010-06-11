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
* Description:   This file contains the header file of the IAUpdateDialogUtil class 
*
*/



#ifndef IAUPDATEDIALOGUTIL_H
#define IAUPDATEDIALOGUTIL_H

#include <QtCore/qmetaobject.h>
#include <QObject>
#include <HbAction>

class IAUpdateDialogObserver;


class IAUpdateDialogUtil : public QObject
{
    Q_OBJECT
    
    public:
            
    IAUpdateDialogUtil(QObject *parent, IAUpdateDialogObserver *observer = NULL);
    ~IAUpdateDialogUtil();

    void showInformation(const QString &text, HbAction *primaryAction);
    
    void showQuestion(const QString &text, HbAction *primaryAction, HbAction *secondaryAction);
    
    void showAgreement(HbAction *primaryAction, HbAction *secondaryAction = NULL);  
    
    public slots:

    void finished(HbAction *action);
    
    
    private:
    
        
    private:
    
    IAUpdateDialogObserver* mObserver; //not owned
    
};

#endif  // IAUPDATEDIALOGUTIL_H
            
// End of File
