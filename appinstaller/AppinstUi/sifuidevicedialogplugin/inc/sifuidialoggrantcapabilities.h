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
* Description: Grant capabilities dialog in SifUi
*
*/

#ifndef SIFUIDIALOGGRANTCAPABILITIES_H
#define SIFUIDIALOGGRANTCAPABILITIES_H

#include <HbDialog>


/**
 * SifUi dialog requesting the user to grant capabilities
 * for the application being installed.
 */
class SifUiDialogGrantCapabilities : public HbDialog
{
    Q_OBJECT

public:     // constructor and destructor
    SifUiDialogGrantCapabilities(const QString &applicationName,
        const QVariant &capabilities, QGraphicsItem *parent = 0);
    virtual ~SifUiDialogGrantCapabilities();

signals:
    void accepted();
    void rejected();

private slots:
    void handleAccepted();
    void handleRejected();
};

#endif // SIFUIDIALOGGRANTCAPABILITIES_H
