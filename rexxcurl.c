/*
 *  Rexx/CURL
 *  Copyright (C) 2001  Mark Hessling <M.Hessling@qut.edu.au>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * Error handling:
 *                              error_rexxcurl       error_curl
 *                          
 * ------------------------------------------------------------
 * wrong # params           ******* syntax error **********
 * internal error                   set                 N/A
 * cURL runtime errors              -1                  set
 */

#define RXCURL_VERSION "1.0.0"
#define RXCURL_DATE "28 June 2001"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "rexxcurl.h"

char *RxPackageName = "rexxcurl";

#define INTERR_CURL_ERROR            1
#define INTERR_CURL_ERROR_STRING     "Error from cURL"
#define INTERR_INVALID_NUMBER        2
#define INTERR_INVALID_NUMBER_STRING "Invalid Number"
#define INTERR_INVALID_OPTION        3
#define INTERR_INVALID_OPTION_STRING "Invalid Option"
#define INTERR_NO_MEMORY             4
#define INTERR_NO_MEMORY_STRING      "Out of memory"
#define INTERR_INVALID_HANDLE        5
#define INTERR_INVALID_HANDLE_STRING "Invalid cURL handle"
#define INTERR_INVALID_FILE          6
#define INTERR_INVALID_FILE_STRING   "Invalid filename"
#define INTERR_INVALID_BOOL          7
#define INTERR_INVALID_BOOL_STRING   "Invalid boolean"
#define INTERR_INVALID_STEM          8
#define INTERR_INVALID_STEM_STRING   "Expecting a stem as parameter"
#define INTERR_INVALID_VARIABLE      9
#define INTERR_INVALID_VARIABLE_STRING "Invalid variable:"
#define INTERR_READONLY_VARIABLE     10
#define INTERR_READONLY_VARIABLE_STRING "Cannot set readonly variable"

#define RXCURLOPT_STRING       1  /* string */
#define RXCURLOPT_LIST         2  /* stem for slist */
#define RXCURLOPT_LONG         3  /* number */
#define RXCURLOPT_OUTFILE      4  /* output file */
#define RXCURLOPT_INFILE       5  /* intput file */
#define RXCURLOPT_BOOL         6  /* bool */
#define RXCURLOPT_POLICY       7  /* policy */
#define RXCURLOPT_POST_DATA    8  /* stem for httppostdata */
#define RXCURLOPT_POST_FIELDS  9  /* stem for httppostfields */
#define RXCURLOPT_OUTSTEM     10  /* stem for outstem */
#define RXCURLOPT_HEADERSTEM  11  /* stem for headerstem */

#define RXCURLINFO_STRING      1  /* string */
#define RXCURLINFO_LONG        2  /* number */
#define RXCURLINFO_DOUBLE      3  /* double */

static char *curl_errors[] =
{
   "OK",                      /* 0 */
   "UNSUPPORTED_PROTOCOL",    /* 1 */
   "FAILED_INIT",             /* 2 */
   "URL_MALFORMAT",           /* 3 */
   "URL_MALFORMAT_USER",      /* 4 */
   "COULDNT_RESOLVE_PROXY",   /* 5 */
   "COULDNT_RESOLVE_HOST",    /* 6 */
   "COULDNT_CONNECT",         /* 7 */
   "FTP_WEIRD_SERVER_REPLY",  /* 8 */
   "FTP_ACCESS_DENIED",       /* 9 */
   "FTP_USER_PASSWORD_INCORRECT", /* 10 */
   "FTP_WEIRD_PASS_REPLY",    /* 11 */
   "FTP_WEIRD_USER_REPLY",    /* 12 */
   "FTP_WEIRD_PASV_REPLY",    /* 13 */
   "FTP_WEIRD_227_FORMAT",    /* 14 */
   "FTP_CANT_GET_HOST",       /* 15 */
   "FTP_CANT_RECONNECT",      /* 16 */
   "FTP_COULDNT_SET_BINARY",  /* 17 */
   "PARTIAL_FILE",            /* 18 */
   "FTP_COULDNT_RETR_FILE",   /* 19 */
   "FTP_WRITE_ERROR",         /* 20 */
   "FTP_QUOTE_ERROR",         /* 21 */
   "HTTP_NOT_FOUND",          /* 22 */
   "WRITE_ERROR",             /* 23 */
   "MALFORMAT_USER",          /* 24 - user name is illegally specified */
   "FTP_COULDNT_STOR_FILE",   /* 25 - failed FTP upload */
   "READ_ERROR",              /* 26 - could open/read from file */
   "OUT_OF_MEMORY",           /* 27 */
   "OPERATION_TIMEOUTED",     /* 28 - the timeout time was reached */
   "FTP_COULDNT_SET_ASCII",   /* 29 - TYPE A failed */
   "FTP_PORT_FAILED",         /* 30 - FTP PORT operation failed */
   "FTP_COULDNT_USE_REST",    /* 31 - the REST command failed */
   "FTP_COULDNT_GET_SIZE",    /* 32 - the SIZE command failed */
   "HTTP_RANGE_ERROR",        /* 33 - RANGE "command" didn"'t work */
   "HTTP_POST_ERROR",         /* 34 */
   "SSL_CONNECT_ERROR",       /* 35 - wrong when connecting with SSL */
   "FTP_BAD_DOWNLOAD_RESUME", /* 36 - couldn"'t resume download */
   "FILE_COULDNT_READ_FILE",  /* 37 */
   "LDAP_CANNOT_BIND",        /* 38 */
   "LDAP_SEARCH_FAILED",      /* 39 */
   "LIBRARY_NOT_FOUND",       /* 40 */
   "FUNCTION_NOT_FOUND",      /* 41 */
   "ABORTED_BY_CALLBACK",     /* 42 */
   "BAD_FUNCTION_ARGUMENT",   /* 43 */
   "BAD_CALLING_ORDER",       /* 44 */
   "HTTP_PORT_FAILED",        /* 45 - HTTP Interface operation failed */
   "BAD_PASSWORD_ENTERED",    /* 46 - my_getpass() returns fail */
   "TOO_MANY_REDIRECTS ",     /* 47 - catch endless re-direct loops */
   "UNKNOWN_TELNET_OPTION",   /* 48 - User specified an unknown option */
   "TELNET_OPTION_SYNTAX ",   /* 49 - Malformed telnet option */
   "OBSOLETE",                /* 50 - removed after 7.7.3 */
   "SSL_PEER_CERTIFICATE",    /* 51 - peer"'s certificate wasn't ok */
};

typedef struct 
{
   char *name;
   int  number;
   int optiontype; /* things like STEM, STRING, INT */
} curl_options;

