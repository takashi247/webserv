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
COPY confs/nginx.conf /etc/nginx/nginx.conf
COPY www/ /var/www/html/

# Create simlink in sites-enabled directory
# RUN ln -s /etc/nginx/sites-available/webserv-docker.conf /etc/nginx/sites-enabled/webserv-docker.conf

# set daemon off to run nginx in foreground
RUN echo "daemon off;" >> /etc/nginx/nginx.conf

# Change owner of all files in the www directory
RUN chown -R www-data:www-data /var/www;

RUN chmod 333 /var/www/html/no_perm.html

# Keep running the container
ENTRYPOINT [ "nginx" ]
