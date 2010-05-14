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
* Description:    
*
*/



// INCLUDE FILES

#include <hbaction.h>
#include <hbdialog.h>
#include <hbtextitem.h>

#include "iaupdateresultsdialog.h"
#include "iaupdateresultsinfo.h"
#include "iaupdatedebug.h"


IAUpdateResultsDialog::IAUpdateResultsDialog(QObject *parent)
    : QObject(parent)
{
    
}

IAUpdateResultsDialog::~IAUpdateResultsDialog()
{
}

void IAUpdateResultsDialog::showResults(const TIAUpdateResultsInfo &param)
{
    QString buf;
    constructText(param,buf);
    HbDialog dialog;
    HbTextItem *text = new HbTextItem(&dialog);
    text->setFontSpec(HbFontSpec(HbFontSpec::Primary));
    text->setText(buf);
    dialog.setContentWidget(text);
    HbAction *primaryAction = new HbAction("Ok");    
    dialog.setPrimaryAction(primaryAction);
    dialog.setTimeout(HbPopup::NoTimeout);
    dialog.show();
    //dialog.exec();    
    return;  
}

// -----------------------------------------------------------------------------
// IAUpdateResultsDialog::constructText
//
// -----------------------------------------------------------------------------
//
void IAUpdateResultsDialog::constructText(const TIAUpdateResultsInfo &param, QString &buf)
{ 
    if (param.iCountSuccessfull == 0 && param.iCountCancelled == 0 &&
        param.iCountFailed == 0)
    {
        QString stringCount;
        stringCount.setNum(param.iCountSuccessfull);    
        buf.append(stringCount);
        buf.append(" updates successful");
        buf.append("\n");
        return;
    } 
  
    if (param.iCountSuccessfull != 0)
    {
        QString stringCount;
        stringCount.setNum(param.iCountSuccessfull);    
        buf.append(stringCount);
        if (param.iCountSuccessfull == 1)
        {
            buf.append(" application updated"); 
        }
        else
        {
            buf.append(" applications updated"); 
        }
        buf.append("\n");
    }
    
    if (param.iCountCancelled != 0)
    {
        QString stringCount;
        stringCount.setNum(param.iCountCancelled);    
        buf.append(stringCount);
        if (param.iCountCancelled == 1)
        {
            buf.append(" update cancelled");
        }
        else
        {
            buf.append(" updates cancelled");
        }
        buf.append("\n");
    }

    if (param.iCountFailed != 0)
    {
        QString stringCount;
        stringCount.setNum(param.iCountFailed);    
        buf.append(stringCount);
        if (param.iCountFailed == 1)
        {
            buf.append(" update failed");
        }
        else
        {
            buf.append(" updates failed");
        }
        buf.append("\n");
    }
    
    if (param.iFileInUseError)
    {
    	buf.append("Close all applications and try again.");
    	buf.append("\n");
    }
} 

//  End of File  
