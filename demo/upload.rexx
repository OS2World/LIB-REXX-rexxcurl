#!/usr/bin/rexxcurl
/*
 * Rexx/CURL sample program to upload files to the /incoming directory
 * on an anonymous ftp site and change their permissions so they can be 
 * downloaded. 
 * Once all the files have been uploaded, list the directory.
 * Pass the files to upload on the command line.
 */
Call RxFuncAdd 'CurlLoadFuncs', 'rexxcurl', 'CurlLoadFuncs'
Call CurlLoadFuncs

Parse Arg filenames
site = 'ftp://localhost/incoming/'
user = 'ftp:M.Hessling@qut.edu.au'

curl = CurlInit()
If curl \= '' Then
   Do
      Say 'We are running' CurlVariable('VERSION')

/*      Call CurlSetopt curl, 'VERBOSE', 1 */

      Call CurlSetopt curl, 'UPLOAD', 1
      If curlerror.intcode \= 0 Then Call Abort 'Error setting UPLOAD option'
      Call CurlSetopt curl, 'USERPWD', user
      If curlerror.intcode \= 0 Then Call Abort 'Error setting USERPWD option'
      Do i = 1 To Words( filenames )
         upload_file = Word( filenames, i )
         Call CurlSetopt curl, 'INFILE', upload_file
         If curlerror.intcode \= 0 Then Call Abort 'Error setting INFILE option'
         Call CurlSetopt curl, 'URL', site || upload_file
         If curlerror.intcode \= 0 Then Call Abort 'Error setting URL option'
         post.0 = 1
         post.1 = 'SITE chmod 644' upload_file
         Call CurlSetopt curl, 'FTPCMDSAFTER', 'post.'
         If curlerror.intcode \= 0 Then Call Abort 'Error setting FTPCMDSAFTER option'
         /*
          * Upload the file...
          */
         Call CurlPerform curl
         If curlerror.intcode \= 0 Then Call Abort 'Error performing upload of' upload_file
      End
      /*
       * Now the uploads are finished, list the new directory to see our new
       * file(s)
       */
      Call CurlSetopt curl, 'INFILE', ''
      If curlerror.intcode \= 0 Then Call Abort 'Error setting INFILE option off'
      Call CurlSetopt curl, 'URL', site
      If curlerror.intcode \= 0 Then Call Abort 'Error setting URL option'
      Call CurlSetopt curl, 'UPLOAD', 0
      If curlerror.intcode \= 0 Then Call Abort 'Error setting UPLOAD option off'
      Call CurlPerform curl
      If curlerror.intcode \= 0 Then Call Abort 'Error performing list of files in' site
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
