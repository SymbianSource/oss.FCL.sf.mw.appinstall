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
* Description: Software install notification content widget.
*
*/

#include "sifuidialogcontentwidget.h"
#include <QGraphicsLinearLayout>
#include <hbstackedwidget.h>
#include <hblabel.h>
#include <hbpushbutton.h>
#include <hbcombobox.h>
#include <hbprogressbar.h>
#include <QPixmap>
#include <qsysteminfo.h>                    // QSystemStorageInfo
QTM_USE_NAMESPACE
#if defined(Q_OS_SYMBIAN)
#include <driveinfo.h>                      // DriveInfo
#include <fbs.h>                            // CFbsBitmap
#endif  // Q_OS_SYMBIAN

const char KSifUiDefaultApplicationIcon[] = "qtg_large_application.svg";

const int KSifUiKilo = 1024;
const int KSifUiMega = 1024*1024;

const int KAppNameIndex = 0;
const int KAppSizeIndex = 1;

enum TSifUiDriveName {
    EUnknown,
    EPhoneMemory,
    EMassStorage,
    EMemoryCard
};


// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// fbsBitmap()
// ----------------------------------------------------------------------------
//
CFbsBitmap *fbsBitmap(int handle)
{
    CFbsBitmap *bitmap = 0;
#if defined(Q_OS_SYMBIAN)
    bitmap = new CFbsBitmap;
    if (bitmap) {
        if (!bitmap->Duplicate(handle)) {
            delete bitmap;
            bitmap = 0;
        }
    }
#endif // Q_OS_SYMBIAN
    return bitmap;
}

// ----------------------------------------------------------------------------
// driveName()
// ----------------------------------------------------------------------------
//
TSifUiDriveName driveName(const QChar& volume)
{
#if defined(Q_OS_SYMBIAN)
    int err = 0;
    TChar drive;
    err = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultPhoneMemory, drive);
    if (!err && volume.toAscii() == drive) {
        return EPhoneMemory;
    }
    err = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultMassStorage, drive);
    if (!err && volume.toAscii() == drive) {
        return EMassStorage;
    }
    err = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRemovableMassStorage, drive);
    if (!err && volume.toAscii() == drive) {
        return EMemoryCard;
    }
