# Get base image
FROM debian:buster

# Get necessary applications
RUN set -ex; \
	apt-get update; \
	apt-get -y install \
	# to run nginx
	nginx; \
	# Remove apt-get caches
	rm -rf /var/lib/apt/lists;

# Copy files in conf directory to docker container
COPY conf/webserv-docker.conf /etc/nginx/sites-available/webserv-docker.conf
COPY www/ /var/www/html/

# Create simlink in sites-enabled directory
RUN ln -s /etc/nginx/sites-available/webserv-docker.conf /etc/nginx/sites-enabled/webserv-docker.conf

# set daemon off to run nginx in foreground
RUN echo "daemon off;" >> /etc/nginx/nginx.conf

# Change owner of all files in the www directory
RUN chown -R www-data:www-data /var/www; \
	chmod 400 /var/www/html/readable.html; \
	chmod 200 /var/www/html/writable.html; \
	chmod 100 /var/www/html/executable.html; \
	chmod 000 /var/www/html/dir_forbidden; \
	chmod -R 755 /var/www/html/dir_on; \
	chmod 000 /var/www/html/dir_on/sub_dir_forbidden; \
	chmod 000 /var/www/html/error_404_not_permitted.html;

# Keep running the container
ENTRYPOINT [ "nginx" ]
