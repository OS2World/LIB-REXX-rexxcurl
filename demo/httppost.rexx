#!/usr/bin/rexxcurl
/*
 * Rexx/CURL sample program to execute a HTTP POST to a web server.
 * This example, subscribes the user to the rexxcurl-users mailing
 * list at SourceForge.
 * Pass the email address and password on the command line.
 */

Call RxFuncAdd 'CurlLoadFuncs', 'rexxcurl', 'CurlLoadFuncs'
Call CurlLoadFuncs

Parse Arg email password
If password = '' Then
   Do
      Say 'No password supplied'
      Exit 1
   End
If Countstr( '@', email ) \= 1 Then
   Do
      Say 'The supplied email address is invalid'
      Exit 1
   End

site = 'http://lists.sourceforge.net/lists/subscribe/rexxcurl-users' 

curl = CurlInit()
If curl \= '' Then
   Do
      Say 'We are running' CurlVariable('VERSION')

      Call CurlSetopt curl, 'HTTPPOST', 1
      field.1 = 'email='email
      field.2 = 'pw='password
      field.3 = 'pw-conf='password
      field.4 = 'digest=0'
      field.5 = 'email-button=Subscribe'
      field.0 = 5

      Call CurlSetopt curl, 'HTTPPOSTFIELDS', 'field.'
      Call CurlSetopt curl, 'OUTSTEM', 'response.'

      Call CurlSetopt curl, 'URL', site
      If curlerror.intcode \= 0 Then Call Abort 'Error setting URL option'

      Call CurlPerform curl
      If curlerror.intcode \= 0 Then Call Abort 'Error POSTing to' site
      /*
       * Check that the submission worked
       */
      ok = 0
      Do i = 1 To response.0
         If Countstr( 'Instructions are being sent', response.i ) \= 0 Then 
            Do
               ok = 1
               Leave
            End
      End
      If ok Then Say 'Subscription worked OK for' email
      Else 
         Do
            Say 'Subscription failed for' email '- results follow...'
            Do i = 1 To response.0
               Say response.i
            End
         End
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
