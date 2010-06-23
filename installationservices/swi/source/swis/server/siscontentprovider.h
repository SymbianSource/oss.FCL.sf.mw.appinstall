/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @internalComponent 
*/

#ifndef __SISCONTENTPROVIDER_H__
#define __SISCONTENTPROVIDER_H__

#include "sisuihandler.h"
#include "expressionevaluator.h"

namespace Swi
{
  
  namespace Sis
  {
    class CController;
    class CInfo;
    class CInstallBlock;
  }

  using namespace Swi::Sis;

/**
 * This class is a wrapper around Swi::Sis::CController. It is used 
 * by Swi::CInstallationPlanner to access the information needed to build
 * a Swi::InstallationPlan.
 * It must provide some business methods to ease the retrieval of information
 * from the sisx file for the planner to use.
 *
 * @internalComponent
 * @released
 */
class CContentProvider : public CBase
 {
 public: // Life-Cycle methods
	/**
	 * Constructs a new content provider object and puts it on the cleanup 
	 * stack.
	 *
	 * @param aController SISController object
	 *
	 * @return A new content provider object on the cleanup stack
	 */
    static CContentProvider* NewLC(const CController& aController);

	/**
	 * Constructs a new content provider object.
	 *
	 * @param aController SISController object
	 *
	 * @return A new content provider object
	 */
    static CContentProvider* NewL(const CController& aController);
   ~CContentProvider();

 public: // Business methods

   /**
    * Accessor for the underlying CController. 
    *
    * @return The underlying controller.
    */
   const CController& Controller() const;

   /**
    * Returns a TAppInfo structure localized to the default language
    * of the device. If that is not available the first language of the
    * SISX file will be used.
    *
    * @return An appropriately localized TAppInfo structure. 
    */
   TAppInfo DefaultLanguageAppInfoL() const;

   /**
    * Builds a TAppInfo structure localized to the given language index.
    * Note that the langauge index must be valid, if this is not the case
    * the function will leave.
    *
    * @param aLanguageIndex The index of the language in the SISX file.
    * @return An appropriately localized TAppInfo structure. 
    */
   TAppInfo LocalizedAppInfoL(TInt aLanguageIndex) const;


   /**
    * Returns a TAppInfo structure localized to the default language
    * of the device, for a given CInfo object. If that is not available 
    * the first language of the SISX file will be used.
    *
    * @param aInfo	The CInfo object to make the application description from.
    * @return An appropriately localized TAppInfo structure. 
    */
   TAppInfo DefaultLanguageAppInfoL(const CInfo& aInfo) const;

  /**
    * Builds a TAppInfo structure localized to the given language index, for
    * a given CInfo object.
    * Note that the langauge index must be valid, if this is not the case
    * the function will leave.
    *
    * @param aInfo	The CInfo object to make the application description from.
    * @param aLanguageIndex The index of the language in the SISX file.
    * @return An appropriately localized TAppInfo structure. 
    */
	TAppInfo LocalizedAppInfoL(const CInfo& aInfo, TInt aLanguageIndex) const;

   /**
    * Calculates the total size of all files in the software installation,
    * below a given SIS controller.
    *
    * @param  aInstallBlock        The install block whose size will be calculated.
    * @param  aExpressionEvaluator Used to evaluate the CExpression expressions from
    *                              the SISX files in the software installation.
    * @param  aEmbedded				ETrue to include embedded controllers in the total size
    * @return TInt64               The installation's size below a controller.
    */
   TInt64 TotalSizeL(const CInstallBlock&  aInstallBlock,
   					 CExpressionEvaluator* aExpressionEvaluator,
   					 TBool aEmbedded) const;

   /**
    *
    */
   RPointerArray<TDesC> LocalizedOptionsL(TInt aLanguageIndex) const;
   
   /**
    *
    */
   const RArray<TLanguage>& AvailableLanguages() const;

 protected:   
   CContentProvider(const CController& aController);
   void ConstructL();
   
 private:
   const CController&      iController;
   RArray<TLanguage> iAvailableLanguages;
   
   /**
    * Calculates the size of all the SIS file descriptions in a SISInstallBlock.
    *
    * @param  aInstallBlock  The install block whose size will be calculated.
    * @return TInt64         The total size of all the SIS file descriptions.
    */ 
   TInt64 SizeOfFileDescriptionsInInstallBlock(const CInstallBlock& aInstallBlock) const;

 };

inline const CController& CContentProvider::Controller() const { return iController; }
inline const RArray<TLanguage>& CContentProvider::AvailableLanguages() const { return iAvailableLanguages; }

} // end namespace Swi

#endif // __SISCONTENTPROVIDER_H__
