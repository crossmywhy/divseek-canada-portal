#
# STAR Informatics adapted Dockerfile for the
# construction of a DivSeek Canada (Tripal-based) Portal
# Version 0.0.1 - 27 September 2018
#
# The starting inspiration for this project is https://github.com/erasche/docker-tripal
#

##########################################################################################################################
# UNSOLVED OR IGNORED TRIPAL DOCKERFILE ISSUES (RMB, October 2018)
#
# 1) Complaint that "debconf: delaying package configuration, since apt-utils is not installed"
#
# 2) Warning messages:
#       update-rc.d: warning: start and stop actions are no longer supported; falling back to defaults
#       invoke-rc.d: could not determine current runlevel
#       invoke-rc.d: policy-rc.d denied execution of start.
#
# 2) Complaint about missing 'unzip' command for php is cryptic. Couldn't yet figure out how to fix this
#   (tried installing a non-existent php7.2-zip package). Abandoning this task for now but if PHP zip file errors
#    rear their ugly head in the system, it will need to be revisited.
#
# 3) untrusted 'tini' GPG signature: the Github repo distributing the software simply states that the software is signed
#    with a given GPG key; however, batch 'trusting' if the key is a challenge. Since the complaint of an 'untrusted'
#    key is non-fatal (and since we are somewhat confident that this key *is* the one for 'tini', this issue is ignored.
#
##########################################################################################################################

FROM php:7.2-apache
MAINTAINER Richard Bruskiewich <richard@starinformatics.com>

