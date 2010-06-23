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
* Description:   This module contains the implementation of IAUpdateDialogUtil class 
*                member functions.
*
*/
#include <hblabel.h>
#include <hbmessagebox.h>

#include "iaupdatedialogutil.h"
#include "iaupdatedialogobserver.h"
#include "iaupdatedebug.h"



IAUpdateDialogUtil::IAUpdateDialogUtil(QObject *parent, IAUpdateDialogObserver *observer)
: QObject(parent),
  mObserver(observer)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateDialogUtil::IAUpdateDialogUtil()");
}

IAUpdateDialogUtil::~IAUpdateDialogUtil()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateDialogUtil::~IAUpdateDialogUtil()");
}


void IAUpdateDialogUtil::showInformation(const QString &text, HbAction *primaryAction)
{    
    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeInformation); 
    messageBox->setText(text);
    int actionCount = messageBox->actions().count();
    for (int i=actionCount-1; i >= 0; i--)
    { 
        messageBox->removeAction(messageBox->actions().at(i));
    }
    if (primaryAction)
    {    
        messageBox->addAction(primaryAction);
    }  
    messageBox->setTimeout(HbPopup::NoTimeout); 
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->open(this,SLOT(finished(HbAction*)));
}


void IAUpdateDialogUtil::showQuestion(const QString &text, HbAction *primaryAction, HbAction *secondaryAction)
{
    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeQuestion); 
    messageBox->setIconVisible(false);
    messageBox->setText(text);
    int actionCount = messageBox->actions().count();
    for (int i=actionCount-1; i >= 0; i--)
    { 
        messageBox->removeAction(messageBox->actions().at(i));
    }
    if (primaryAction)
    {    
        messageBox->addAction(primaryAction);
    }
    if (secondaryAction)
    {    
        messageBox->addAction(secondaryAction);
    } 
    messageBox->setTimeout(HbPopup::NoTimeout); 
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->open(this,SLOT(finished(HbAction*)));
}


void IAUpdateDialogUtil::showAgreement(HbAction *primaryAction, HbAction *secondaryAction)
{
    HbMessageBox *agreementDialog = new HbMessageBox(HbMessageBox::MessageTypeQuestion); 
    HbLabel *label = new HbLabel(agreementDialog);
    label->setHtml(QString("Disclaimer"));
    agreementDialog->setHeadingWidget(label);
    agreementDialog->setIconVisible(false);
    agreementDialog->setText("This application allows you to download and use applications and services provided by Nokia or third parties. Service Terms and Privacy Policy will apply. Nokia will not assume any liability or responsibility for the availability or third party applications or services. Before using the third party application or service, read the applicable terms of use.<br /><br />Use of this application involves transmission of data. Contact your network service provider for information about data transmission charges.<br /><br />(c) 2007-2010 Nokia. All rights reserved.");

    int actionCount = agreementDialog->actions().count();
    for (int i=actionCount-1; i >= 0; i--)
    { 
        agreementDialog->removeAction(agreementDialog->actions().at(i));
    }
    if (primaryAction)
    {    
        agreementDialog->addAction(primaryAction);
    }    
    if (secondaryAction)
    {
        agreementDialog->addAction(secondaryAction);
    }
    agreementDialog->setTimeout(HbPopup::NoTimeout);
    agreementDialog->setAttribute(Qt::WA_DeleteOnClose);
    agreementDialog->open(this,SLOT(finished(HbAction*)));
}



void IAUpdateDialogUtil::finished(HbAction *action)
{
    if (mObserver)
    {
        mObserver->dialogFinished(action);
    }
}






