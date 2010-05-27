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
* Definition of the Swi::Sis::Parser
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISPARSER_H__
#define __SISPARSER_H__

#include <e32base.h>
#include <e32std.h>

namespace Swi 
{
  class MSisDataProvider;

  namespace Sis
  {
    class CContents;

	/**
	 * This class is the entry point to the SISX parser infrastructure.
	 * It provides a static method that will return an initialised
	 * Swi::Sis::CContent class which represents the SisContent
	 * structure as described in SGL.GT0188.251.
	 *
	 * @internalTechnology
	 * @released
	 */
    class Parser 
    {
    public:

      /**
       * Given a MDataProvider to some SISX content, this method checks
       * the Symbian header and returns a CContent ready to extract a
       * CController and the Data.
       *
       * @param aDataProvider An instance of MSisDataProvider which will give
       *                      us access to the SISX data payload.
       *
       * @return A CContent object which will use the given data provider.
       */
      IMPORT_C static CContents* ContentsL(MSisDataProvider& aDataProvider);
      
      /**
      Create a SIS stub (.SIS file with no Data) from an existing SIS file
      
      @param aFile The target file handle where the SIS stub will be written
      @param aDataProvider Used to read the existing SIS file.
      */
      IMPORT_C static void CreateSisStubL(RFile& aFile, MSisDataProvider& aDataProvider);
     
    protected:

      /**
       * Helper function used to check the SISX header's CRC.
       * 
       * @param aCrc  The CRC found in the SISX file header
       * @param aUid1 The first uid (= 0x101FD0EF SISX Unique Uid)
       * @param aUid2 The second uid ( = 0 Reserved for future use)
	   * @param aUid3 The third uid (package specific)	       
	   *
	   * @return KErrNone if the CRC is correct, KErrCorrupt otherwise. 
       */
      static TInt CheckUidCrc(TUint32 aCrc, const TDesC8& aUid1, const TDesC8& aUid2, const TDesC8& aUid3);
      
     private:
      /**
      Read the Symbian header and return the header Uid fields
      @param aDataProvider An instance of MSisDataProvider which will give us access to the SISX header
      @param aUid1 used to return the first Uid
      @param aUid2 used to return the second Uid
      @param aUid3 used to return the third Uid
      @param aUidCrc used to return the Uid CRC
      */
      static void ReadSymbianHeaderL(MSisDataProvider& aDataProvider, TInt32& aUid1, TInt32& aUid2, TInt32& aUid3, TUint32& aUidCrc);
      
      /**
      Write the Symbian header to the file handle supplied
      @param aFile A file to write the SisX header to
      @param aUid1 the first Uid
      @param aUid2 the second Uid
      @param aUid3 the third Uid
      @param aUidCrc the Uid CRC
      */
      static void WriteSymbianHeaderL(RFile& aFile, TInt32& aUid1, TInt32& aUid2, TInt32& aUid3, TUint32& aUidCrc);
      
    };
  }
}

#endif // __SISPARSER_H__
