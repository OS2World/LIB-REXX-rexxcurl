Summary: Rexx external function package to use cURL
Name: RexxCURL
Version: 1.0
Release: 1
Copyright: LGPL
Group: Development/Languages
Source: ftp://prdownloads.sourceforge.net/rexxcurl/RexxCURL-1.0.tar.gz
%description
Rexx/Tk is an external function package for interfacing to cURL, 7.8 or
above.  It enables Rexx programmers to maniate URL based resources
by calling functions that wrap the cURL easy functions provided in libcurl.
It is also available on several other operating systems. 
For more information on Rexx/CURL, visit http://rexxcurl.sourceforge.net/
For more information on Rexx, visit http://www.rexxla.org/
For more information on cURL, visit http://curl.haxx.se/
%prep
%setup

%build
./configure --with-rexx=rexxtrans
make

%install
make install

%files
/usr/bin/rexxcurl
/usr/lib/librexxcurl.so
/usr/share/rexxcurl/testcurl.rexx
/usr/share/rexxcurl/httppost.rexx
/usr/share/rexxcurl/scp.rexx
/usr/share/rexxcurl/upload.rexx

%post
ldconfig /usr/lib