static curl_options RexxCurlOptions[] =
{
   { "CLOSEPOLICY"     ,CURLOPT_CLOSEPOLICY     ,RXCURLOPT_POLICY      },
   { "CONNECTTIMEOUT"  ,CURLOPT_CONNECTTIMEOUT  ,RXCURLOPT_LONG        },
   { "COOKIE"          ,CURLOPT_COOKIE          ,RXCURLOPT_STRING      },
   { "COOKIEFILE"      ,CURLOPT_COOKIEFILE      ,RXCURLOPT_INFILE      },
   { "CUSTOMREQUEST"   ,CURLOPT_CUSTOMREQUEST   ,RXCURLOPT_STRING      },
   { "EGDSOCKET"       ,CURLOPT_EGDSOCKET       ,RXCURLOPT_STRING      },
   { "ERRFILE"         ,CURLOPT_STDERR          ,RXCURLOPT_OUTFILE     },
   { "FAILONERROR"     ,CURLOPT_FAILONERROR     ,RXCURLOPT_BOOL        },
   { "FILETIME"        ,CURLOPT_FILETIME        ,RXCURLOPT_BOOL        },
   { "FOLLOWLOCATION"  ,CURLOPT_FOLLOWLOCATION  ,RXCURLOPT_BOOL        },
   { "FORBIDREUSE"     ,CURLOPT_FORBID_REUSE    ,RXCURLOPT_BOOL        },
   { "FRESHCONNECT"    ,CURLOPT_FRESH_CONNECT   ,RXCURLOPT_BOOL        },
   { "FTPAPPEND"       ,CURLOPT_FTPAPPEND       ,RXCURLOPT_BOOL        },
   { "FTPCMDSAFTER"    ,CURLOPT_POSTQUOTE       ,RXCURLOPT_LIST        },
   { "FTPCMDSBEFORE"   ,CURLOPT_QUOTE           ,RXCURLOPT_LIST        },
   { "FTPCRLF"         ,CURLOPT_CRLF            ,RXCURLOPT_BOOL        },
   { "FTPLISTONLY"     ,CURLOPT_FTPLISTONLY     ,RXCURLOPT_BOOL        },
   { "FTPPORT"         ,CURLOPT_FTPPORT         ,RXCURLOPT_STRING      },
   { "HEADER"          ,CURLOPT_HEADER          ,RXCURLOPT_BOOL        },
   { "HEADERFILE"      ,CURLOPT_WRITEHEADER     ,RXCURLOPT_OUTFILE     },
   { "HEADERSTEM"      ,CURLOPT_WRITEHEADER     ,RXCURLOPT_HEADERSTEM  },
   { "HTTPHEADER"      ,CURLOPT_HTTPHEADER      ,RXCURLOPT_LIST        },
   { "HTTPPOST"        ,CURLOPT_POST            ,RXCURLOPT_BOOL        },
   { "HTTPPOSTDATA"    ,CURLOPT_HTTPPOST        ,RXCURLOPT_POST_DATA   },
   { "HTTPPOSTFIELDS"  ,CURLOPT_POSTFIELDS      ,RXCURLOPT_POST_FIELDS },
   { "HTTPPROXYTUNNEL" ,CURLOPT_HTTPPROXYTUNNEL ,RXCURLOPT_BOOL        },
   { "HTTPPUT"         ,CURLOPT_PUT             ,RXCURLOPT_BOOL        },
   { "INFILE"          ,CURLOPT_INFILE          ,RXCURLOPT_INFILE      },
   { "INTERFACE"       ,CURLOPT_INTERFACE       ,RXCURLOPT_STRING      },
   { "KRB4LEVEL"       ,CURLOPT_KRB4LEVEL       ,RXCURLOPT_STRING      },
   { "LOWSPEEDLIMIT"   ,CURLOPT_LOW_SPEED_LIMIT ,RXCURLOPT_LONG        },
   { "LOWSPEEDTIME"    ,CURLOPT_LOW_SPEED_TIME  ,RXCURLOPT_LONG        },
   { "MAXCONNECTS"     ,CURLOPT_MAXCONNECTS     ,RXCURLOPT_LONG        },
   { "MAXREDIRS"       ,CURLOPT_MAXREDIRS       ,RXCURLOPT_LONG        },
   { "NETRC"           ,CURLOPT_NETRC           ,RXCURLOPT_BOOL        },
   { "NOBODY"          ,CURLOPT_NOBODY          ,RXCURLOPT_BOOL        },
   { "OUTFILE"         ,CURLOPT_FILE            ,RXCURLOPT_OUTFILE     },
   { "OUTSTEM"         ,CURLOPT_FILE            ,RXCURLOPT_OUTSTEM     },
   { "PROXY"           ,CURLOPT_PROXY           ,RXCURLOPT_STRING      },
   { "PROXYUSERPWD"    ,CURLOPT_PROXYUSERPWD    ,RXCURLOPT_STRING      },
   { "RANDOMFILE"      ,CURLOPT_RANDOM_FILE     ,RXCURLOPT_STRING      },
   { "RANGE"           ,CURLOPT_RANGE           ,RXCURLOPT_STRING      },
   { "REFERER"         ,CURLOPT_REFERER         ,RXCURLOPT_STRING      },
   { "RESUMEFROM"      ,CURLOPT_RESUME_FROM     ,RXCURLOPT_LONG        },
   { "SSLCERT"         ,CURLOPT_SSLCERT         ,RXCURLOPT_STRING      },
   { "SSLCERTPASSWD"   ,CURLOPT_SSLCERTPASSWD   ,RXCURLOPT_STRING      },
   { "SSLPEERCERT"     ,CURLOPT_CAINFO          ,RXCURLOPT_STRING      },
   { "SSLVERIFYPEER"   ,CURLOPT_SSL_VERIFYPEER  ,RXCURLOPT_BOOL        },
   { "SSLVERSION"      ,CURLOPT_SSLVERSION      ,RXCURLOPT_LONG        },
   { "TIMECONDITION"   ,CURLOPT_TIMECONDITION   ,RXCURLOPT_LONG        },
   { "TIMEOUT"         ,CURLOPT_TIMEOUT         ,RXCURLOPT_LONG        },
   { "TIMEVALUE"       ,CURLOPT_TIMEVALUE       ,RXCURLOPT_LONG        },
   { "TRANSFERTEXT"    ,CURLOPT_TRANSFERTEXT    ,RXCURLOPT_BOOL        },
   { "UPLOAD"          ,CURLOPT_UPLOAD          ,RXCURLOPT_BOOL        },
   { "URL"             ,CURLOPT_URL             ,RXCURLOPT_STRING      },
   { "USERAGENT"       ,CURLOPT_USERAGENT       ,RXCURLOPT_STRING      },
   { "USERPWD"         ,CURLOPT_USERPWD         ,RXCURLOPT_STRING      },
   { "VERBOSE"         ,CURLOPT_VERBOSE         ,RXCURLOPT_BOOL        },
   { NULL              ,0                       ,0                     }
};

