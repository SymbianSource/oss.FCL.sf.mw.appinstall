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
* Description:  SW installation settings changer application.
*
*/

#include "swinstsettingschanger.h"
#include <hbmainwindow.h>
#include <hbview.h>
#include <QGraphicsLinearLayout>
#include <hbcombobox.h>
#include <hbtextedit.h>
#include <hbcheckbox.h>
#include <hbpushbutton.h>
#include <centralrepository.h>
#include <SWInstallerInternalCRKeys.h>


SwInstSettingsChanger::SwInstSettingsChanger(int& argc, char* argv[]) : HbApplication(argc, argv),
    mMainWindow(0), mMainView(0), mOcspProcedure(0), mDefaultUrl(0), mAllowUntrusted(0)
{
    mMainWindow = new HbMainWindow();
    mMainView = new HbView();
    mMainView->setTitle(tr("SWInstSettings"));

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);

    HbPushButton *readButton = new HbPushButton("Read");
    connect(readButton, SIGNAL(clicked()), this, SLOT(handleRead()));
    layout->addItem(readButton);

    HbPushButton *saveButton = new HbPushButton("Save");
    connect(saveButton, SIGNAL(clicked()), this, SLOT(handleSave()));
    layout->addItem(saveButton);

    mOcspProcedure = new HbComboBox;
    mOcspProcedure->setEditable(false);
    QStringList ocspProcedureList;
    ocspProcedureList << "Off" << "On" << "Must";
    mOcspProcedure->setItems(ocspProcedureList);
    layout->addItem(mOcspProcedure);

    mDefaultUrl = new HbTextEdit;
    layout->addItem(mDefaultUrl);

    mAllowUntrusted = new HbCheckBox;
    mAllowUntrusted->setText(tr("KSWInstallerAllowUntrusted"));
    layout->addItem(mAllowUntrusted);

    HbPushButton *closeButton = new HbPushButton("Close");
    connect(closeButton, SIGNAL(clicked()), qApp, SLOT(quit()));
    layout->addItem(closeButton);

    mMainView->setLayout(layout);
    mMainWindow->addView(mMainView);
    mMainWindow->show();

    handleRead();
}

SwInstSettingsChanger::~SwInstSettingsChanger()
{
    delete mMainView;
    delete mMainWindow;
}

void SwInstSettingsChanger::handleRead()
{
    QT_TRAP_THROWING( DoHandleReadL() );
}

void SwInstSettingsChanger::handleSave()
{
    QT_TRAP_THROWING( DoHandleSaveL() );
}

void SwInstSettingsChanger::DoHandleReadL()
{
    CRepository* repository = CRepository::NewL( KCRUidSWInstallerSettings );
    CleanupStack::PushL( repository );

    TInt ocspProcedure = 0;
    User::LeaveIfError( repository->Get( KSWInstallerOcspProcedure, ocspProcedure ) );
    mOcspProcedure->setCurrentIndex( ocspProcedure );

    HBufC* ocspUrlBuf = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
    TPtr ocspUrl( ocspUrlBuf->Des() );
    User::LeaveIfError( repository->Get( KSWInstallerOcspDefaultURL, ocspUrl ) );
    QString defaultUrl = QString::fromUtf16(ocspUrl.Ptr(), ocspUrl.Length());
    mDefaultUrl->setPlainText( defaultUrl );
    CleanupStack::PopAndDestroy( ocspUrlBuf );

    TInt allowUntrusted = 0;
    User::LeaveIfError( repository->Get( KSWInstallerAllowUntrusted, allowUntrusted ) );
    mAllowUntrusted->setChecked( allowUntrusted );

    CleanupStack::PopAndDestroy( repository );
}

void SwInstSettingsChanger::DoHandleSaveL()
{
    CRepository* repository = CRepository::NewL( KCRUidSWInstallerSettings );
    CleanupStack::PushL( repository );

    TInt ocspProcedure = mOcspProcedure->currentIndex();
    User::LeaveIfError( repository->Set( KSWInstallerOcspProcedure, ocspProcedure ) );

    HBufC* ocspUrlBuf = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
    TPtr ocspUrl( ocspUrlBuf->Des() );
    QString string = mDefaultUrl->toPlainText();
    TPtrC ptr( reinterpret_cast<const TText*>( string.constData() ) );
    ocspUrl.Copy( ptr );
    User::LeaveIfError( repository->Set( KSWInstallerOcspDefaultURL, ocspUrl ) );
    CleanupStack::PopAndDestroy( ocspUrlBuf );

    TInt allowUntrusted = mAllowUntrusted->isChecked();
    User::LeaveIfError( repository->Set( KSWInstallerAllowUntrusted, allowUntrusted ) );

    CleanupStack::PopAndDestroy( repository );
}

