/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the IAUpdateWaitDialog class.
*
*/



#ifndef IAUPDATEWAITDIALOG_H
#define IAUPDATEWAITDIALOG_H

// INCLUDES
#include <QObject>

#include "iaupdatewaitdialog.h"


// FORWARD DECLARATIONS
class HbProgressDialog;
class MIAUpdateWaitDialogObserver;

// CLASS DECLARATION

/**
* Wait dialog for IA Update client
*/
class IAUpdateWaitDialog : public QObject
{
    Q_OBJECT

public: // Constructors and destructor

    /**
    * Constructor
    */
    IAUpdateWaitDialog();

    ~IAUpdateWaitDialog();

public:  // New functions
    
    int showDialog(const QString& text);
        
    void SetCallback(MIAUpdateWaitDialogObserver* callback );  
    
    void close();
    
public slots:

    void dialogCancelled();

private: //data
    
    HbProgressDialog *mWaitDialog;
    MIAUpdateWaitDialogObserver *mCallback;        
};


#endif // IAUPDATEWAITDIALOG_H

// End of file