static curl_options RexxCurlGetinfos[] =
{
   { "CONNECT_TIME"           ,CURLINFO_CONNECT_TIME           ,RXCURLINFO_DOUBLE},
   { "CONTENT_LENGTH_DOWNLOAD",CURLINFO_CONTENT_LENGTH_DOWNLOAD,RXCURLINFO_DOUBLE},
   { "CONTENT_LENGTH_UPLOAD"  ,CURLINFO_CONTENT_LENGTH_UPLOAD  ,RXCURLINFO_DOUBLE},
   { "EFFECTIVE_URL"          ,CURLINFO_EFFECTIVE_URL          ,RXCURLINFO_STRING},
   { "FILETIME"               ,CURLINFO_FILETIME               ,RXCURLINFO_LONG  },
   { "HEADER_SIZE"            ,CURLINFO_HEADER_SIZE            ,RXCURLINFO_LONG  },
   { "HTTP_CODE"              ,CURLINFO_HTTP_CODE              ,RXCURLINFO_LONG  },
   { "NAMELOOKUP_TIME"        ,CURLINFO_NAMELOOKUP_TIME        ,RXCURLINFO_DOUBLE},
   { "PRETRANSFER_TIME"       ,CURLINFO_PRETRANSFER_TIME       ,RXCURLINFO_DOUBLE},
   { "REQUEST_SIZE"           ,CURLINFO_REQUEST_SIZE           ,RXCURLINFO_LONG  },
   { "SIZE_DOWNLOAD"          ,CURLINFO_SIZE_DOWNLOAD          ,RXCURLINFO_DOUBLE},
   { "SIZE_UPLOAD"            ,CURLINFO_SIZE_UPLOAD            ,RXCURLINFO_DOUBLE},
   { "SPEED_DOWNLOAD"         ,CURLINFO_SPEED_DOWNLOAD         ,RXCURLINFO_DOUBLE},
   { "SPEED_UPLOAD"           ,CURLINFO_SPEED_UPLOAD           ,RXCURLINFO_DOUBLE},
   { "SSL_VERIFYRESULT"       ,CURLINFO_SSL_VERIFYRESULT       ,RXCURLINFO_LONG  },
   { "TOTAL_TIME"             ,CURLINFO_TOTAL_TIME             ,RXCURLINFO_DOUBLE},
   { NULL                     ,0                               ,0                }
};

static curl_options RexxCurlPolicies[] =
{
   { "OLDEST"                 ,CURLCLOSEPOLICY_OLDEST             ,0 },
   { "LEAST_RECENTLY_USED"    ,CURLCLOSEPOLICY_LEAST_RECENTLY_USED,0 },
   { "LEAST_TRAFFIC"          ,CURLCLOSEPOLICY_LEAST_TRAFFIC      ,0 },
   { "SLOWEST"                ,CURLCLOSEPOLICY_SLOWEST            ,0 },
   { "CALLBACK"               ,CURLCLOSEPOLICY_CALLBACK           ,0 },
   { NULL                     ,0                                  ,0 }
};

#define NUMBER_REXXCURL_OPTIONS (sizeof(RexxCurlOptions)/sizeof(curl_options))
static char UsedOptions[NUMBER_REXXCURL_OPTIONS];
static FILE *FilePtrs[NUMBER_REXXCURL_OPTIONS];
static char *StringPtrs[NUMBER_REXXCURL_OPTIONS];
static struct curl_slist *SListPtrs[NUMBER_REXXCURL_OPTIONS];
static struct HttpPost *HttpPostFirstPtrs[NUMBER_REXXCURL_OPTIONS];
static struct HttpPost *HttpPostLastPtrs[NUMBER_REXXCURL_OPTIONS];

rxfunc( CurlLoadFuncs );
rxfunc( CurlDropFuncs );
rxfunc( CurlInit );
rxfunc( CurlSetopt );
rxfunc( CurlPerform );
rxfunc( CurlGetinfo );
rxfunc( CurlCleanup );
rxfunc( CurlVariable );

/*-----------------------------------------------------------------------------
 * Table of CURL Functions. Used to install/de-install functions.
 * If you change this table, don't forget to change the table at the end
 * of this file.
 *----------------------------------------------------------------------------*/
RexxFunction RxPackageFunctions[] = {
   { "CURLINIT"         ,CurlInit        ,"CurlInit"        , 1  },
   { "CURLCLEANUP"      ,CurlCleanup     ,"CurlCleanup"     , 1  },
   { "CURLSETOPT"       ,CurlSetopt      ,"CurlSetopt"      , 1  },
   { "CURLPERFORM"      ,CurlPerform     ,"CurlPerform"     , 1  },
   { "CURLGETINFO"      ,CurlGetinfo     ,"CurlGetinfo"     , 1  },
   { "CURLVARIABLE"     ,CurlVariable    ,"CurlVariable"    , 1  },
   { "CURLDROPFUNCS"    ,CurlDropFuncs   ,"CurlDropFuncs"   , 1  },
   { "CURLLOADFUNCS"    ,CurlLoadFuncs   ,"CurlLoadFuncs"   , 0  }, /* Don't lod for DLL */
   { NULL, NULL, NULL,0 }
};

int g_rexxcurl_error = 0;
CURLcode g_curl_error = 0;
char *curl_error[CURL_ERROR_SIZE+1];

char rexxcurl_error_prefix[350];

static int outstem_index=0;
static int outstem_tail=0;
static char *outstem_strptr=NULL;
static int outstem_strlength=0;
static int headerstem_index=0;
static int headerstem_tail=0;

#define DEFAULT_REXXCURL_ERROR        "CURLERROR."
#define INTERRM_PREFIX        "INTERRM"
#define INTCODE_PREFIX        "INTCODE"
#define CURLERRM_PREFIX       "CURLERRM"
#define CURLCODE_PREFIX       "CURLCODE"

#if defined(_MSC_VER)
static void win32_cleanup(void)
{
  WSACleanup();
}

static CURLcode win32_init(void)
{
  WORD wVersionRequested;  
  WSADATA wsaData; 
  int err; 
  wVersionRequested = MAKEWORD(1, 1); 
    
  err = WSAStartup(wVersionRequested, &wsaData); 
    
  if (err != 0) 
    /* Tell the user that we couldn't find a useable */ 
    /* winsock.dll.     */ 
    return 1;
    
  /* Confirm that the Windows Sockets DLL supports 1.1.*/ 
  /* Note that if the DLL supports versions greater */ 
  /* than 1.1 in addition to 1.1, it will still return */ 
  /* 1.1 in wVersion since that is the version we */ 
  /* requested. */ 
    
  if ( LOBYTE( wsaData.wVersion ) != 1 || 
       HIBYTE( wsaData.wVersion ) != 1 ) { 
    /* Tell the user that we couldn't find a useable */ 

    /* winsock.dll. */ 
    WSACleanup(); 
    return 1; 
  }
  return 0; /* 0 is ok */
}
#endif

/*-----------------------------------------------------------------------------
 * This function returns the size of the specified file. Returns -1 if the
 * file does not exist or is not a "normal" file eg a directory
 *----------------------------------------------------------------------------*/
size_t get_file_size( char *fn )
{
   struct stat stat_buf;
   size_t rc;

   rc = stat( fn, &stat_buf ) ;
   if (rc == 0)
   {
      if ( (stat_buf.st_mode & S_IFMT) == S_IFDIR)
         rc = -1;
      else
         rc = stat_buf.st_size;
   }
   return rc;
}

