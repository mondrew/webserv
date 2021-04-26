#!/usr/bin/python

import os

print "Content-type: text/html\r\n\r\n";
print "<font size=+2><u><i>Environment meta-variables</u></i></font><br><br>";
for param in os.environ.keys():
	print "<b>%20s</b>: %s<br>" % (param, os.environ[param])
