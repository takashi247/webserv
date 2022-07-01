#!/usr/bin/perl

use CGI;

$query = new CGI;

print $query->header();

print "hello";
print chr(26);
