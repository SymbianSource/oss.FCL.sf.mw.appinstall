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
* Description: Content widget for grant capabilities dialog in SifUi
*
*/

#ifndef SIFUIDIALOGGRANTCAPABILITIESCONTENT_H
#define SIFUIDIALOGGRANTCAPABILITIESCONTENT_H

#include <HbWidget>


/**
 * Content widget for SifUiDialogGrantCapabilities dialog.
 */
class SifUiDialogGrantCapabilitiesContent : public HbWidget
{
    Q_OBJECT

public:     // constructor and destructor
    SifUiDialogGrantCapabilitiesContent(const QString &applicationName,
        const QVariant &capabilities, QGraphicsItem *parent = 0);
    virtual ~SifUiDialogGrantCapabilitiesContent();

signals:
    void accepted();
    void rejected();

private slots:
    void viewDetails();

private:    // new functions
    void setCapabilities(const QVariant &capabilities);
#ifdef Q_OS_SYMBIAN
    void setCapabilitiesSymbianL( const TDesC8 &aBuf );
    QString capabilityName( const TCapability aCapability );
#endif // Q_OS_SYMBIAN

private:    // data
    QStringList mCapabilities;
    bool mIsCapabilitiesValid;
};

#endif // SIFUIDIALOGGRANTCAPABILITIESCONTENT_H
