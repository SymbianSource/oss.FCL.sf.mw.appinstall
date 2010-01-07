/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#if (__OS400_TGTVRM__>=510)                               /* @01a */
    #pragma datamodel(P128)                               /* @01a */
#endif                                                    /* @01a */

/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Id: OS400Defs.hpp 568078 2007-08-21 11:43:25Z amassari $
 */


// ---------------------------------------------------------------------------
//  AIX runs in big endian mode
// ---------------------------------------------------------------------------
#define ENDIANMODE_BIG


// ---------------------------------------------------------------------------
//  And define our file handle abstraction
// ---------------------------------------------------------------------------
typedef void* FileHandle;
#if (__OS400_TGTVRM__>=510)                                /* @01a */  
     #pragma datamodel(pop)                                /* @01a */ 
#endif                                                     /* @01a */
