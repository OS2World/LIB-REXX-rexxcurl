#!/usr/bin/rexxcurl
/*
 * Rexx/CURL sample program to mirror an FTP location in the currrent
 * directory.
 * Pass the FTP location, username and password on the command line.
 */
Parse Arg location username password
If Right( location, 1 ) \= '/' Then location = location'/'
If Left( location, 6 ) \= 'ftp://' Then
   Do
      Say 'Invalid FTP location specified'
      Exit 1
   End

Call RxFuncAdd 'CurlLoadFuncs', 'rexxcurl', 'CurlLoadFuncs'
Call CurlLoadFuncs

Call RxFuncAdd 'SysLoadFuncs', 'rexxutil', 'SysLoadFuncs'
Call SysLoadFuncs

dirs.0 = 1
dirs.1 = location
user = username':'password
cwd = Directory()

curl = CurlInit()
If curl \= '' Then
   Do
      Parse Version ver
      Say 'We are running' CurlVariable('VERSION') 'on' ver

      Call CurlSetopt curl, 'VERBOSE', 1 

      Call CurlSetopt curl, 'USERPWD', user
      If curlerror.intcode \= 0 Then Call Abort 'Error setting USERPWD option'

      Call CurlSetopt curl, 'OUTSTEM', 'files.'
      If curlerror.intcode \= 0 Then Call Abort 'Error setting OUTSTEM option'

      Call CurlSetopt curl, 'URL', location
      If curlerror.intcode \= 0 Then Call Abort 'Error setting URL option'

      Call CurlPerform curl
      If curlerror.intcode \= 0 Then Call Abort 'Error getting directory for' location

      Do Forever
         If dirs.0 = 0 Then Leave
         Call GetFiles dirs.1
      End
      /*
       * Cleanup the connection
       */
      Call CurlCLeanup curl
   End
Call CurlDropFuncs 'UNLOAD'
Return 0

/*
 * Gets the list of files in the specified directory
 * Changes directory locally to incoming directory
 * If a directory:
 * - Adds the directory to dirs. array with correct full path
 * - Create directory locally
 * If a file:
 * - Download file
 * Once all files processed for incoming directory, delete
 * it from dirs. array
 */
GetFiles:
Parse Arg dir
newlocal = cwd || '/' Changestr( location, dir, '' )
Call Directory( newlocal )
Do i = 1 To files.0
say '<<'files.i'>>'
   Parse Var files.i perms . . . . . . . fn
   Select
      When Left( files.i, 1) = 'd' & fn = '.' Then Nop
      When Left( files.i, 1) = 'd' & fn = '..' Then Nop
      When Left( files.i, 1) = 'd' Then
         Do
            idx = dirs.0
            Call SysStemInsert 'dirs.', idx, dir||fn'/'
/*
            Address System 'mkdir' fn
*/
            Say 'making dir' fn
         End
      Otherwise
         Do
/*
            Call CurlSetopt curl, 'URL', dir||fn
            If curlerror.intcode \= 0 Then Call Abort 'Error setting URL option'
            Call CurlPerform curl
            If curlerror.intcode \= 0 Then Call Abort 'Error downloading' dir||fn
*/
            Say 'Getting' dir||fn
         End
   End
End
Call SysStemDelete 'dirs.', 1
dirs.0 = 0
Return

Abort: Procedure Expose curlerror.
Parse Arg msg
Say msg
If curlerror.curlcode \= 0 Then Say 'cURL error:' curlerror.curlcode '-' curlerror.curlerrm
Else Say 'RexxCURL error:' curlerror.intcode '-' curlerror.interrm
Call CurlCLeanup curl
Call CurlDropFuncs 'UNLOAD'
Exit 1