/*-----------------------------------------------------------------------------
 * This function is necessary for Win32 platform
 *----------------------------------------------------------------------------*/
size_t file_write_function( void *ptr, size_t size, size_t nmemb, void *stream )
{
   size_t num_bytes;

   num_bytes = fwrite( ptr, size, nmemb, stream );
   return num_bytes;
}

/*-----------------------------------------------------------------------------
 * This function creates a compound Rexx variable with the supplied name
 * and vale.
 *----------------------------------------------------------------------------*/
int create_rexx_compound( char *stem, int tail, char *value, int valuelen )
{
   char name[350];
   int namelen;

   namelen = sprintf( name, "%s%d", stem, tail );
   SetRexxVariable( name, namelen, value, valuelen );
   return 0;
}
/*-----------------------------------------------------------------------------
 * This function writes the output from the site to a stem. Called from
 * OUTSTEM option
 *----------------------------------------------------------------------------*/
size_t outstem_write_function( void *ptr, size_t size, size_t nmemb, void *stream )
{
   size_t num_bytes=size*nmemb;

   if ( outstem_strlength)
      outstem_strptr = (char *)realloc( outstem_strptr, outstem_strlength + num_bytes + 1);
   else
      outstem_strptr = (char *)malloc( outstem_strlength + num_bytes + 1);

   if ( outstem_strptr == NULL )
   {
      return -1;
   }
   memcpy( outstem_strptr+outstem_strlength, ptr, num_bytes );
   outstem_strlength += num_bytes;
   outstem_strptr[outstem_strlength] = '\0';
   return num_bytes;
}
/*-----------------------------------------------------------------------------
 * This function creates the compound variables for the stem.
 *----------------------------------------------------------------------------*/
int outstem_create( void )
{
   char *tmp,*ptr;

   if ( outstem_strptr )
   {
      ptr = outstem_strptr;
      tmp = strstr( ptr, "\r\n" );
   
      while( tmp != NULL )
      {
         *tmp = '\0';
         create_rexx_compound( StringPtrs[outstem_index], ++outstem_tail, (char *)ptr, strlen( ptr ) ); 
         ptr = tmp+2;
         tmp = strstr( ptr, "\r\n" );
      }
      if ( *ptr != '\0' )
      {
         create_rexx_compound( StringPtrs[outstem_index], ++outstem_tail, (char *)ptr, strlen( ptr ) );
      }
   }

   return 0;
}
/*-----------------------------------------------------------------------------
 * This function writes the output from the site to a stem. Called from
 * HEADERSTEM option
 *----------------------------------------------------------------------------*/
size_t headerstem_write_function( void *ptr, size_t size, size_t nmemb, void *stream )
{
   size_t num_bytes=size*nmemb;
   char *tmp;

   tmp = strtok( ptr, "\n" );
   while( tmp != NULL )
   {
      create_rexx_compound( StringPtrs[headerstem_index], ++headerstem_tail, (char *)tmp, strlen( tmp ) );
      tmp = strtok( NULL, "\n" );
   }

   return num_bytes;
}

/*-----------------------------------------------------------------------------
 * Clear the cURL error message.
 *----------------------------------------------------------------------------*/
void ClearCURLError( void )
{
   char var[350];
   int varlen;

   InternalTrace("ClearCURLError", NULL);

   /* 
    * Set CURLERROR.CURLERRM variable
    */
   varlen = sprintf( var,"%s%s", rexxcurl_error_prefix, CURLERRM_PREFIX );
   (void)SetRexxVariable( var, varlen, "", 0 );
   /* 
    * Set CURLERROR.CURLCODE variable
    */
   varlen = sprintf( var,"%s%s", rexxcurl_error_prefix, CURLCODE_PREFIX );
   (void)SetRexxVariable( var, varlen, "0", 1 );
   g_curl_error = 0;
   return;
}

/*-----------------------------------------------------------------------------
 * Set the cURL error message.
 *----------------------------------------------------------------------------*/
void SetCURLError( CURLcode curlcode, char *curlmsg)
{
   char var[350];
   char msg[350];
   int varlen;
   int msglen;

   InternalTrace("SetCURLError", "%d,%s", curlcode, curlmsg);
   g_curl_error = curlcode;
   /* 
    * Set CURLERROR.CURLERRM variable
    */
   varlen = sprintf( var,"%s%s", rexxcurl_error_prefix, CURLERRM_PREFIX );
   (void)SetRexxVariable( var, varlen, curlmsg, strlen(curlmsg) );
   /* 
    * Set CURLERROR.CURLCODE variable
    */
   msglen = sprintf( msg, "%ld", g_curl_error );
   varlen = sprintf( var,"%s%s", rexxcurl_error_prefix, CURLCODE_PREFIX );
   (void)SetRexxVariable( var, varlen, msg, msglen );
   return;
}

/*-----------------------------------------------------------------------------
 * Clear the internal error message.
 *----------------------------------------------------------------------------*/
int ClearIntError( void )
{
   char var[350];
   int varlen;

   InternalTrace("ClearIntError", NULL );

   g_rexxcurl_error = 0;
   /* 
    * Set CURLERROR.INTERRM variable
    */
   varlen = sprintf( var,"%s%s", rexxcurl_error_prefix, INTERRM_PREFIX );
   (void)SetRexxVariable( var, varlen, "", 0 );
   /* 
    * Set CURLERROR.INTCODE variable
    */
   varlen = sprintf( var,"%s%s", rexxcurl_error_prefix, INTCODE_PREFIX );
   (void)SetRexxVariable( var, varlen, "0", 1 );

   return( g_rexxcurl_error );
}

/*-----------------------------------------------------------------------------
 * Set the internal error message.
 *----------------------------------------------------------------------------*/
int SetIntError(char *fn, int lineno, int errcode, char *errmsg)
{
   char msg[350];
   char var[350];
   int msglen;
   int varlen;

   InternalTrace("SetIntError", "%s,%d,%d,%s", fn, lineno, errcode, errmsg );

   g_rexxcurl_error = -errcode;

   /* 
    * Set CURLERROR.INTERRM variable
    */
   if ( RxGetRunFlags() & MODE_INTERNAL )
      msglen = sprintf(msg, "Rexx/CURL-%02d: %s [%s:%d]", errcode, errmsg, fn, lineno);
   else
      msglen = sprintf(msg, "REXX/CURL-%02d: %s", errcode, errmsg);
   varlen = sprintf( var,"%s%s", rexxcurl_error_prefix, INTERRM_PREFIX );
   (void)SetRexxVariable( var, varlen, msg, msglen );

   /* 
    * Set CURLERROR.INTCODE variable
    */
   msglen = sprintf( msg, "%ld", g_rexxcurl_error );
   varlen = sprintf( var,"%s%s", rexxcurl_error_prefix, INTCODE_PREFIX );
   (void)SetRexxVariable( var, varlen, msg, msglen );

   return( g_rexxcurl_error );
}

