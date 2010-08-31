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
* Description:   Header file of IAUpdateResultsDialog
*
*/



#ifndef IAUPDATERESULTSDIALOG_H
#define IAUPDATERESULTSDIALOG_H


//  INCLUDES
#include <QObject>

class TIAUpdateResultsInfo;

class IAUpdateResultsDialog : public QObject
{
public:

    IAUpdateResultsDialog(QObject *parent = 0);
    ~IAUpdateResultsDialog();
    
    void showResults( const TIAUpdateResultsInfo &param, QObject* receiver, const char* member );
   
private:

    void constructText(const TIAUpdateResultsInfo &param, QString &buf);
};
#endif      // IAUPDATERESULTSDIALOG_H
            
// End of File