# Install packages and PHP-extensions
#
# DivSeek Canada additions to apt package BUILD_DEPS provisioning:
# lynx, vim, phppgadmin (web UI visible through default server port 80?)
#
RUN apt-get -q update && \
    mkdir -p /usr/share/man/man1 /usr/share/man/man7 && \
    DEBIAN_FRONTEND=noninteractive apt-get -yq --no-install-recommends install apt-utils zip unzip && \
    DEBIAN_FRONTEND=noninteractive apt-get -yq --no-install-recommends install \
    file libfreetype6 libjpeg62 libpng16-16 libpq-dev libx11-6 libxpm4 \
    postgresql-client wget patch cron logrotate git nano python python-requests python-setuptools \
    memcached libmemcached11 libmemcachedutil2 gpg dirmngr ca-certificates && \
    BUILD_DEPS="libfreetype6-dev libjpeg62-turbo-dev libmcrypt-dev libpng-dev libxpm-dev re2c zlib1g-dev libmemcached-dev python-pip python-dev libpq-dev" ; \
    DEBIAN_FRONTEND=noninteractive apt-get -yq --no-install-recommends install $BUILD_DEPS \
 && docker-php-ext-configure gd \
        --with-jpeg-dir=/usr/lib/x86_64-linux-gnu --with-png-dir=/usr/lib/x86_64-linux-gnu \
        --with-xpm-dir=/usr/lib/x86_64-linux-gnu --with-freetype-dir=/usr/lib/x86_64-linux-gnu \
 && docker-php-ext-install gd mbstring pdo_pgsql zip \
 && pip install setuptools wheel virtualenv --upgrade \
 && pip install chado==2.1.5 tripal==3.0 \
 && pecl install memcached \
 && apt-get purge -y --auto-remove -o APT::AutoRemove::RecommendsImportant=false -o APT::AutoRemove::SuggestsImportant=false $BUILD_DEPS \
 && rm -rf /var/lib/apt/lists/*

# && pip install --upgrade pip  # breaks system pip to give a 'from pip import main' error?
# && pecl install uploadprogress # not yet compatible with php7 on PECL??
#
# FROM THE TRIPAL INSTALLATION INSTRUCTIONS. NOT SURE THAT THIS ARE NEEDED... MAY ALREADY BE INSTALLED AND CONFIGURE SOMEWHERE ABOVE?
# && DEBIAN_FRONTEND=noninteractive apt-get -yq --no-install-recommends install php-dev php-cli libapache2-mod-php \
# && DEBIAN_FRONTEND=noninteractive apt-get -yq --no-install-recommends install php-pgsql php-gd php-xml \

#####################################################
# Add Tini to monitor and clean up zombie processes #
# See https://github.com/krallin/tini for details   #
#####################################################
ENV TINI_VERSION v0.18.0
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini /tini
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini.asc /tini.asc

# Initially tried to run this GPG validation with the keyserver, hkp://p80.pool.sks-keyservers.net:80, as specified in the tini README
# but it didn't work. Tried hkp://keyserver.ubuntu.com:80 instead... seemed to work, albeit reporting the key as "ultimately untrusted"?
# After reviewing the situation, given that the given public key is published in the README of the web site,
# controlled by the owner of the software, I've decided to 'trust' it.
RUN export GNUPGHOME="$(mktemp -d)" \
 && gpg --keyserver hkp://keyserver.ubuntu.com:80  --recv-keys 595E85A6B1B4779EA4DAAEC70B588DFF0527A9B7 \
 && gpg --verify /tini.asc \
 && rm -rf "$GNUPGHOME" /tini.asc \
 && chmod +x /tini

# Compile a php7 compatible version of uploadprogress module
#RUN cd /tmp && git clone https://github.com/php/pecl-php-uploadprogress.git && cd pecl-php-uploadprogress && phpize && ./configure && make && make install && cd /

EXPOSE 80

# Download Drupal from ftp.drupal.org

# As of September 28, 2018, supports php 7.2.x:
# MONITOR THIS RELEASE FOR FULL MATURE RELEASE OVER COMING WEEKS!
# https://www.drupal.org/project/drupal/releases/7.59
ENV DRUPAL_VERSION=7.59
ENV DRUPAL_TARBALL_MD5=7e09c6b177345a81439fe0aa9a2d15fc

WORKDIR /var/www
RUN rm -R html \
 && curl -OsS https://ftp.drupal.org/files/projects/drupal-${DRUPAL_VERSION}.tar.gz \
 && echo "${DRUPAL_TARBALL_MD5}  drupal-${DRUPAL_VERSION}.tar.gz" | md5sum -c \
 && tar -xf drupal-${DRUPAL_VERSION}.tar.gz && rm drupal-${DRUPAL_VERSION}.tar.gz \
 && mv drupal-${DRUPAL_VERSION} html \
 && cd html \
 && rm [A-Z]*.txt install.php web.config sites/default/default.settings.php

# Install composer and drush by using composer
ENV COMPOSER_BIN_DIR=/usr/local/bin
RUN curl -sS https://getcomposer.org/installer | php -- --install-dir=/usr/local/bin --filename=composer \
 && composer global require drush/drush:7.* \
 && drush cc drush \
 && mkdir /etc/drush && echo "<?php\n\$options['yes'] = TRUE;\n\$options['v'] = TRUE;\n" > /etc/drush/drushrc.php

RUN wget --no-verbose https://github.com/erasche/chado-schema-builder/releases/download/1.31-jenkins110/chado-1.31-tripal.sql.gz -O /chado-master-tripal.sql.gz \
    && wget --no-verbose --no-check-certificate https://drupal.org/files/drupal.pgsql-bytea.27.patch -O /drupal.pgsql-bytea.27.patch

WORKDIR html

# Install elasticsearch php library (required by tripal_elasticsearch)
RUN cd /var/www/html/sites/all/libraries/\
    && mkdir elasticsearch-php \
    && cd elasticsearch-php \
    && composer require "elasticsearch/elasticsearch:~5.0" \
    && cd /var/www/html/

ENV BASE_URL_PATH="/tripal" \
    GALAXY_SHARED_DIR="/tripal-data/" \
    ENABLE_DRUPAL_CACHE=1 \
    ENABLE_OP_CACHE=1 \
    ENABLE_MEMCACHE=1 \
    ENABLE_CRON_JOBS=0 \
    TRIPAL_BASE_CODE_GIT="https://github.com/tripal/tripal.git[@2f2ae83f0f763f7882f1db3bd6c73d3a09b391d6]" \
    TRIPAL_GIT_CLONE_MODULES="https://github.com/abretaud/tripal_rest_api.git[@20b027b2a90b3fb2be050ec1c968da191af826c4] https://github.com/tripal/tripal_elasticsearch.git[@eddac33a464e71f52c5c091cd8aaa7ceced50cc7] https://github.com/tripal/trpdownload_api.git" \
    TRIPAL_DOWNLOAD_MODULES="queue_ui tripal_analysis_interpro-7.x-3.x tripal_analysis_blast-7.x-3.x tripal_analysis_go-7.x-2.1" \
    TRIPAL_ENABLE_MODULES="tripal_genetic tripal_natural_diversity tripal_phenotype tripal_project tripal_pub tripal_stock tripal_analysis_blast tripal_analysis_interpro tripal_analysis_go tripal_rest_api tripal_elasticsearch trpdownload_api"

RUN repo_url=`echo $TRIPAL_BASE_CODE_GIT | sed 's/\(.\+\)\[@\w\+\]/\1/'`; \
    rev=`echo $TRIPAL_BASE_CODE_GIT | sed 's/.\+\[@\(\w\+\)\]/\1/'`; \
    git clone $repo_url /var/www/html/sites/all/modules/tripal; \
    if [ "$repo_url" != "$rev" ]; then \
        cd /var/www/html/sites/all/modules/tripal; \
        git reset --hard $rev; \
        cd /var/www/html/; \
    fi;

# Pre download all default modules
RUN drush pm-download ctools views libraries services ultimate_cron memcache ${TRIPAL_BASE_MODULE} \
    $TRIPAL_DOWNLOAD_MODULES \
    && for repo in $TRIPAL_GIT_CLONE_MODULES; do \
        repo_url=`echo $repo | sed 's/\(.\+\)\[@\w\+\]/\1/'`; \
        rev=`echo $repo | sed 's/.\+\[@\(\w\+\)\]/\1/'`; \
        module_name=`basename $repo_url .git`; \
        git clone $repo_url /var/www/html/sites/all/modules/$module_name; \
        if [ "$repo_url" != "$rev" ]; then \
            cd /var/www/html/sites/all/modules/$module_name; \
            git reset --hard $rev; \
            cd /var/www/html/sites/all/modules/; \
        fi; \
    done

#
# This Tripal 'view' patch seems totally Tripal v.2 specific
# Found at https://github.com/tripal/tripal/blob/7.x-2.x/tripal_views/views-sql-compliant-three-tier-naming-1971160-30.patch
#
# But, the correspondding view directory - https://github.com/tripal/tripal/blob/7.x-3.x/tripal_views/ - is *totally* absence,
# thus, I am ignoring it in this attempted v3.0 build!
#
#RUN cd /var/www/html/sites/all/modules/views \
#    && patch -p1 < ../tripal/tripal_views/views-sql-compliant-three-tier-naming-1971160-30.patch \
#   && cd /var/www/html/

#
# This custom search function also seems absent in v.3.0, so I ignore it for now?
#
# Add custom functions
#ADD search.sql /search.sql

#
# This php config file also seems absent in v.3.0, so I ignore it for now?
# However, see additional Apache2 configuration below
#
# Add PHP-settings
#ADD php-conf.d/ $PHP_INI_DIR/conf.d/

#
# This logrotate config file also seems absent in v.3.0, so I ignore it for now?
#
# Add logrotate conf
#ADD logrotate.d/tripal /etc/logrotate.d/

#
# This etc/tripal/settings.php file also seems absent in v.3.0, so I ignore it for now?
#
# copy sites/default's defaults
#ADD etc/tripal/settings.php /etc/tripal/settings.php

#
# This entrypoint.sh README.md file also seems absent in v.3.0, so I ignore it for now?
#
# Add README.md, entrypoint-script and scripts-folder
#ADD entrypoint.sh README.md  /

ADD /scripts/ /scripts/

#
# This tripal_apache.conf file also seems absent in v.3.0, so I ignore it for now?
#
#ADD tripal_apache.conf /etc/apache2/conf-enabled/tripal_apache.conf

###########################################
# Configuring Apache2 for Tripal / Drupal #
###########################################

# Update apache2 configuration for drupal: enable rewrites, proxy and proxy_http
RUN a2enmod rewrite && a2enmod proxy && a2enmod proxy_http

# Configure Apache2 root html document access for Drupal and Tripal
COPY scripts/apache2_drupal.conf /etc/apache2/sites-available/000-default.conf

# Use the default production PHP configuration...
RUN mv $PHP_INI_DIR/php.ini-production $PHP_INI_DIR/php.ini

# But override with custom Drupal / Tripal settings memory settings
COPY config/tripal_php.ini $PHP_INI_DIR/conf.d/

# Provide compatibility for images depending on previous versions
RUN ln -s /var/www/html /app

# Stub pages to just to check that Apache and PHP are working...
COPY scripts/testpage.html /var/www/html/index.html
COPY scripts/phptest.php   /var/www/html/phptest.php

# Then, restart the server
RUN apache2ctl restart

# Run the system entrypoint under tini?
#ENTRYPOINT ["/tini", "--version"]
#CMD ["/entrypoint.sh"]
