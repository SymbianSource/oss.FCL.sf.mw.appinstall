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
 * $Id: ValidationContext.hpp 568078 2007-08-21 11:43:25Z amassari $
 */

#if !defined(VALIDATION_CONTEXT_HPP)
#define VALIDATION_CONTEXT_HPP

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/RefHashTableOf.hpp>
#include <xercesc/util/NameIdPool.hpp>
#include <xercesc/util/XMemory.hpp>

XERCES_CPP_NAMESPACE_BEGIN

class XMLRefInfo;
class DTDEntityDecl;
class DatatypeValidator;
class ElemStack;

class XMLPARSER_EXPORT ValidationContext : public XMemory
{
public :
    // -----------------------------------------------------------------------
    /** @name Virtual destructor for derived classes */
    // -----------------------------------------------------------------------
    //@{

    /**
      * virtual destructor
      *
      */
    virtual ~ValidationContext(){};
    //@}

    // -----------------------------------------------------------------------
    /** @name The ValidationContext Interface */
    // -----------------------------------------------------------------------
    //@{

    /**
      * IDRefList
      *
      */
    virtual RefHashTableOf<XMLRefInfo>*  getIdRefList() const = 0;

    virtual void                         setIdRefList(RefHashTableOf<XMLRefInfo>* const) = 0;

    virtual void                         clearIdRefList() = 0;

    virtual void                         addId(const XMLCh * const ) = 0;

    virtual void                         addIdRef(const XMLCh * const ) = 0;

    virtual void                         toCheckIdRefList(bool) = 0;

    /**
      * EntityDeclPool
      *
      */
    virtual const NameIdPool<DTDEntityDecl>* getEntityDeclPool() const = 0;

    virtual const NameIdPool<DTDEntityDecl>* setEntityDeclPool(const NameIdPool<DTDEntityDecl>* const) = 0;    
           
    virtual void                             checkEntity(const XMLCh * const ) const = 0 ;

    /**
      * Union datatype handling
      *
      */

    virtual DatatypeValidator * getValidatingMemberType() const = 0 ;
    virtual void setValidatingMemberType(DatatypeValidator * validatingMemberType) = 0 ;

    /**
      * QName datatype handling
      * Create default implementations for source code compatibility
      */
    virtual bool isPrefixUnknown(XMLCh* /* prefix */) { return true; };
    virtual void setElemStack(ElemStack* /* elemStack */) {};
    //@}

   
protected :
    // -----------------------------------------------------------------------
    /**  Hidden Constructors */
    // -----------------------------------------------------------------------
    //@{
    ValidationContext(MemoryManager* const memMgr = XMLPlatformUtils::fgMemoryManager)
    :fMemoryManager(memMgr)
    {
    };
    //@}

    // -----------------------------------------------------------------------
    //  Data members
    //
    //  fMemoryManager
    //      Pluggable memory manager for dynamic allocation/deallocation.
    // -----------------------------------------------------------------------
    MemoryManager*                    fMemoryManager;

private :
    // -----------------------------------------------------------------------
    /** name  Unimplemented copy constructor and operator= */
    // -----------------------------------------------------------------------
    //@{
    ValidationContext(const ValidationContext& );
    ValidationContext& operator=(const ValidationContext& );
    //@}

};

XERCES_CPP_NAMESPACE_END

#endif
