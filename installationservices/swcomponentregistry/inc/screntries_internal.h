/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Declares entry classes for Software Components Registry interface.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SCRENTRIES_INTERNAL_H
#define SCRENTRIES_INTERNAL_H

#include <e32base.h>
#include <s32strm.h>
#include <f32file.h> 
#include <usif/usifcommon.h>
#include <usif/scr/screntries.h>

//class Usif::CLocalizableCommonData;
//class Usif::CScrLogEntry;

namespace Usif
	{
		class CLocalizableCommonData;
		NONSHARABLE_CLASS(CLocalizedSoftwareTypeName) : public CLocalizableCommonData
		/**
			This class contains localized software type names.
			Software type names are localizable names and transferred to the SCR 
			with the objects of this class.
			
			@internalTechnology
		 */
			{
		public:	
			/**
				Creates a localized software type name object.
				@param aName The name of the software type.
				@param aLocale The language code of the object's locale.
				@return A pointer to the newly allocated localized software type name object, if creation is successful.
			 */
			IMPORT_C static CLocalizedSoftwareTypeName* NewL(const TDesC& aName, TLanguage aLocale);
			
			/**
				Creates a localized software type name object.
				@param aName The name of the software type.
				@param aLocale The language code of the object's locale.
				@return A pointer to the newly allocated localized software type name object, if creation is successful.
						The pointer is also put onto the cleanup stack.
			 */
			IMPORT_C static CLocalizedSoftwareTypeName* NewLC(const TDesC& aName, TLanguage aLocale);
			
			/**
				Creates a localized software type name object from a stream.
				@param aStream The stream to read the localized software type name object from.
				@return A pointer to the newly allocated localized software type name object, if creation is successful.
				@internalComponent
			 */
			IMPORT_C static CLocalizedSoftwareTypeName* NewL(RReadStream& aStream);
			
			~CLocalizedSoftwareTypeName();
			
			/**
				Externalises a localized software type name object to a write stream.
				@param aStream The stream to which the object is externalised.
				@internalComponent
			 */
			IMPORT_C void ExternalizeL(RWriteStream& aStream) const;	
			
			/** @return The localized software type name. */
			IMPORT_C const TDesC& NameL() const;
			
		private:
			CLocalizedSoftwareTypeName();
			CLocalizedSoftwareTypeName(TLanguage aLocale);
			void ConstructL(const TDesC& aName);
			void InternalizeL(RReadStream& aStream);
			
		private:
			HBufC* iName;	   ///< The localized software type name.
			};

	} // End of namespace Usif

#endif // SCRENTRIES_H
