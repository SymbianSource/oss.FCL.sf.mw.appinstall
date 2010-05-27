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
* Description: Unit test implementation for SIF UI device dialog.
*
*/

#include "ut_sifuidevicedialog.h"
#include <sifuidialog.h>


// ---------------------------------------------------------------------------
// TestSifUiDeviceDialog::initTestCase()
// ---------------------------------------------------------------------------
//
void TestSifUiDeviceDialog::initTestCase()
    {
    }

// ---------------------------------------------------------------------------
// TestSifUiDeviceDialog::init()
// ---------------------------------------------------------------------------
//
void TestSifUiDeviceDialog::init()
    {
    }

// ---------------------------------------------------------------------------
// TestSifUiDeviceDialog::cleanup()
// ---------------------------------------------------------------------------
//
void TestSifUiDeviceDialog::cleanup()
    {
    }

// ---------------------------------------------------------------------------
// TestSifUiDeviceDialog::cleanupTestCase()
// ---------------------------------------------------------------------------
//
void TestSifUiDeviceDialog::cleanupTestCase()
    {
    }

// ---------------------------------------------------------------------------
// TestSifUiDeviceDialog::showInfoNote()
// ---------------------------------------------------------------------------
//
void TestSifUiDeviceDialog::showInfoNote()
    {
    QVariantMap params;
    SifUiDialog dlg(params);
    bool isConstructOk = dlg.constructDialog(params);
    QVERIFY(isConstructOk);
    }

// ---------------------------------------------------------------------------
// main()
// ---------------------------------------------------------------------------
//
QTEST_MAIN(TestSifUiDeviceDialog)
#include "moc_ut_sifuidevicedialog.cpp"

 