static void init_options()
{
   int i;
   for ( i = 0; i < NUMBER_REXXCURL_OPTIONS; i++ )
   {
      FilePtrs[i] = NULL;
      StringPtrs[i] = NULL;
      SListPtrs[i] = NULL;
      HttpPostFirstPtrs[i] = NULL;
      HttpPostLastPtrs[i] = NULL;
      UsedOptions[i] = '\0';
   }
}

static void reset_options()
{
   int i;
   for ( i = 0; i < NUMBER_REXXCURL_OPTIONS; i++ )
   {
      if ( UsedOptions[i] )
      {
         if ( FilePtrs[i] )
         {
            fclose( FilePtrs[i] );
            FilePtrs[i] = NULL;
         }
         if ( StringPtrs[i] )
         {
            free( StringPtrs[i] );
            StringPtrs[i] = NULL;
         }
         if ( SListPtrs[i] )
         {
            curl_slist_free_all( SListPtrs[i] );
            SListPtrs[i] = NULL;
         }
         if ( HttpPostFirstPtrs[i] )
         {
            curl_formfree( HttpPostFirstPtrs[i] );
            HttpPostFirstPtrs[i] = NULL;
            HttpPostLastPtrs[i] = NULL;
         }
         UsedOptions[i] = '\0';
      }
   }
}

static int find_option( char *str, int len )
{
   register int i = 0;

   for ( i = 0; RexxCurlOptions[i].name != NULL; i++ )
   {
      if ( memcmpi( str, RexxCurlOptions[i].name, len ) == 0 )
         return i;
   }
   return ( -1 );
}

static int find_getinfo( char *str, int len )
{
   register int i = 0;

   for ( i = 0; RexxCurlGetinfos[i].name != NULL; i++ )
   {
      if ( memcmpi( str, RexxCurlGetinfos[i].name, len ) == 0 )
         return i;
   }
   return ( -1 );
}

static int find_policy( char *str, int len )
{
   register int i = 0;

   for ( i = 0; RexxCurlPolicies[i].name != NULL; i++ )
   {
      if ( memcmpi( str, RexxCurlPolicies[i].name, len ) == 0 )
         return i;
   }
   return ( -1 );
}

/*====== Here come the real interface functions to curl ======*/

rxfunc( CurlInit )
{
   CURL *curl;

   ClearCURLError( );
   ClearIntError( );
   FunctionPrologue( (char *)name, argc, argv );
   if ( my_checkparam( name, argc, 0, 0 ) )
      return( 1 );
   /*
    * Do some initialisations
    */
   init_options();
   curl = curl_easy_init();
   return RxReturnPointer( retstr, (void *)curl ) ;
}

rxfunc( CurlCleanup )
{
   long curl;

   if ( g_curl_error) ClearCURLError( );
   if ( g_rexxcurl_error) ClearIntError( );
   FunctionPrologue( (char *)name, argc, argv );
   if ( my_checkparam( name, argc, 1, 1 ) )
      return( 1 );
   if ( StrToNumber( &argv[0], &curl ) != 0 )
   {
      SetIntError( __FILE__, __LINE__, INTERR_INVALID_HANDLE, INTERR_INVALID_HANDLE_STRING );
      return RxReturnNumber( retstr, INTERR_INVALID_HANDLE ) ;
   }
   curl_easy_cleanup( (CURL *)curl );
   reset_options();
   memset( UsedOptions, 0, sizeof(char)*NUMBER_REXXCURL_OPTIONS );
   return RxReturnString( retstr, "" ) ;
}

