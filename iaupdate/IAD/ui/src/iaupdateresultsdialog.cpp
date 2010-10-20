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

#include <hbmessagebox.h>
#include <hblabel.h>

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

void IAUpdateResultsDialog::showResults(const TIAUpdateResultsInfo &param, QObject *receiver, const char *member)
{
    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeInformation);
    
    HbLabel *label = new HbLabel(messageBox);
    label->setHtml(hbTrId("txt_software_title_update_results"));
    messageBox->setHeadingWidget(label);
    
    messageBox->setIconVisible(false);
            
    QString buf;
    constructText(param,buf);
    messageBox->setText(buf);
    messageBox->setTimeout(HbPopup::NoTimeout);
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->open(receiver, member);
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
        buf.append(hbTrId("txt_software_info_ln_updates_successful", param.iCountSuccessfull));
        buf.append("<br />");
        return;
    } 
  
    if (param.iCountSuccessfull != 0)
    {
        buf.append(hbTrId("txt_software_info_ln_updates_successful", param.iCountSuccessfull));
        buf.append("<br />");
    }
    
    if (param.iCountCancelled != 0)
    {
        buf.append(hbTrId("txt_software_info_ln_updates_cancelled", param.iCountCancelled));
        buf.append("<br />");
    }

    if (param.iCountFailed != 0)
    {
        buf.append(hbTrId("txt_software_info_ln_updates_failed", param.iCountFailed));
        buf.append("<br />");
    }
    
    if (param.iFileInUseError)
    {
    	buf.append(hbTrId("txt_swupdate_info_close_all_applications_and_try_a")); 
    	buf.append("<br />");
    }
} 

//  End of File  
