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
 * $Id: SynchronizedStringPool.hpp 568078 2007-08-21 11:43:25Z amassari $
 */

#if !defined(SYNCHRONIZEDSTRINGPOOL_HPP)
#define SYNCHRONIZEDSTRINGPOOL_HPP

#include <xercesc/framework/MemoryManager.hpp>
#include <xercesc/util/StringPool.hpp>
#include <xercesc/util/Mutexes.hpp>

XERCES_CPP_NAMESPACE_BEGIN

//
//  This class provides a synchronized string pool implementation.
//  This will necessarily be slower than the regular StringPool, so it
//  should only be used when updates need to be made in a thread-safe
//  way.  Updates will be made on datastructures local to this object;
//  all queries that don't involve mutation will first be directed at
//  the StringPool implementation with which this object is
//  constructed.
class XMLUTIL_EXPORT XMLSynchronizedStringPool : public XMLStringPool
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XMLSynchronizedStringPool
    (
        const XMLStringPool *  constPool
        , const unsigned int   modulus = 109
        , MemoryManager* const manager = XMLPlatformUtils::fgMemoryManager
    );
    virtual ~XMLSynchronizedStringPool();


    // -----------------------------------------------------------------------
    //  Pool management methods
    // -----------------------------------------------------------------------
    virtual unsigned int addOrFind(const XMLCh* const newString);
    virtual bool exists(const XMLCh* const newString) const;
    virtual bool exists(const unsigned int id) const;
    virtual void flushAll();
    virtual unsigned int getId(const XMLCh* const toFind) const;
    virtual const XMLCh* getValueForId(const unsigned int id) const;
    virtual unsigned int getStringCount() const;


private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    XMLSynchronizedStringPool(const XMLSynchronizedStringPool&);
    XMLSynchronizedStringPool& operator=(const XMLSynchronizedStringPool&);


    // -----------------------------------------------------------------------
    // private data members
    //  fConstPool
    //      the pool whose immutability we're protecting
    // fMutex
    //      mutex to permit synchronous updates of our StringPool
    const XMLStringPool* fConstPool;
    XMLMutex             fMutex;
};

XERCES_CPP_NAMESPACE_END

#endif