rxfunc( CurlSetopt )
{
   ULONG rc = 0L;
   long curl;
   int i,opt,policy_opt;
   long long_opt;
   CURLcode curl_rc;
   RXSTRING value;
   size_t file_size;
   char *tmp;

   if ( g_curl_error) ClearCURLError( );
   if ( g_rexxcurl_error) ClearIntError( );
   FunctionPrologue( (char *)name, argc, argv );
   if ( my_checkparam( name, argc, 3, 3 ) )
      return( 1 );
   if ( StrToNumber( &argv[0], &curl ) != 0 )
   {
      SetIntError( __FILE__, __LINE__, INTERR_INVALID_HANDLE, INTERR_INVALID_HANDLE_STRING );
      return RxReturnString( retstr, "" ) ;
   }
   opt = find_option( argv[1].strptr, argv[1].strlength );
   if ( opt == (-1) )
   {
      SetIntError( __FILE__, __LINE__, INTERR_INVALID_OPTION, INTERR_INVALID_OPTION_STRING );
      return RxReturnString( retstr, "" ) ;
   }
   switch( RexxCurlOptions[opt].optiontype )
   {
      case RXCURLOPT_STRING:
         if ( StringPtrs[opt] )
            free( StringPtrs[opt] );
         StringPtrs[opt] = (char *)malloc( argv[2].strlength + 1 );
         if ( StringPtrs[opt] == NULL )
         {
            SetIntError( __FILE__, __LINE__, INTERR_NO_MEMORY, INTERR_NO_MEMORY_STRING );
            break;
         }
         memcpy( StringPtrs[opt], argv[2].strptr, argv[2].strlength );
         StringPtrs[opt][argv[2].strlength] = '\0';
         /*
          * Do any extra processing here for some options
          */
         switch( RexxCurlOptions[opt].number )
         {
            default:
               break;
         }
         rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, RexxCurlOptions[opt].number, StringPtrs[opt] );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         break;
      case RXCURLOPT_OUTFILE:
         /* parameter must be the name of a file to read from or blank to turn it off */
         if ( argv[2].strlength )
         {
            FilePtrs[opt] = fopen( argv[2].strptr, "wb" );
            if ( FilePtrs[opt] == NULL )
            {
               SetIntError( __FILE__, __LINE__, INTERR_INVALID_FILE, INTERR_INVALID_FILE_STRING );
               break;
            }
         }
         else
         {
            if ( FilePtrs[opt] )
            {
               fclose( FilePtrs[opt] );
               FilePtrs[opt] = NULL;
            }
         }
         rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, RexxCurlOptions[opt].number, FilePtrs[opt] );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         switch ( RexxCurlOptions[opt].number )
         {
            case CURLOPT_FILE:
               if ( argv[2].strlength )
               {
                  /*
                   * Always set the write function if we have an output file
                   */
                  rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, CURLOPT_WRITEFUNCTION, &file_write_function );
                  if ( curl_rc != 0 )
                  {
                     SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
                     SetCURLError( curl_rc, curl_errors[rc] );
                     break;
                  }
               }
               break;
            default:
               break;
         }
         break;
      case RXCURLOPT_INFILE:
         /* parameter must be the name of a file to read from or blank to turn it off */
         if ( argv[2].strlength )
         {
            FilePtrs[opt] = fopen( argv[2].strptr, "rb" );
            if ( FilePtrs[opt] == NULL )
            {
               SetIntError( __FILE__, __LINE__, INTERR_INVALID_FILE, INTERR_INVALID_FILE_STRING );
               break;
            }
         }
         else
         {
            if ( FilePtrs[opt] )
            {
               fclose( FilePtrs[opt] );
               FilePtrs[opt] = NULL;
            }
         }
         rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, RexxCurlOptions[opt].number, FilePtrs[opt] );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         /*
          * Now do some extra stuff depending on the option specified...
          */
         switch ( RexxCurlOptions[opt].number )
         {
            case CURLOPT_INFILE:
               /*
                * Always set the file size for the specified INFILE
                */
               if ( argv[2].strlength )
               {
                  file_size = get_file_size( argv[2].strptr );
               }
               else
               {
                  file_size = 0;
               }
               if ( file_size == -1 )
               {
                  SetIntError( __FILE__, __LINE__, INTERR_INVALID_FILE, INTERR_INVALID_FILE_STRING );
                  break;
               }
               rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, CURLOPT_INFILESIZE, (long)file_size );
               if ( curl_rc != 0 )
               {
                  SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
                  SetCURLError( curl_rc, curl_errors[rc] );
                  break;
               }
               break;
            default:
               break;
         }

         break;
      case RXCURLOPT_LONG:
         if ( StrToNumber( &argv[2], &long_opt ) != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_INVALID_NUMBER, INTERR_INVALID_NUMBER_STRING );
            break;
         }
         FunctionTrace("CurlSetopt", "Setting LONG value: %ld", long_opt);
         rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, RexxCurlOptions[opt].number, long_opt );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         break;
      case RXCURLOPT_BOOL:
         if ( StrToBool( &argv[2], &long_opt ) != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_INVALID_BOOL, INTERR_INVALID_BOOL_STRING );
            break;
         }
         rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, RexxCurlOptions[opt].number, long_opt );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         break;
      case RXCURLOPT_POLICY:
         policy_opt = find_policy( argv[2].strptr, argv[2].strlength );
         if ( policy_opt == (-1) )
         {
            SetIntError( __FILE__, __LINE__, INTERR_INVALID_OPTION, INTERR_INVALID_OPTION_STRING );
            break;
         }
         long_opt = RexxCurlPolicies[policy_opt].number;
         rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, RexxCurlOptions[opt].number, long_opt );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         break;
      case RXCURLOPT_LIST:
         /*
          * Check that arg[2] is a stem, then convert the stem into
          * a cURL linked list
          */
         if ( argv[2].strptr[argv[2].strlength-1] != '.' )
         {
            SetIntError( __FILE__, __LINE__, INTERR_INVALID_STEM, INTERR_INVALID_STEM_STRING );
            break;
         }
         argv[2].strptr[argv[2].strlength] = '\0';
         if ( GetRexxVariableInteger( argv[2].strptr, &policy_opt, 0 ) == NULL )
         {
            SetIntError( __FILE__, __LINE__, INTERR_INVALID_STEM, INTERR_INVALID_STEM_STRING );
            break;
         }
         if ( SListPtrs[opt] )
         {
            curl_slist_free_all( SListPtrs[opt] );
            SListPtrs[opt] = NULL;
         }
         for ( i = 1; i <= policy_opt; i++ )
         {
            if ( GetRexxVariable( argv[2].strptr, &value, i ) == NULL )
            {
               SetIntError( __FILE__, __LINE__, INTERR_INVALID_STEM, INTERR_INVALID_STEM_STRING );
               return RxReturnString( retstr, "" ) ;
            }
            InternalTrace("CurlSetopt(RXCURLOPT_LIST)", "Variable: <%s.%d> Value: <%s>", argv[2].strptr, i, value.strptr );
            SListPtrs[opt] = curl_slist_append( SListPtrs[opt], value.strptr );
            /*
             * Free the memory allocated in GetRexxVariable()
             */
            free( value.strptr );
            if ( SListPtrs[opt] == NULL )
            {
               SetIntError( __FILE__, __LINE__, INTERR_NO_MEMORY, INTERR_NO_MEMORY_STRING );
               return RxReturnString( retstr, "" ) ;
            }
         }
         rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, RexxCurlOptions[opt].number, SListPtrs[opt] );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         break;
      case RXCURLOPT_POST_DATA:
         /*
          * Check that arg[2] is a stem, then convert the stem into
          * a cURL linked list
          */
         if ( argv[2].strptr[argv[2].strlength-1] != '.' )
         {
            SetIntError( __FILE__, __LINE__, INTERR_INVALID_STEM, INTERR_INVALID_STEM_STRING );
            break;
         }
         argv[2].strptr[argv[2].strlength] = '\0';
         if ( GetRexxVariableInteger( argv[2].strptr, &policy_opt, 0 ) == NULL )
         {
            SetIntError( __FILE__, __LINE__, INTERR_INVALID_STEM, INTERR_INVALID_STEM_STRING );
            break;
         }
         if ( HttpPostFirstPtrs[opt] )
         {
            curl_formfree( HttpPostFirstPtrs[opt] );
            HttpPostFirstPtrs[opt] = NULL;
            HttpPostLastPtrs[opt] = NULL;
         }
         for ( i = 1; i <= policy_opt; i++ )
         {
            if ( GetRexxVariable( argv[2].strptr, &value, i ) == NULL )
            {
               SetIntError( __FILE__, __LINE__, INTERR_INVALID_STEM, INTERR_INVALID_STEM_STRING );
               return RxReturnString( retstr, "" ) ;
            }
            InternalTrace("CurlSetopt(RXCURLOPT_POST_DATA)", "Variable: <%s%d> Value: <%s>", argv[2].strptr, i, value.strptr );
            rc = curl_rc = curl_formparse( value.strptr, &HttpPostFirstPtrs[opt], &HttpPostLastPtrs[opt] );
            /*
             * Free the memory allocated in GetRexxVariable()
             */
            free( value.strptr );
            if ( curl_rc != 0 )
            {
               SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
               SetCURLError( curl_rc, curl_errors[rc] );
               return RxReturnString( retstr, "" ) ;
            }
         }
         rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, RexxCurlOptions[opt].number, HttpPostFirstPtrs[opt] );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         break;
      case RXCURLOPT_POST_FIELDS:
         /*
          * Check that arg[2] is a stem, then convert the stem into
          * a string of values
          */
         if ( argv[2].strptr[argv[2].strlength-1] != '.' )
         {
            SetIntError( __FILE__, __LINE__, INTERR_INVALID_STEM, INTERR_INVALID_STEM_STRING );
            break;
         }
         argv[2].strptr[argv[2].strlength] = '\0';
         if ( GetRexxVariableInteger( argv[2].strptr, &policy_opt, 0 ) == NULL )
         {
            SetIntError( __FILE__, __LINE__, INTERR_INVALID_STEM, INTERR_INVALID_STEM_STRING );
            break;
         }

         for ( i = 1; i <= policy_opt; i++ )
         {
            if ( GetRexxVariable( argv[2].strptr, &value, i ) == NULL )
            {
               SetIntError( __FILE__, __LINE__, INTERR_INVALID_STEM, INTERR_INVALID_STEM_STRING );
               return RxReturnString( retstr, "" ) ;
            }
            if ( StringPtrs[opt] == NULL )
            {
               StringPtrs[opt] = (char *)malloc( value.strlength + 2 );
               if ( StringPtrs[opt] == NULL )
               {
                  SetIntError( __FILE__, __LINE__, INTERR_NO_MEMORY, INTERR_NO_MEMORY_STRING );
                  break;
               }
               strcpy( StringPtrs[opt], "" );
               tmp = StringPtrs[opt];
            }
            else
            {
               StringPtrs[opt] = (char *)realloc( StringPtrs[opt], strlen( StringPtrs[opt]) + value.strlength + 2 );
               if ( StringPtrs[opt] == NULL )
               {
                  SetIntError( __FILE__, __LINE__, INTERR_NO_MEMORY, INTERR_NO_MEMORY_STRING );
                  break;
               }
            }
            /*
             * Concatenate '&' and the new value to the previous string value
             */
            strcat( StringPtrs[opt], "&" );
            strcat( StringPtrs[opt], value.strptr );
            /*
             * Free the memory allocated in GetRexxVariable()
             */
            free( value.strptr );
         }
         InternalTrace("CurlSetopt(RXCURLOPT_POST_FIELDS)", "Value: <%s>", StringPtrs[opt] );
         rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, RexxCurlOptions[opt].number, StringPtrs[opt] );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         /*
          * Now set the length of the generated string...
          */
         rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, CURLOPT_POSTFIELDSIZE, strlen( StringPtrs[opt]) );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         break;
      case RXCURLOPT_OUTSTEM:
         /*
          * Check that arg[2] is a stem, then setup the write callback
          */
         if ( argv[2].strptr[argv[2].strlength-1] != '.' )
         {
            SetIntError( __FILE__, __LINE__, INTERR_INVALID_STEM, INTERR_INVALID_STEM_STRING );
            break;
         }
         /*
          * Save the stem name for the write function...
          */
         if ( StringPtrs[opt] )
            free( StringPtrs[opt] );
         StringPtrs[opt] = (char *)malloc( argv[2].strlength + 1 );
         if ( StringPtrs[opt] == NULL )
         {
            SetIntError( __FILE__, __LINE__, INTERR_NO_MEMORY, INTERR_NO_MEMORY_STRING );
            break;
         }
         memcpy( StringPtrs[opt], argv[2].strptr, argv[2].strlength );
         StringPtrs[opt][argv[2].strlength] = '\0';
         /*
          * Set these globals to allow the write function to know which
          * compound variable to write.
          */
         outstem_index = opt;
         outstem_tail = 0;
         if ( outstem_strptr )
            free( outstem_strptr );
         outstem_strlength = 0;

         rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, CURLOPT_WRITEFUNCTION, &outstem_write_function );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         break;
      case RXCURLOPT_HEADERSTEM:
         /*
          * Check that arg[2] is a stem, then setup the write callback
          */
         if ( argv[2].strptr[argv[2].strlength-1] != '.' )
         {
            SetIntError( __FILE__, __LINE__, INTERR_INVALID_STEM, INTERR_INVALID_STEM_STRING );
            break;
         }
         /*
          * Save the stem name for the write function...
          */
         if ( StringPtrs[opt] )
            free( StringPtrs[opt] );
         StringPtrs[opt] = (char *)malloc( argv[2].strlength + 1 );
         if ( StringPtrs[opt] == NULL )
         {
            SetIntError( __FILE__, __LINE__, INTERR_NO_MEMORY, INTERR_NO_MEMORY_STRING );
            break;
         }
         memcpy( StringPtrs[opt], argv[2].strptr, argv[2].strlength );
         StringPtrs[opt][argv[2].strlength] = '\0';
         /*
          * Set these globals to allow the write function to know which
          * compound variable to write.
          */
         headerstem_index = opt;
         headerstem_tail = 0;

         rc = (ULONG)curl_rc = curl_easy_setopt( (CURL *)curl, CURLOPT_HEADERFUNCTION, &headerstem_write_function );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         break;
      default:
         /* error */
         break;
   }
   return RxReturnString( retstr, "" ) ;
}

