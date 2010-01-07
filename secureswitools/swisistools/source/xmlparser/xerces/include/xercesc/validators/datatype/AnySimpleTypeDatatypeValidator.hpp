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
 * $Id: AnySimpleTypeDatatypeValidator.hpp 568078 2007-08-21 11:43:25Z amassari $
 */

#if !defined(ANYSIMPLETYPEDATATYPEVALIDATOR_HPP)
#define ANYSIMPLETYPEDATATYPEVALIDATOR_HPP

#include <xercesc/validators/datatype/DatatypeValidator.hpp>

XERCES_CPP_NAMESPACE_BEGIN

class VALIDATORS_EXPORT AnySimpleTypeDatatypeValidator : public DatatypeValidator
{
public:
    // -----------------------------------------------------------------------
    //  Public Constructor
    // -----------------------------------------------------------------------
	/** @name Constructor */
    //@{

    AnySimpleTypeDatatypeValidator
    (
        MemoryManager* const manager = XMLPlatformUtils::fgMemoryManager
    );

	//@}

    // -----------------------------------------------------------------------
    //  Public Destructor
    // -----------------------------------------------------------------------
	/** @name Destructor. */
    //@{

    virtual ~AnySimpleTypeDatatypeValidator();

	//@}

	virtual const RefArrayVectorOf<XMLCh>* getEnumString() const;

    // -----------------------------------------------------------------------
    // Getter methods
    // -----------------------------------------------------------------------
    /** @name Getter Functions */
    //@{

    /**
      * Returns whether the type is atomic or not
      */
    virtual bool isAtomic() const;

    //@}

    // -----------------------------------------------------------------------
    // Validation methods
    // -----------------------------------------------------------------------
    /** @name Validation Function */
    //@{

     /**
	   * Checks that the "content" string is valid datatype.
       * If invalid, a Datatype validation exception is thrown.
	   *
	   * @param  content   A string containing the content to be validated
	   *
	   */
	virtual void validate
                 (
                  const XMLCh*             const content
                ,       ValidationContext* const context = 0
                ,       MemoryManager*     const manager = XMLPlatformUtils::fgMemoryManager
                  );

    /**
      * Checks whether a given type can be used as a substitute
      *
      * @param  toCheck    A datatype validator of the type to be used as a
      *                    substitute
      *
      */

    bool isSubstitutableBy(const DatatypeValidator* const toCheck);

	 //@}

    // -----------------------------------------------------------------------
    // Compare methods
    // -----------------------------------------------------------------------
    /** @name Compare Function */
    //@{

    /**
      * Compares content in the Domain value vs. lexical value.
      *
      * @param  value1    string to compare
      *
      * @param  value2    string to compare
      *
      */
    int compare(const XMLCh* const value1, const XMLCh* const value2
        , MemoryManager* const manager = XMLPlatformUtils::fgMemoryManager
        );

    //@}

    /**
      * Returns an instance of the base datatype validator class
	  * Used by the DatatypeValidatorFactory.
      */
	virtual DatatypeValidator* newInstance
    (
        RefHashTableOf<KVStringPair>* const facets
        , RefArrayVectorOf<XMLCh>* const enums
        , const int finalSet
        , MemoryManager* const manager = XMLPlatformUtils::fgMemoryManager
    );

    /***
     * Support for Serialization/De-serialization
     ***/
    DECL_XSERIALIZABLE(AnySimpleTypeDatatypeValidator)

private:
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    AnySimpleTypeDatatypeValidator(const AnySimpleTypeDatatypeValidator&);
    AnySimpleTypeDatatypeValidator& operator=(const AnySimpleTypeDatatypeValidator&);
};


// ---------------------------------------------------------------------------
//  DatatypeValidator: Getters
// ---------------------------------------------------------------------------
inline bool AnySimpleTypeDatatypeValidator::isAtomic() const {

    return false;
}


// ---------------------------------------------------------------------------
//  DatatypeValidators: Compare methods
// ---------------------------------------------------------------------------
inline int AnySimpleTypeDatatypeValidator::compare(const XMLCh* const,
                                                   const XMLCh* const
                                                   , MemoryManager* const)
{
    return -1;
}

// ---------------------------------------------------------------------------
//  DatatypeValidators: Validation methods
// ---------------------------------------------------------------------------
inline bool
AnySimpleTypeDatatypeValidator::isSubstitutableBy(const DatatypeValidator* const)
{
    return true;
}

inline void 
AnySimpleTypeDatatypeValidator::validate(const XMLCh*             const
                                       ,       ValidationContext* const
                                       ,       MemoryManager*     const)
{
    return;
}

XERCES_CPP_NAMESPACE_END

#endif

/**
  * End of file AnySimpleTypeDatatypeValidator.hpp
  */