#endif  // Q_OS_SYMBIAN
    return EUnknown;
}


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::SifUiDialogContentWidget()
// ----------------------------------------------------------------------------
//
SifUiDialogContentWidget::SifUiDialogContentWidget(QGraphicsItem *parent,
        Qt::WindowFlags flags) : HbWidget(parent, flags),
        mAppIcon(0), mAppName(0), mAppSize(0),
        mMainLayout(0), mAppDetailsLayout(0), mStackedWidget(0),
        mMemorySelection(0), mProgressBar(0), mErrorText(0),
        mBitmap(0), mMask(0)
{
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::~SifUiDialogContentWidget()
// ----------------------------------------------------------------------------
//
SifUiDialogContentWidget::~SifUiDialogContentWidget()
{
    delete mBitmap;
    delete mMask;
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::constructFromParameters()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::constructFromParameters(const QVariantMap &parameters)
{
    Q_ASSERT(mMainLayout == 0);
    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical);

    // Application icon
    QGraphicsLinearLayout *iconAndAppLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    Q_ASSERT(mAppIcon == 0);
    mAppIcon = new HbLabel;
    updateAppIcon(parameters);
    iconAndAppLayout->addItem(mAppIcon);

    // Application name and version
    Q_ASSERT(mAppDetailsLayout == 0);
    mAppDetailsLayout = new QGraphicsLinearLayout(Qt::Vertical);
    Q_ASSERT(mAppName == 0);
    mAppName = new HbLabel(applicationName(parameters));
    mAppDetailsLayout->addItem(mAppName);

    // Application size
    Q_ASSERT(mAppSize == 0);
    updateAppSize(parameters);

    // Other application details
    if (parameters.contains(KSifUiApplicationDetails)) {
        addDetails(parameters.value(KSifUiApplicationDetails).toStringList());
    }
    iconAndAppLayout->addItem(mAppDetailsLayout);
    mMainLayout->addItem(iconAndAppLayout);

    // Memory selection, progress bar, and error text within a stacked widget
    Q_ASSERT(mStackedWidget == 0);
    mStackedWidget = new HbStackedWidget;

    Q_ASSERT(mMemorySelection == 0);
    mMemorySelection = new HbComboBox;
    connect(mMemorySelection, SIGNAL(currentIndexChanged(int)),
            this, SIGNAL(handleMemorySelectionChange(int)));
    mStackedWidget->addWidget(mMemorySelection);

    Q_ASSERT(mProgressBar == 0);
    mProgressBar = new HbProgressBar;
    mProgressBar->setRange(0,0);    // busy indicator by default
    mStackedWidget->addWidget(mProgressBar);

    Q_ASSERT(mErrorText == 0);
    mErrorText = new HbLabel;
    mStackedWidget->addWidget(mErrorText);

    mMainLayout->addItem(mStackedWidget);
    if (!updateErrorText(parameters) &&
        !updateProgressBar(parameters) &&
        !updateMemorySelection(parameters)) {
        mStackedWidget->hide();
    }

    setLayout(mMainLayout);
    }

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::updateFromParameters()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::updateFromParameters(const QVariantMap &parameters)
{
    // Application icon
    updateAppIcon(parameters);

    // Application name and version
    if (parameters.contains(KSifUiApplicationName)) {
        QString appNameStr = applicationName(parameters);
        if (mAppName) {
            if (mAppName->plainText() != appNameStr) {
                mAppName->setPlainText(appNameStr);
            }
        } else {
            HbLabel *appName = new HbLabel(appNameStr);
            mAppDetailsLayout->insertItem(KAppNameIndex, appName);
            mAppName = appName;
        }
    }

    // Application size
    updateAppSize(parameters);

    // Other application details
    if (parameters.contains(KSifUiApplicationDetails)) {
        removeDetails();
        addDetails(parameters.value(KSifUiApplicationDetails).toStringList());
    }

    // Stacked widgets: memory selection, progress bar and error text
    if (updateErrorText(parameters) ||
        updateProgressBar(parameters) ||
        updateMemorySelection(parameters)) {
        if (!mStackedWidget->isVisible()) {
            mStackedWidget->show();
        }
    } else {
        if (mStackedWidget->isVisible()) {
            mStackedWidget->hide();
        }
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::changeType()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::changeType(SifUiDeviceDialogType type)
{
    switch (type) {
        case SifUiConfirmationQuery:
            mStackedWidget->setCurrentWidget(mMemorySelection);
            break;
        case SifUiProgressNote:
            mStackedWidget->setCurrentWidget(mProgressBar);
            break;
        case SifUiCompleteNote:
            // TODO: remove mStackedWidget?
            break;
        case SifUiErrorNote:
            mStackedWidget->setCurrentWidget(mErrorText);
            break;
        default:
            break;
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::applicationName()
// ----------------------------------------------------------------------------
//
QString SifUiDialogContentWidget::applicationName() const
{
    if (mAppName) {
        return mAppName->plainText();
    }
    return QString();
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::isDefaultIconUsed()
// ----------------------------------------------------------------------------
//
bool SifUiDialogContentWidget::isDefaultIconUsed() const
{
    return (mBitmap != 0 && mMask != 0);
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::iconHandles()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::iconHandles(int &iconHandle, int &maskHandle) const
{
    if (mBitmap && mMask) {
        iconHandle = mBitmap->Handle();
        maskHandle = mMask->Handle();
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::progressInfo()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::progressInfo(int &finalValue, int &currentValue) const
{
    if (mProgressBar) {
        finalValue = mProgressBar->maximum();
        currentValue = mProgressBar->progressValue();
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::handleMemorySelectionChange()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::handleMemorySelectionChange(int selectedIndex)
{
    QChar selectedDrive = mDriveLetterList[selectedIndex][0];
    // TODO: save selected drive to cenrep

    emit memorySelectionChanged( selectedDrive );
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::applicationName()
// ----------------------------------------------------------------------------
//
QString SifUiDialogContentWidget::applicationName(const QVariantMap &parameters)
{
    QString appName = "";
    if (parameters.contains(KSifUiApplicationName)) {
        QString nameParam = parameters.value(KSifUiApplicationName).toString();
        if (parameters.contains(KSifUiApplicationVersion)) {
            QString versionParam = parameters.value(KSifUiApplicationVersion).toString();
            //: Template for application name and version in SW install confirmation query.
            //: %1 is the application name and %2 is the version number.
            //: Version number consist of major, minor, and build numbers.
            //: For example: "Chess (v 1.01(123))".
            // TODO: enable when translations ready
            //appName = hbTrId("txt_sisxui_install_appname_version").arg(nameParam, versionParam);
            appName = tr("%1 (v %2)").arg(nameParam, versionParam);
        } else {
            appName = nameParam;
        }
    }
    return appName;
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::applicationSize()
// ----------------------------------------------------------------------------
//
QString SifUiDialogContentWidget::applicationSize(const QVariantMap &parameters)
{
    QString appSize = "";
    if (parameters.contains(KSifUiApplicationSize)) {
        uint size = parameters.value(KSifUiApplicationSize).toUInt();
        if (size > 0) {
            if (size > KSifUiMega) {
                //: Application size in SW install confirmation query, %1 is in megabytes
                // TODO: enable when translations ready
                //appSize = hbTrId("txt_sisxui_install_appsize_mb").arg(size/KSifUiMega);
                appSize = tr("%1 MB").arg(size/KSifUiMega);
            } else if(size > KSifUiKilo) {
                //: Application size in SW install confirmation query, %1 is in kilobytes
                // TODO: enable when translations ready
                //appSize = hbTrId("txt_sisxui_install_appsize_kb").arg(size/KSifUiKilo);
                appSize = tr("%1 kB").arg(size/KSifUiKilo);
            } else {
                //: Application size in SW install confirmation query, %1 is in bytes
                // TODO: enable when translations ready
                //appSize = hbTrId("txt_sisxui_install_appsize_b").arg(size);
                appSize = tr("%1 B").arg(size);
            }
        }
    }
    return appSize;
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::removeDetails()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::removeDetails()
{
    int firstDetailsItemIndex = ( mAppName ? 1 : 0 ) + ( mAppSize ? 1 : 0 );
    QGraphicsLayoutItem *item = 0;
    for (int i = 0; i < mAppDetailsLayout->count(); ++i) {
        if (i >= firstDetailsItemIndex) {
            item = mAppDetailsLayout->itemAt(i);
            mAppDetailsLayout->removeAt(i);
            delete item;
            item = 0;
        }
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::addDetails()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::addDetails(const QStringList &detailList)
{
    QStringListIterator detailsIter(detailList);
    while (detailsIter.hasNext())
        {
        addDetail(detailsIter.next());
        }
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::addDetail()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::addDetail(const QString &detailText)
{
    Q_ASSERT(mAppDetailsLayout != 0);
    HbLabel *detailItem = new HbLabel(detailText);
    mAppDetailsLayout->addItem(detailItem);
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::updateAppIcon()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::updateAppIcon(const QVariantMap &parameters)
{
    Q_ASSERT(mAppIcon != 0);
    if (parameters.contains(KSifUiApplicationIconHandle) &&
        parameters.contains(KSifUiApplicationIconMaskHandle)) {
        int iconHandle = parameters.value(KSifUiApplicationIconHandle).toInt();
        int maskHandle = parameters.value(KSifUiApplicationIconMaskHandle).toInt();
        QPixmap pixmap;
        delete mBitmap;
        mBitmap = fbsBitmap(iconHandle);
        delete mMask;
        mMask = fbsBitmap(maskHandle);
        if (mBitmap && mMask) {
            pixmap = QPixmap::fromSymbianCFbsBitmap(mBitmap);
            pixmap.setAlphaChannel(QPixmap::fromSymbianCFbsBitmap(mMask));
        }
        mAppIcon->setIcon(HbIcon(pixmap));
    } else {
        if (mAppIcon->icon().isNull()) {
            mAppIcon->setIcon(HbIcon(KSifUiDefaultApplicationIcon));
        }
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::updateAppSize()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::updateAppSize(const QVariantMap &parameters)
{
    if (parameters.contains(KSifUiApplicationSize)) {
        QString appSizeStr = applicationSize(parameters);
        if (appSizeStr.length() > 0) {
            if (mAppSize) {
                if (mAppSize->plainText() != appSizeStr) {
                    mAppSize->setPlainText(appSizeStr);
                }
            } else {
                HbLabel *appSize = new HbLabel(appSizeStr);
                mAppDetailsLayout->insertItem(KAppSizeIndex, appSize);
                mAppSize = appSize;
            }
        }
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::updateMemorySelection()
// ----------------------------------------------------------------------------
//
bool SifUiDialogContentWidget::updateMemorySelection(const QVariantMap &parameters)
{
    if (parameters.contains(KSifUiMemorySelection)) {
        QString drives = parameters.value(KSifUiMemorySelection).toString();
        mDriveLetterList = drives.split(",");

        QStringList driveList;
        QSystemStorageInfo info;
        QStringList volumeList = info.logicalDrives();
        foreach (QString volume, volumeList) {
            if (mDriveLetterList.contains(volume)) {
                qlonglong size = info.availableDiskSpace(volume);
                switch (driveName(volume[0])) {
                case EPhoneMemory:
                    if (size > KSifUiMega) {
                        //: Drive name for internal phone memory with megabytes of free space.
                        //: %1 is replaced with drive letter (usually 'C')
                        //: %2 is replaced with available free space (in megabytes, MB)
                        //TODO: use hbTrId("txt_sisxui_device_memory_mb") when available
                        driveList.append(tr("%1: Device (%L2 MB free)"
                            ).arg(volume).arg(size/KSifUiMega));
                    } else {
                        //: Drive name for internal phone memory with kilobytes of free space.
                        //: %1 is replaced with drive letter (usually 'C')
                        //: %2 is replaced with available free space (in kilobytes, kB)
                        //TODO: use hbTrId("txt_sisxui_device_memory_kb") when available
                        driveList.append(tr("%1: Device (%L2 kB free)"
                            ).arg(volume).arg(size/KSifUiKilo));
                    }
                    break;
                case EMassStorage:
                    if (size > KSifUiMega) {
                        //: Drive name for mass storage with megabytes of free space.
                        //: %1 is replaced with drive letter (usually 'E')
                        //: %2 is replaced with available free space (in megabytes, MB)
                        // TODO: use hbTrId("txt_sisxui_mass_storage_mb") when available
                        driveList.append(tr("%1: Mass.mem (%L2 MB free)"
                            ).arg(volume).arg(size/KSifUiMega));
                    } else {
                        //: Drive name for mass storage with kilobytes of free space.
                        //: %1 is replaced with drive letter (usually 'E')
                        //: %2 is replaced with available free space (in kilobytes, kB)
                        // TODO: use hbTrId("txt_sisxui_mass_storage_kb") when available
                        driveList.append(tr("%1: Mass.mem (%L2 kB free)"
                            ).arg(volume).arg(size/KSifUiKilo));
                    }
                    break;
                case EMemoryCard:
                    if (size > KSifUiMega) {
                        //: Drive name for memory card with megabytes of free space.
                        //: %1 is replaced with drive letter (usually 'F')
                        //: %2 is replaced with available free space (in megabytes, MB)
                        // TODO: use hbTrId("txt_sisxui_memory_card_mb") when available
                        driveList.append(tr("%1: Mem.card (%L2 MB free)"
                            ).arg(volume).arg(size/KSifUiMega));
                    } else {
                        //: Drive name for memory card with kilobytes of free space.
                        //: %1 is replaced with drive letter (usually 'F')
                        //: %2 is replaced with available free space (in kilobytes, kB)
                        // TODO: use hbTrId("txt_sisxui_memory_card_kb") when available
                        driveList.append(tr("%1: Mem.card (%L2 kB free)"
                            ).arg(volume).arg(size/KSifUiKilo));
                    }
                    break;
                default:
                    break;
                }
            }
        }

        mMemorySelection->setItems(driveList);
        mStackedWidget->setCurrentWidget(mMemorySelection);

        // TODO: set selected item, read the default from cenrep

        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::updateProgressBar()
// ----------------------------------------------------------------------------
//
bool SifUiDialogContentWidget::updateProgressBar(const QVariantMap &parameters)
{
    bool progressBarChanged = false;
    if (parameters.contains(KSifUiProgressNoteFinalValue)) {
        mProgressBar->setMaximum(parameters.value(KSifUiProgressNoteFinalValue).toInt());
        progressBarChanged = true;
    }
    if (parameters.contains(KSifUiProgressNoteValue)) {
        int newValue = mProgressBar->progressValue();
        newValue += parameters.value(KSifUiProgressNoteValue).toInt();
        mProgressBar->setProgressValue(newValue);
        progressBarChanged = true;
    }
    if (progressBarChanged) {
        mStackedWidget->setCurrentWidget(mProgressBar);
    }
    return progressBarChanged;
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::updateErrorText()
// ----------------------------------------------------------------------------
//
bool SifUiDialogContentWidget::updateErrorText(const QVariantMap &parameters)
{
    if (parameters.contains(KSifUiErrorCode)) {
        // TODO: proper error texts
        QString errorText = tr("Error %1").arg(parameters.value(KSifUiErrorCode).toInt());
        mErrorText->setPlainText(errorText);
        mStackedWidget->setCurrentWidget(mErrorText);
        return true;
    }
    return false;
}