rxfunc( CurlPerform )
{
   long curl;
   CURLcode rc;
   char value[20]; /* big enough for an int */
   int valuelen,i;

   if ( g_curl_error) ClearCURLError( );
   if ( g_rexxcurl_error) ClearIntError( );
   FunctionPrologue( (char *)name, argc, argv );
   if ( my_checkparam( name, argc, 1, 1 ) )
      return( 1 );
   if ( StrToNumber( &argv[0], &curl ) != 0 )
   {
      SetIntError( __FILE__, __LINE__, INTERR_INVALID_HANDLE, INTERR_INVALID_HANDLE_STRING );
      return RxReturnString( retstr, "" ) ;
   }
   /*
    * Set the CURLOPT_ERRORBUFFER here to ensure we get a string
    * error if something goes wrong.
    */
   curl_easy_setopt( (CURL *)curl, CURLOPT_ERRORBUFFER, curl_error );
   g_curl_error = rc = curl_easy_perform( (CURL *)curl );
   if ( rc != 0 )
   {
      SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
      SetCURLError( rc, (char *)curl_error );
   }
   /*
    * For those options that return their data in a stem, we need to set
    * the stem.0 value to the number of stem variables actually created.
    */
   if ( outstem_index )
   {
      outstem_create();
      valuelen = sprintf( value, "%d", outstem_tail );
      create_rexx_compound( StringPtrs[outstem_index], 0, value, valuelen );
      outstem_index = outstem_tail = outstem_strlength = 0;
      if ( outstem_strptr )
      {
         free( outstem_strptr );
         outstem_strptr = NULL;
         outstem_strlength = 0;
      }
   }
   if ( headerstem_index )
   {
      valuelen = sprintf( value, "%d", headerstem_tail );
      create_rexx_compound( StringPtrs[headerstem_index], 0, value, valuelen );
      headerstem_index = headerstem_tail = 0;
   }
   /*
    * If OUTFILE is used, close it here and set the FILE * to NULL
    */
   for ( i = 0; i < NUMBER_REXXCURL_OPTIONS; i++ )
   {
      if ( RexxCurlOptions[i].optiontype == RXCURLOPT_OUTFILE
      &&   FilePtrs[i] )
      {
         fclose( FilePtrs[i] );
         FilePtrs[i] = NULL;
      }
   }
   return RxReturnString( retstr, "" ) ;
}

