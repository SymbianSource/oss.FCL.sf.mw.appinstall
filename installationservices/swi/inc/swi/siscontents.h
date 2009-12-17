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

#ifndef __SISCONTENTS_H__
#define __SISCONTENTS_H__

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include "sisfieldtypes.h"

#include "sisfield.h"

namespace Swi
{
	class MSisDataProvider;

	namespace Sis 
		{

		class CData;
		class CController;
		class CCompressed;
		class CCrc;

		/**
		 * This class represents a Contents. Contents is a basic structure found in  files.
		 * It is described in SGL.GT0188.251.
		 *
		 * @internalTechnology
		 * @released
		 */
		class CContents : public CField
			{
		public:
		
			/**
			 * This creates a new empty CContents object.
			 * 
			 * @param aDataProvider An instance of MSisDataProvider used to 
			 *                      access the raw data to be parsed.
			 * 
			 * @return An instance of Swi::Sis::CContents
			 */
			static CContents* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

			virtual ~CContents();

		protected:

			/**
 			 * The constructor.
 			 *
			 * @param aDataProvider An instance of MSisDataProvider used to 
			 *                      access the raw data to be parsed.			
			 */
			CContents(MSisDataProvider& aDataProvider);

			void ConstructL(TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);
			
		public:

			/**
			 * Reads the SISController (uncompressing if necessary).
			 *
			 * @return A buffer containing the uncompressed controller.
			 */
			IMPORT_C HBufC8* ReadControllerL() const;

			/**
			 * This function reads in the Data part of the  file. Please note it does not
			 * actual read in the contents of any of the files to install just their positions, size
			 * and name.
			 *
			 * @param aFile 	 The open  file, with file pointer at start of Data
			 * @param aFileIndex The index of the file to extract.
			 * @param aDataUnit  The index of the data unit we are referring to.
			 */
			IMPORT_C void ReadDataL(RFile& aFile, TInt aFileIndex, TInt aDataUnit);

			/**
			 * This function reads in the Data part of the  file. Please note it does not
			 * actual read in the contents of any of the files to install just their positions, size
			 * and name.
			 *
			 * @param aFile 	 The open  file, with file pointer at start of Data
			 * @param aFileIndex The index of the file to extract.
			 * @param aDataUnit  The index of the data unit we are referring to.
			 * @param aLength		The length of data to extract from the file
			 */
			IMPORT_C void ReadDataL(RFile& aFile, TInt aFileIndex, TInt aDataUnit, TInt64 aLength);


			/** Write out the Sisx header the controller fields to a file
			@param aFile The file to write the stub fields into
			*/
			static void WriteStubFieldsL(RFile& aFile, MSisDataProvider& aDataProvider);
			
			/** Determines whether the SIS file described by this object is a stub
			@return ETrue if the SIS file is a stub
			*/
			TBool IsStub();
			
			/** Check the data and controller CRC values
			@return ETrue if the CRC is correct, EFalse otherwise
			*/
			IMPORT_C void CheckCrcL();
		private:
			/** Extract the relevant fields from the SisContent field
			@param aWriteStream The stream to write the fields to
			@param aDataProvider The source of the original SIS file
			*/
			static void ExtractSisStubFieldsL(RWriteStream& aWriteStream, MSisDataProvider& aDataProvider, TInt64& aMaxLength);
			
			/** Read the SisContents header
			This just skips over the type and length fields
			*/
			void ReadHeaderL();

		private:		
			/**
			The SignedController structure of the  file.
			*/
			CCompressed* iCompressed;

			/**
			The Data structure of the  file.
			*/
			CData* iData;

			CCrc* iDataCrc;
			CCrc* iControllerCrc;

			MSisDataProvider& iDataProvider;
			};
				
		} // namespace Sis
	} // namespace Swi

#include "siscontents.inl"

#endif