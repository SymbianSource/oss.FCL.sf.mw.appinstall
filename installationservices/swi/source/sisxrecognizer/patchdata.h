/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include <e32base.h>

/**
This 8-bit variable is used in CApaSisxRecognizer. Its default value is zero.
It is used to override the default behaviour, i.e. recognise .sis file 
extention and set iConfidence to ECertain; all other extensions will set 
iConfidence to EPossible.
If KSisxRecognizerConst is set to non-zero, CApaSisxRecognizer would set 
iConfidence to ECertain for .sis, EPossible for .sisx and ENotRecognized for 
all other extensions.

This variable can be changed at ROM build time using the patchdata obey keyword.
See Symbian OS Developer Library documentation on how to patch DLL data exports.
@file
@internalComponent

*/
IMPORT_C extern const TUint8 KSisxRecognizerConst;
