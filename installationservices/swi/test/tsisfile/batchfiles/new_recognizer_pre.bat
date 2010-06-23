@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
REM Setup the epoc.ini to pick up "KSisxRecognizerConst" for the new recognizer
copy \epoc32\data\epoc.ini \epoc32\data\epoc_temp.ini
copy \epoc32\data\epoc_recognizer.ini \epoc32\data\epoc.ini

REM Setup the data files for the tests
copy \epoc32\release\winscw\udeb\z\tswi\tsis\data\053177a_0.sis \epoc32\release\winscw\udeb\z\tswi\tsis\data\053177a_0.sisx
copy \epoc32\release\winscw\udeb\z\tswi\tsis\data\053177a_0.sis \epoc32\release\winscw\udeb\z\tswi\tsis\data\053177a_0.sisa
copy \epoc32\release\winscw\udeb\z\tswi\tsis\data\053177a_0.sis \epoc32\release\winscw\udeb\z\tswi\tsis\data\053177a_0
copy \epoc32\release\winscw\urel\z\tswi\tsis\data\053177a_0.sis \epoc32\release\winscw\urel\z\tswi\tsis\data\053177a_0.sisx
copy \epoc32\release\winscw\urel\z\tswi\tsis\data\053177a_0.sis \epoc32\release\winscw\urel\z\tswi\tsis\data\053177a_0.sisa
copy \epoc32\release\winscw\urel\z\tswi\tsis\data\053177a_0.sis \epoc32\release\winscw\urel\z\tswi\tsis\data\053177a_0