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
* Description: SIF UI dialog content widget.
*
*/

#ifndef SIFUIDIALOGCONTENTWIDGET_H
#define SIFUIDIALOGCONTENTWIDGET_H

#include <hbwidget.h>
#include <sifuidialogdefinitions.h>

class HbLabel;
class QGraphicsLinearLayout;
class HbStackedWidget;
class HbComboBox;
class HbProgressBar;
class CFbsBitmap;


class SifUiDialogContentWidget : public HbWidget
{
    Q_OBJECT

public:
    SifUiDialogContentWidget(QGraphicsItem *parent=0, Qt::WindowFlags flags=0);
    virtual ~SifUiDialogContentWidget();

    void constructFromParameters(const QVariantMap &parameters);
    void updateFromParameters(const QVariantMap &parameters);
    void changeType(SifUiDeviceDialogType type);

signals:
    void memorySelectionChanged(const QChar &driveLetter);

private slots:
	void handleMemorySelectionChange(int selectedIndex);

private:
    Q_DISABLE_COPY(SifUiDialogContentWidget)
    QString applicationName(const QVariantMap &parameters);
    QString applicationSize(const QVariantMap &parameters);
    void removeDetails();
    void addDetails(const QStringList &detailList);
    void addDetail(const QString &detailText);
    void updateAppIcon(const QVariantMap &parameters);
    void updateAppSize(const QVariantMap &parameters);
    bool updateMemorySelection(const QVariantMap &parameters);
    bool updateProgressBar(const QVariantMap &parameters);
    bool updateErrorText(const QVariantMap &parameters);

private:    // data
    HbLabel *mAppIcon;
    HbLabel *mAppName;
    HbLabel *mAppSize;
    QGraphicsLinearLayout *mMainLayout;
    QGraphicsLinearLayout *mAppDetailsLayout;
    HbStackedWidget *mStackedWidget;
    HbComboBox *mMemorySelection;
    QStringList mDriveLetterList;
    HbProgressBar *mProgressBar;
    HbLabel *mErrorText;
    CFbsBitmap *mBitmap;
    CFbsBitmap *mMask;
};

#endif // SIFUIDIALOGCONTENTWIDGET_H
