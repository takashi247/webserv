FROM httpd:2.4

# install required packages to use CGI module
RUN apt update; \
  apt install -y libcgi-session-perl vim;

# copy html/cgi/config documents
COPY ./www/ /usr/local/apache2/htdocs/
COPY ./conf/my-httpd.conf /usr/local/apache2/conf/httpd.conf
COPY ./cgi-bin/* /usr/local/apache2/cgi-bin/

# run some shell commands to change permissions etc.
RUN sed -i '1c#!/usr/bin/perl' /usr/local/apache2/cgi-bin/printenv; \
  sed -i '1c#!/bin/sh' /usr/local/apache2/cgi-bin/test-cgi; \
  chmod +x /usr/local/apache2/cgi-bin/printenv; \
  chmod +x /usr/local/apache2/cgi-bin/test-cgi; \
  mkdir -p /usr/local/apache2/htdocs/upload; \
  chown -R www-data:www-data /usr/local/apache2/;

# run program foreground to properly handle cgi request
CMD httpd-foreground -c "LoadModule cgid_module modules/mod_cgid.so"