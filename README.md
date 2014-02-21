Library-DIY
===========

Library DIY Installation
------------------------

Library DIY is built on Drupal, so the basic Drupal installation will end in a
working system.


1. Create a database and user account.


2. Install the software to the web server's root. This will place Library DIY in a subdirectory /diy off the web server's root. Note: Library DIY can be installed in a different location, including the site root with a minor change to the .htaccess file. Please see Appendix A for for more information.

  ```
  cd /var/www/html
  git clone git@libsrv2.lib.pdx.edu:web/library-diy.git diy
  cd diy
  git checkout master
  ```

3. Configure Drupal.

  a. Copy the the default settings file.

  ```
  cp sites/default/default.settings.php sites/default/settings.php
  ```

  b. Edit $databases (to set the database connection information) and $drupal_hash_salt.
  
  ```php

    $databases = array (
      'default' => 
      array (
        'default' => 
        array (
          'database' => 'diy_db_name',
          'username' => 'diy_db_user',
          'password' => 'diy_db_pass',
          'host' => 'db_server_name',
          'port' => '',
          'driver' => 'mysql',
          'prefix' => '',
        ),
      ),
    );

    $drupal_hash_salt = 'big_long_string';
    
  ```


4. Add the site files directory.

  ```
  mkdir sites/default/files
  ```

5. Set permissions on writable directories.

  ```
  # Debian, Ubuntu, etc.
  chown -R www-data.www-data diy/sites/default 

  # RHEL, CentOS, etc.
  chown -R apache.apache diy/sites/default
  ```

6. Load the initial database tables.

  ```
  mysql -u diy_db_user -p diy_db_name < library-diy.mysql
  ```

7. Login to the site using the default admin account

  ```
  http://server-name/diy/user
  user: admin
  pass: yiddy
  ```
  
8. Adding a custom header/footer (optional)
  ```
  To add a custom header/footer, edit the core template file:
    \sites\all\themes\diy\page.tpl.php
   
  Replace with custom header code:
  <!-- CUSTOM HEADER GOES HERE -->
   
  Replace with custom footer code:
  <!-- CUSTOM FOOTER GOES HERE -->
  ```


Getting Started & Creating Content
------------------------

[View the Library DIY Wiki](https://github.com/pdxlibrary/Library-DIY/wiki) for tips on getting started with Library DIY.


Appendix
------------------------

Appendix A: Installing Library DIY in a location other than the default location
  ```
  .htaccess changes – set the RewriteBase to "/" to install at the webroot or change 
  {subdirectory} to the path where Library DIY is installed off of the webroot.
  
  # Modify the RewriteBase if you are using Drupal in a subdirectory or in a
  # VirtualDocumentRoot and the rewrite rules are not working properly.
  # For example if your site is at http://example.com/drupal uncomment and
  # modify the following line:
  # RewriteBase /drupal
  #
  # If your site is running in a VirtualDocumentRoot at http://example.com/,
  # uncomment the following line:
  RewriteBase /{subdirectory}
  ```