rxfunc( CurlGetinfo )
{
   ULONG rc = 0L;
   CURLcode curl_rc;
   long curl;
   int opt;
   char *return_string=NULL;
   double return_double;
   long return_long;

   if ( g_curl_error) ClearCURLError( );
   if ( g_rexxcurl_error) ClearIntError( );
   FunctionPrologue( (char *)name, argc, argv );
   if ( my_checkparam( name, argc, 2, 2 ) )
      return( 1 );
   if ( StrToNumber( &argv[0], &curl ) != 0 )
   {
      SetIntError( __FILE__, __LINE__, INTERR_INVALID_HANDLE, INTERR_INVALID_HANDLE_STRING );
      return RxReturnString( retstr, "" ) ;
   }
   opt = find_getinfo( argv[1].strptr, argv[1].strlength );
   if ( opt == (-1) )
   {
      SetIntError( __FILE__, __LINE__, INTERR_INVALID_OPTION, INTERR_INVALID_OPTION_STRING );
      return RxReturnString( retstr, "" ) ;
   }
   switch( RexxCurlGetinfos[opt].optiontype )
   {
      case RXCURLINFO_STRING:
         rc = (ULONG)curl_rc = curl_easy_getinfo( (CURL *)curl, RexxCurlGetinfos[opt].number, return_string );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         return RxReturnString( retstr, return_string );
         break;
      case RXCURLINFO_DOUBLE:
         rc = (ULONG)curl_rc = curl_easy_getinfo( (CURL *)curl, RexxCurlGetinfos[opt].number, &return_double );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         return RxReturnDouble( retstr, return_double );
         break;
      case RXCURLINFO_LONG:
         rc = (ULONG)curl_rc = curl_easy_getinfo( (CURL *)curl, RexxCurlGetinfos[opt].number, &return_long );
         if ( curl_rc != 0 )
         {
            SetIntError( __FILE__, __LINE__, INTERR_CURL_ERROR, INTERR_CURL_ERROR_STRING );
            SetCURLError( curl_rc, curl_errors[rc] );
            break;
         }
         return RxReturnNumber( retstr, return_long );
         break;
      default:
         /* error */
         break;
   }
   return RxReturnString( retstr, "" ) ;
}

rxfunc( CurlVariable )
{
   ULONG rc = 0L;
   char buf[250];

   if ( g_curl_error) ClearCURLError( );
   if ( g_rexxcurl_error) ClearIntError( );
   FunctionPrologue( (char *)name, argc, argv );
   if ( my_checkparam( name, argc, 1, 2 ) )
      return( 1 );
   if ( argv[0].strlength == 5 && memcmp( "DEBUG", argv[0].strptr, argv[0].strlength ) == 0 )
   {
      if ( argc == 1 )
      {
         sprintf( buf, "%d", RxGetRunFlags( ) );
         return RxReturnString( retstr, buf );
      }
      else
      {
         RxSetRunFlags ( atoi( (char *)argv[1].strptr ) );
      }
   }
   else if ( argv[0].strlength == 9 && memcmp( "DEBUGFILE", argv[0].strptr, argv[0].strlength ) == 0 )
   {
      if ( argc == 1 )
         return RxReturnString( retstr, RxGetTraceFile() );
      else
      {
         rc = RxSetTraceFile( (char *)argv[1].strptr );
      }
   }
   else if ( argv[0].strlength == 7 && memcmp( "VERSION", argv[0].strptr, argv[0].strlength ) == 0 )
   {
      if ( argc == 1 )
      {
         sprintf( buf, "%s %s %s %s", RxPackageName, RXCURL_VERSION, RXCURL_DATE, curl_version() );
         return RxReturnString( retstr, buf );
      }
      else
      {
         sprintf( buf, "%s: %s", INTERR_READONLY_VARIABLE_STRING, argv[0].strptr );
         SetIntError( __FILE__, __LINE__, INTERR_READONLY_VARIABLE, buf );
      }
   }
   else if ( argv[0].strlength == 5 && memcmp( "ERROR", argv[0].strptr, argv[0].strlength ) == 0 )
   {
      if ( argc == 1 )
      {
         return RxReturnString( retstr, rexxcurl_error_prefix );
      }
      else
      {
         memcpy( rexxcurl_error_prefix, argv[1].strptr, argv[1].strlength );
         rexxcurl_error_prefix[argv[1].strlength] = '\0';
      }
   }
   else
   {
      sprintf( buf, "%s %s", INTERR_INVALID_VARIABLE_STRING, argv[0].strptr );
      SetIntError( __FILE__, __LINE__, INTERR_INVALID_VARIABLE, buf );
   }
   return RxReturnString( retstr, "" );
}

/*====== Routines for handling registration of functions ======*/


rxfunc( CurlLoadFuncs )
{
   ULONG rc = 0L;

#if defined(DYNAMIC_LIBRARY)
   if ( !QueryRxFunction( "CURLINIT" ) )
   {
      rc = InitRxPackage( NULL );
      /* 
       * Register all external functions
       */
      if ( !rc )
         rc = RegisterRxFunctions( );
   }
#endif
   return RxReturnNumber( retstr, rc );
}

rxfunc( CurlDropFuncs )
{
   RFH_RETURN_TYPE rc=0;
   int unload=0;

   FunctionPrologue( ( char* )name, argc, argv );

   if ( my_checkparam( name, argc, 0, 1 ) )
      return( 1 );
   if ( argv[0].strlength == 6
   &&   memcmpi( argv[0].strptr, "UNLOAD", 6 ) == 0 )
      unload = 1;
   rc = ( RFH_RETURN_TYPE )TermRxPackage( RxPackageName, unload );
   return RxReturnNumber( retstr, rc );
}



/*
 * The following functions are used in rxpackage.c
 */

/*-----------------------------------------------------------------------------
 * Print a usage message.
 *----------------------------------------------------------------------------*/
void usage

#ifdef HAVE_PROTO
   (void)
#else
   ()
#endif

{
   (void)fprintf(stderr,
      "\nVersion: %s %s %s\n\nUsage:   %s [-h]\n         %s [-idvf<trace file>] [Rexx program name]\n\n",
      RxPackageName,
      RXCURL_VERSION,
      RXCURL_DATE,
      RxPackageName,
      RxPackageName);
   exit( 1 );
}

/*-----------------------------------------------------------------------------
 * Execute any initialisation
 *----------------------------------------------------------------------------*/
int InitialisePackage

#ifdef HAVE_PROTO
   ( void )
#else
   ( )
#endif

{
#if defined(_MSC_VER)
   if ( win32_init() )
      return 1;
#endif
   curl_global_init(CURL_GLOBAL_ALL);
   strcpy( rexxcurl_error_prefix, DEFAULT_REXXCURL_ERROR );
   return 0;
}

/*-----------------------------------------------------------------------------
 * Execute any termination
 *----------------------------------------------------------------------------*/
int TerminatePackage

#ifdef HAVE_PROTO
   ( void )
#else
   ( )
#endif

{
   curl_global_cleanup();
#if defined(_MSC_VER)
   win32_cleanup();
#endif
   return 0;
}

#if defined(USE_REXX6000)
/*
 * This function is used as the entry point for the REXX/6000
 * interpreter
 * If you change this table, don't forget to change the table at the
 * start of this file.
 */
USHORT InitFunc( RXFUNCBLOCK **FuncBlock )
{
   static RXFUNCBLOCK funcarray[] =
   {
      { "CURADDCH",        CurAddch       ,NULL },
      { "CURDROPFUNCS",    CurDropFuncs   ,NULL },
      { "CURLOADFUNCS",    CurLoadFuncs   ,NULL },
      { NULL, NULL, NULL }
   } ;
   *FuncBlock = funcarray;
   return (USHORT)0;
}
#endif
