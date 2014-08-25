#!/usr/bin/rexxcurl
/*
 * Rexx/CURL sample program to behave similarly to scp; the SSH copy program
 */
Parse Arg arg1 arg2
If arg1 = '' Then Call usage
outfile = ''
infile = ''
Select
   When Countstr( '@', arg1 ) = 1 Then
      Do
         If Right( arg1, 1 ) = '/' Then
            Do
               Say 'Invalid parameter: remote parameter must refer to a file, not a directory'
               Exit 1
            End
         upload = 0
         Parse Value Parser( arg1, arg2 ) With url userpwd outfile
      End
   When Countstr( '@', arg2 ) = 1 Then
      Do
         upload = 1
         Parse Value Parser( arg2, arg1 ) With url userpwd infile
         If Stream( infile, 'C', 'QUERY EXISTS' ) = '' Then
            Do
               Say 'Invalid parameter: local file' arg1 'does not exist'
               Exit 1
            End
      End
   Otherwise
      Do
         Say 'You must specify one of the parameters as a remote location'
         Exit 1
      End
End
/*
say 'url:' url
say 'userpwd:' userpwd
say 'outfile:' outfile
say 'infile:' infile
say 'upload:' upload
*/
size.0 = 'SIZE_DOWNLOAD'
size.1 = 'SIZE_UPLOAD'

Call RxFuncAdd 'CurlLoadFuncs', 'rexxcurl', 'CurlLoadFuncs'
Call CurlLoadFuncs
curl = CurlInit()
If curl \= '' Then
   Do
      Call CurlSetopt curl, 'UPLOAD', upload
      If curlerror.intcode \= 0 Then Call Abort 'Error setting UPLOAD option'
      Call CurlSetopt curl, 'USERPWD', userpwd
      If curlerror.intcode \= 0 Then Call Abort 'Error setting USERPWD option'
      Call CurlSetopt curl, 'INFILE', infile
      If curlerror.intcode \= 0 Then Call Abort 'Error setting INFILE option'
      Call CurlSetopt curl, 'OUTFILE', outfile
      If curlerror.intcode \= 0 Then Call Abort 'Error setting OUTFILE option'
      Call CurlSetopt curl, 'URL', url
      If curlerror.intcode \= 0 Then Call Abort 'Error setting URL option'
      /*
       * Do it
       */
      Call CurlPerform curl
      If curlerror.intcode \= 0 Then Call Abort 'Error performing transfer of' arg1 'to' arg2
      /*
       * Display some info...
       */
      Say Format( CurlGetinfo( curl, size.upload ), , 0 ) 'bytes transfered in' CurlGetinfo( curl, 'TOTAL_TIME' ) 'seconds'
      /*
       * Cleanup the connection
       */
      Call CurlCLeanup curl
   End
Call CurlDropFuncs 'UNLOAD'
Return 0

Parser: Procedure
Parse Arg remote, local
Parse Var remote userpwd '@' server '/' remote_file
If Right( remote_file, 1 ) = '/' | remote_file = '' Then
   Do
      base_file_pos = Lastpos( '/', local )
      If base_file_pos = 0 Then base_file = local
      Else
         Do
            base_file = Substr( local, base_file_pos+1 )
         End
   remote_file = remote_file || base_file
   End
If local = '' Then
   Do
      base_file_pos = Lastpos( '/', remote_file )
      If base_file_pos = 0 Then base_file = remote_file
      Else
         Do
            base_file = Substr( remote_file, base_file_pos+1 )
         End
   local = base_file
   End
url = 'ftp://'server'/'remote_file
Return url userpwd local

Usage:
Say 'To copy from a remote site to a local...'
Say 'scp.rexx user[:password]@server/dir/remote_file [local_file]'
Say
Say 'To copy to a remote site from a local...'
Say 'local_file user[:password]@server/dir/[remote_file]'
Exit 0

Abort: Procedure Expose curlerror.
Parse Arg msg
Say msg
If curlerror.curlcode \= 0 Then Say 'cURL error:' curlerror.curlcode '-' curlerror.curlerrm
Else Say 'RexxCURL error:' curlerror.intcode '-' curlerror.interrm
Call CurlCleanup curl
Call CurlDropFuncs 'UNLOAD'
Exit 1
