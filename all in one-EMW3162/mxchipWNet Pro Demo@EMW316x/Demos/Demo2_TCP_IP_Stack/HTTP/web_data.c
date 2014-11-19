const char headerPage[]={
"HTTP/1.1 200 OK\r\n\
Server: MySocket Server\r\n\
Date: TEST\r\n\
Content-Type: text/html\r\n\
Content-Length: %d\r\n\
Connection: close\r\n\
Accept-Ranges: bytes\r\n\r\n"
};

const char HTTPSaveResponse[] = {
"HTTP/1.1 200 OK\r\n\
Server: MySocket Server\r\n\
Date: TEST\r\n\
Content-Type: text/html\r\n\
Content-Length: %d\r\n\
Accept-Ranges: bytes\r\n\
Connection: close\r\n\r\n\
%s"
};

const char authrized[] = {
"HTTP/1.1 401 Authorization Required\r\n"
"Server: MySocket Server\r\n"
"WWW-Authenticate: Basic realm=\"MXCHIP EMW316x\"\r\n"
"Content-Type: text/html\r\n"
"Content-Length: 169\r\n\r\n"
"<HTML>\r\n<HEAD>\r\n<TITLE>Error</TITLE>\r\n"
"<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=ISO-8859-1\">\r\n"
"</HEAD>\r\n<BODY><H1>401 Unauthorized.</H1></BODY>\r\n</HTML>"
};

const char not_found[] = {
"HTTP/1.1 200 OK\r\n\
Server: MySocket Server\r\n\
Content-Length: 145\r\n\
Connection: close\r\n\
Content-Type: text/html\r\n\r\n\
<html><head><title>404 Not Found</title></head><body>\r\n\
<h1>Not Found</h1>\r\n\
<p>The requested URL was not found on this server.</p>\r\n\
</body></html>"
};


const char systemPage[] = {
"<html><head><title>System Setting</title>\r\n\
</head>\r\n\
<body>\
<br /><font size=\"6\" color=\"red\">mxchipWNet HTTP server and OTA Demo</font><br /><br />\r\n\
<br />Firmware Version:&nbsp;%s&nbsp;<br />\r\n\
<form action=\"settings.htm\" method=\"post\">\r\n\
<table id=\"displayme\" border=\"0\" width=\"500\" cellspacing=\"2\">\r\n\
<col align=\"right\" /> <col align=\"left\" />\r\n\
<tbody><tr><td>SSID:&nbsp;</td>\r\n\
<td><Input type=\"text\" name=\"SSID\" value = \"%s\"/></td></tr>\r\n\
<tr><td>Key:&nbsp;</td> <td><Input type=\"text\" name=\"pass\" value= \"%s\"/></td></tr>\r\n\
</tbody></table><br />\r\n\
<INPUT type=\"submit\" name=\"save\" value=\"Save\">\
<INPUT type=\"submit\" name=\"reset\" value=\"Reset\" ><br />\
</FORM>\r\n\
<FORM ENCTYPE=\"multipart/form-data\" action=\"update.htm\" METHOD=POST>\r\n\
<label>Update firmware: <input type=\"file\" name=\"imagefile\" accept=\"bin\"></label>\
<input type=\"submit\" name=\"upload\" value=\"Upload\">\
</FORM></body></html>\r\n"
};

const char systemResponseSucc[]={
"<html>\r\n\
<head>\r\n\
<title>MXCHIP Wi-Fi module</title>\r\n\
</head>\r\n\
<body>\r\n\
<p>Firmware update success, system reboot...please wait 5 seconds and refresh</p>\r\n\
</body>\r\n\
</html>"};

const char systemResponseError[]={
"<html>\r\n\
<head>\r\n\
<title>MXCHIP Wi-Fi module</title>\r\n\
</head>\r\n\
<body>\r\n\
<p>Firmware update fail, system reboot...please wait 5 seconds and refresh</p>\r\n\
</body>\r\n\
</html>"};


const char SaveResponseSucc[]={
"<html>\r\n\
<head>\r\n\
<title>MXCHIP Wi-Fi module</title>\r\n\
</head>\r\n\
<body>\r\n\
<p>Save Config Done!<a href=\"/system.htm\">Return</a></p>\r\n\
</body>\r\n\
</html>"};

const char SaveResponseError[]={
"<html>\r\n\
<head>\r\n\
<title>MXCHIP Wi-Fi module</title>\r\n\
</head>\r\n\
<body>\r\n\
<p>Save Config Error, please retry<a href=\"/system.htm\">Return</a></p>\r\n\
</body>\r\n\
</html>"};

const char ResponseReset[]={
"<html>\r\n\
<head>\r\n\
<title>MXCHIP Wi-Fi module</title>\r\n\
</head>\r\n\
<body>\r\n\
<p>Reset system, please wait 5 seconds and refresh</p>\r\n\
</body>\r\n\
</html>"};

