#!/usr/bin/rexxcurl
/*
 * Rexx/CURL sample program to connect to a web server and display
 * the source of the page specified by the supplied URL.
 */

Call RxFuncAdd 'CurlLoadFuncs', 'rexxcurl', 'CurlLoadFuncs'
Call CurlLoadFuncs

Parse Arg url
If url = '' Then
   Do
      Say 'No URL supplied'
      Exit 1
   End

curl = CurlInit()
If curl \= '' Then
   Do
      msg = 'We are running' CurlVariable('VERSION')
      Say Copies( '*', Length( msg ) )
      Say msg
      Say Copies( '*', Length( msg ) )
      Say

      Call CurlSetopt curl, 'URL', url
      If curlerror.intcode \= 0 Then Call Abort 'Error setting URL option'

      Call CurlPerform curl
      If curlerror.intcode \= 0 Then Call Abort 'Error getting source of' site
      /*
       * Cleanup the connection
       */
      Call CurlCLeanup curl
   End
Call CurlDropFuncs 'UNLOAD'
Return 0

Abort: Procedure Expose curlerror.
Parse Arg msg
Say msg
If curlerror.curlcode \= 0 Then Say 'cURL error:' curlerror.curlcode '-' curlerror.curlerrm
Else Say 'RexxCURL error:' curlerror.intcode '-' curlerror.interrm
Call CurlCLeanup curl
Call CurlDropFuncs 'UNLOAD'
Exit 1
