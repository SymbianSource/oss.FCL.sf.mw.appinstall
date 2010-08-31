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
#include <qvaluespacepublisher.h>           // QValueSpacePublisher
#include <qvaluespacesubscriber.h>          // QValueSpaceSubscriber
#if defined(Q_OS_SYMBIAN)
#include <driveinfo.h>                      // DriveInfo
#include <fbs.h>                            // CFbsBitmap
#endif  // Q_OS_SYMBIAN

QTM_USE_NAMESPACE

const char KSifUiDefaultApplicationIcon[] = "qtg_large_application";
const char KSifUiErrorIcon[] = "qtg_large_warning";

const int KSifUiKilo = 1024;
const int KSifUiMega = 1024*1024;

const int KAppNameIndex = 0;
const int KAppSizeIndex = 1;

enum TSifUiDriveName {
    EPhoneMemory,
    EMassStorage,
    EMemoryCard,
    EOtherDrive
};

const char KInitialDefaultDrive = 'C';

// Path and value name for QValueSpacePublisher/QValueSpaceSubscriber.
const QString KSifUiCenRepPath = "/KCRUIDSifUiDefaults";
const QString KSifUiCenRepDefaultDrive = "KCRUIDSifUiDefaultDrive";

// TODO: replace with proper logging
#ifdef _DEBUG
#define FLOG1(x)        qDebug() << (x)
#define FLOG2(x,y)      qDebug() << (x) << (y)
#define FLOG3(x,y,z)    qDebug() << (x) << (y) << (z)
#else
#define FLOG1(x)
#define FLOG2(x,y)
#define FLOG3(x,y,z)
#endif


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
    return EOtherDrive;
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
        mBitmap(0), mMask(0), mPublisher(0), mSubscriber(0)
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
    FLOG1("SifUiDialogContentWidget::constructFromParameters");

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
    createAppName(applicationName(parameters));
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
    FLOG1("SifUiDialogContentWidget::updateFromParameters");

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
            createAppName(appNameStr);
            mAppDetailsLayout->insertItem(KAppNameIndex, mAppName);
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
    FLOG2("SifUiDialogContentWidget::changeType", type);

    switch (type) {
        case SifUiConfirmationQuery:
            mStackedWidget->setCurrentWidget(mMemorySelection);
            break;
        case SifUiProgressNote:
            mStackedWidget->setCurrentWidget(mProgressBar);
            break;
        case SifUiCompleteNote:
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
// SifUiDialogContentWidget::handleMemorySelectionChange()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::handleMemorySelectionChange(int selectedIndex)
{
    FLOG2("SifUiDialogContentWidget::handleMemorySelectionChange", selectedIndex);

    QChar selectedDrive = mDriveLetterList[selectedIndex][0];
    saveSelectedDriveAsDefault( selectedDrive );
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
            //: Custom layout ID parent. Template for application name and version in SW install
            //: confirmation query. %1 is the application name and %2 is the version number.
            //: Version number consist of major, minor, and build numbers.
            //: For example: "Chess (v 1.01(123))".
            // TODO: use hbTrId when arg() starts to work with limited length arguments like "%[99]1"
            //appName = hbTrId("txt_installer_list_appname_version").arg(nameParam, versionParam);
            appName = QString("%1 (v %2)").arg(nameParam, versionParam);
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
        bool ok = false;
        uint size = parameters.value(KSifUiApplicationSize).toUInt(&ok);
        if (ok) {
            if (size > KSifUiMega) {
                //: Application size in SW install confirmation query, %1 is in megabytes
                appSize = hbTrId("txt_installer_list_appsize_mb").arg(size/KSifUiMega);
            } else if(size > KSifUiKilo) {
                //: Application size in SW install confirmation query, %1 is in kilobytes
                appSize = hbTrId("txt_installer_list_appsize_kb").arg(size/KSifUiKilo);
            } else {
                //: Application size in SW install confirmation query, %1 is in bytes
                appSize = hbTrId("txt_installer_list_appsize_b").arg(size);
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
    detailItem->setTextWrapping(Hb::TextWordWrap);
    detailItem->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
    mAppDetailsLayout->addItem(detailItem);
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::createAppName()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::createAppName(const QString &appName)
{
    Q_ASSERT(mAppName == 0);
    mAppName = new HbLabel(appName);
    mAppName->setTextWrapping(Hb::TextWordWrap);
    mAppName->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::updateAppIcon()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::updateAppIcon(const QVariantMap &parameters)
{
    Q_ASSERT(mAppIcon != 0);

    // TODO: proper icon handling
    if (parameters.contains(KSifUiDialogType) &&
        (parameters.value(KSifUiDialogType).toInt() == SifUiErrorNote)) {
        mAppIcon->setIcon(HbIcon(KSifUiErrorIcon));
    } else if (parameters.contains(KSifUiApplicationIconHandle) &&
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
                appSize->setTextWrapping(Hb::TextWordWrap);
                appSize->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
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

        QChar defaultDrive = readDefaultSelectedDrive();
        int defaultDriveIndex = 0;

        QStringList driveList;
        QSystemStorageInfo info;
        QStringList volumeList = info.logicalDrives();
        foreach (QString volume, volumeList) {
            if (mDriveLetterList.contains(volume)) {
                qlonglong size = info.availableDiskSpace(volume);

                QChar driveLetter(volume[0]);
                switch (driveName(driveLetter)) {
                case EPhoneMemory:
                    if (size > KSifUiMega) {
                        //: Drive name for internal phone memory with megabytes of free space.
                        //: %1 is replaced with drive letter (usually 'C')
                        //: %2 is replaced with available free space (in megabytes, MB)
                        //TODO: enable when available
                        //driveList.append(hbTrId("txt_installer_device_memory_mb")
                        //    .arg(volume).arg(size/KSifUiMega));
                        driveList.append(tr("%1: Phone mem. (%L2 MB)"
                            ).arg(volume).arg(size/KSifUiMega));
                    } else {
                        //: Drive name for internal phone memory with kilobytes of free space.
                        //: %1 is replaced with drive letter (usually 'C')
                        //: %2 is replaced with available free space (in kilobytes, kB)
                        //TODO: enable when available
                        //driveList.append(hbTrId("txt_installer_device_memory_kb")
                        //    .arg(volume).arg(size/KSifUiKilo));
                        driveList.append(tr("%1: Phone mem. (%L2 kB)"
                            ).arg(volume).arg(size/KSifUiKilo));
                    }
                    break;

                case EMassStorage:
                    if (size > KSifUiMega) {
                        //: Drive name for mass storage with megabytes of free space.
                        //: %1 is replaced with drive letter (usually 'E')
                        //: %2 is replaced with available free space (in megabytes, MB)
                        // TODO: enable when available
                        //driveList.append(hbTrId("txt_installer_mass_storage_mb")
                        //    .arg(volume).arg(size/KSifUiMega));
                        driveList.append(tr("%1: Mass.mem (%L2 MB)"
                            ).arg(volume).arg(size/KSifUiMega));
                    } else {
                        //: Drive name for mass storage with kilobytes of free space.
                        //: %1 is replaced with drive letter (usually 'E')
                        //: %2 is replaced with available free space (in kilobytes, kB)
                        // TODO: enable when available
                        //driveList.append(hbTrId("txt_installer_mass_storage_kb")
                        //    .arg(volume).arg(size/KSifUiKilo));
                        driveList.append(tr("%1: Mass.mem (%L2 kB)"
                            ).arg(volume).arg(size/KSifUiKilo));
                    }
                    break;

                case EMemoryCard:
                    if (size > KSifUiMega) {
                        //: Drive name for memory card with megabytes of free space.
                        //: %1 is replaced with drive letter (usually 'F')
                        //: %2 is replaced with available free space (in megabytes, MB)
                        // TODO: enable when available
                        //driveList.append(hbTrId("txt_installer_memory_card_mb")
                        //    .arg(volume).arg(size/KSifUiMega));
                        driveList.append(tr("%1: Mem.card (%L2 MB)"
                            ).arg(volume).arg(size/KSifUiMega));
                    } else {
                        //: Drive name for memory card with kilobytes of free space.
                        //: %1 is replaced with drive letter (usually 'F')
                        //: %2 is replaced with available free space (in kilobytes, kB)
                        // TODO: enable when available
                        //driveList.append(hbTrId("txt_installer_memory_card_kb")
                        //    .arg(volume).arg(size/KSifUiKilo));
                        driveList.append(tr("%1: Mem.card (%L2 kB)"
                            ).arg(volume).arg(size/KSifUiKilo));
                    }
                    break;

                case EOtherDrive:
                default:
                    if (size > KSifUiMega) {
                        //: Generic drive name for other removable drives, like
                        //: USB memories attached via USB OTG adapter.
                        // TODO: proper localisation needed
                        driveList.append(tr("%1: Drive (%L2 MB)"
                            ).arg(volume).arg(size/KSifUiMega));
                    } else {
                        //: Generic drive name for other removable drives, like
                        //: USB memories attached via USB OTG adapter.
                        // TODO: proper localisation needed
                        driveList.append(tr("%1: Drive (%L2 kB)"
                            ).arg(volume).arg(size/KSifUiKilo));
                    }
                    break;
                }

                if (driveLetter == defaultDrive) {
                    defaultDriveIndex = driveList.count() - 1;
                }
            }
        }

        disconnect(mMemorySelection, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleMemorySelectionChange(int)));
        mMemorySelection->setItems(driveList);
        if (defaultDriveIndex) {
            FLOG2("SifUiDialogContentWidget::updateMemorySelection, setCurrentIndex",
                defaultDriveIndex);
            mMemorySelection->setCurrentIndex(defaultDriveIndex);
        }
        connect(mMemorySelection, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleMemorySelectionChange(int)));
        mStackedWidget->setCurrentWidget(mMemorySelection);

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
        bool ok = false;
        newValue += parameters.value(KSifUiProgressNoteValue).toInt(&ok);
        if (ok) {
            mProgressBar->setProgressValue(newValue);
            progressBarChanged = true;
        }
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
    // TODO: move default error messages (category based) here
    if (parameters.contains(KSifUiErrorMessage)) {
        QString errorText = parameters.value(KSifUiErrorMessage).toString();
        mErrorText->setPlainText(errorText);
        mStackedWidget->setCurrentWidget(mErrorText);
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::saveSelectedDriveAsDefault()
// ----------------------------------------------------------------------------
//
void SifUiDialogContentWidget::saveSelectedDriveAsDefault(const QChar& drive)
{
    if (drive != readDefaultSelectedDrive()) {
        if (!mPublisher) {
            mPublisher = new QValueSpacePublisher(KSifUiCenRepPath, this);
        }
        FLOG2("SifUiDialogContentWidget::saveSelectedDriveAsDefault", drive);
        // QValueSpacePublisher supports integer and byte array types in Symbian
        int asciiValue = drive.toAscii();
        mPublisher->setValue(KSifUiCenRepDefaultDrive, asciiValue);
        mPublisher->sync();
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogContentWidget::readDefaultSelectedDrive()
// ----------------------------------------------------------------------------
//
QChar SifUiDialogContentWidget::readDefaultSelectedDrive()
{
    QChar selectedDrive = KInitialDefaultDrive;

    if (!mSubscriber) {
        mSubscriber = new QValueSpaceSubscriber(KSifUiCenRepPath, this);
    }
    QVariant variant = mSubscriber->value(KSifUiCenRepDefaultDrive);
    if (variant.isValid() && !variant.isNull()) {
        bool ok = false;
        int asciiValue = variant.toInt(&ok);
        if (ok) {
            selectedDrive = QChar(asciiValue);
        }
    }

    FLOG2("SifUiDialogContentWidget::readDefaultSelectedDrive", selectedDrive);
    return selectedDrive;
}

