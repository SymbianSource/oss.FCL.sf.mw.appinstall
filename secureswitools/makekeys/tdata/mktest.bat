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
@rem @call cleanup.bat

@echo Test1 - Use varying key sizes (512, 1024, 2048, 3072, 4096)
@echo -----------------------------------------------------------

makekeys -cert           -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" testdef.key  testdef.cer
makekeys -cert -len 512  -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test512.key  test512.cer
makekeys -cert -len 1024 -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test1024.key test1024.cer
makekeys -cert -len 2048 -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test2048.key test2048.cer
@rem makekeys -cert -len 3072 -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test3072.key test3072.cer
@rem makekeys -cert -len 4096 -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test4096.key test4096.cer

@echo .
@echo Test2 - Password protected key file (password not given not command line)
@echo -------------------------------------------------------------------------
@echo When prompted enter password of "test"

makekeys -cert -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test2.key test2.cer
makekeys -req  -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test2.key test2.cer test2.req

@echo .
@echo Test3 - Password protected key file (password on command line)
@echo --------------------------------------------------------------

makekeys -cert -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test3.key test3.cer
makekeys -req  -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test3.key test3.cer test3.req

@echo .
@echo Test4 - Password protected key file (not on cmd line and invalid)
@echo -----------------------------------------------------------------
@echo When prompted enter password of "test"

makekeys -cert -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test3.key test4.cer
makekeys -cert -password wrongpwd -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test3.key test4a.cer

@echo .
@echo Test5 - 
@echo --------

makekeys -cert -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test5.key test5.cer

@echo .
@echo Test6 - Create certificate requests
@echo -----------------------------------
@echo Following certificate requests should by tried with Thawte test server...

makekeys -req -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" testdef.key  testdef.cer  testdef.req
makekeys -req -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test512.key  test512.cer  test512.req
makekeys -req -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test1024.key test1024.cer test1024.req
makekeys -req -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test2048.key test2048.cer test2048.req
makekeys -req -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test3072.key test3072.cer test3072.req
makekeys -req -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test4096.key test4096.cer test4096.req

@echo .
@echo Test7 - Test Distinguished Names
@echo --------------------------------

makekeys -cert -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test7.key test7.cer
makekeys -req  -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test7.key test7.cer test7.req

@echo .
@echo Test8 -
@echo -------

makekeys -cert -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test8.key test8.cer

@echo .
@echo Test9 - 
@echo -------

makekeys -cert -password test -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" test9.key test9.cer

@echo .
@echo Test10 - View certificate and certificate chains
@echo ------------------------------------------------

makekeys -view testdef.cer
makekeys -view test512.cer

@echo .
@echo Test11 - Y2K Test1 - 31-12-99
@echo ------------------------------

date 31-12-99
makekeys -cert -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key311299.key key311299.cer
makekeys -req  -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key311299.key key311299.cer key311299.req

@echo .
@echo Test12 - Y2K Test2 - 28-02-00
@echo ------------------------------

date 28-02-00
makekeys -cert -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key280200.key key280200.cer
makekeys -req  -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key280200.key key280200.cer key280200.req

@echo .
@echo Test13 - Y2K Test3 - 29-02-00
@echo ------------------------------

date 29-02-00
makekeys -cert -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key290200.key key290200.cer
makekeys -req  -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key290200.key key290200.cer key290200.req

@echo .
@echo Test14 - Y2K Test4 - 01-03-00
@echo ------------------------------

date 01-03-00
makekeys -cert -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key010300.key key010300.cer
makekeys -req  -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key010300.key key010300.cer key010300.req

@echo .
@echo Test15 - Y2K Test5 - 01-01-00
@echo ------------------------------

date 01-01-00
makekeys -cert -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key010100.key key010100.cer
makekeys -req  -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key010100.key key010100.cer key010100.req

@echo .
@echo Test16 - Y2K Test7 - 01-01-01
@echo ------------------------------

date 01-01-01
makekeys -cert -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key010101.key key010101.cer
makekeys -req  -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key010101.key key010101.cer key010101.req

@echo .
@echo Test17 - Y2K Test8 - 27-02-00
@echo ------------------------------

date 27-02-00
makekeys -cert -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key270200.key key270200.cer
makekeys -req  -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key270200.key key270200.cer key270200.req

@echo .
@echo Test18 - Y2K Test9 - 28-02-01
@echo ------------------------------

date 28-02-01
makekeys -cert -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key280201.key key280201.cer
makekeys -req  -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key280201.key key280201.cer key280201.req

@echo .
@echo Test19 - Y2K Test10 - 01-03-01
@echo ------------------------------

date 01-03-01
makekeys -cert -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key010301.key key010301.cer
makekeys -req  -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key010301.key key010301.cer key010301.req

@echo .
@echo Test20 - Y2K Test11 - 28-02-04
@echo ------------------------------

date 28-02-04
makekeys -cert -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key280204.key key280204.cer
makekeys -req  -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key280204.key key280204.cer key280204.req

@echo .
@echo Test21 - Y2K Test12 - 29-02-04
@echo ------------------------------

date 29-02-04
makekeys -cert -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key290204.key key290204.cer
makekeys -req  -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key290204.key key290204.cer key290204.req

@echo .
@echo Test22 - Y2K Test13 - 01-03-04
@echo ------------------------------

date 01-03-04
makekeys -cert -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key010304.key key010304.cer
makekeys -req  -password test -dname "CN=Symbian OR=Symbian Ltd CO=GB" key010304.key key010304.cer key010304.req

@echo .
@echo Test23 - Testing PDEF098608.A new option to specify the certificate's expiry date .When the expiry date option is not specified,it defaults to 1 year.
@echo --------

makekeys -cert -password pass -expdays 730 -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" pdef098608_1.key pdef098608_1.cer
makekeys -cert -password pass -dname "CN=Symbian OU=Development OR=Symbian Ltd CO=GB EM=noone@symbian.com" pdef098608_2.key pdef098608_2.cer

@echo .
@echo Test24 - Testing INC107520. makekeys bugs : password should be aleast 4 characters.
@echo --------
makekeys -cert -expdays 3650 -len 512 -dname "CN=1" -password 123  test.key test.cer


@echo .
@echo Test25 - Testing INC107520. makekeys bugs : Empty password with "n" acceptance should create the key and cert.
@echo --------
makekeys -cert -expdays 3650 -len 512 -dname "CN=1"   test.key test.cer


@echo .
@echo Use following URL to test certificate requests:
@echo .
@echo https://www.thawte.com/cgi/server/test.exe
@echo .
@echo try these requests:
@echo   testdef.req
@echo   test512.req
@echo   test1024.req
@echo   test2048.req
@echo   test3072.req
@echo   test4096.req
@echo   test7.req
@echo plus the various date test requests
