<?php  // Moodle configuration file

global $DRUPAL_DB;
global $DRUPAL_COOKIE_PARAMS;

$DRUPAL_DB = array (
  'default' => 
  array (
    'default' => 
    array (
      'database' => 'guo_1userit',
      'username' => 'guo',
      'password' => 'wfKrcRsG7udXcRvZ',
      'host' => 'localhost',
      'port' => '',
      'driver' => 'mysql',
      'prefix' => 'd_',
    ),
  ),
);

$DRUPAL_COOKIE_PARAMS = array(
 'domain' => '.guob.dev.camplus.hk',
 'secure' => 0,
 'httponly' => 1,
);

// There is no php closing tag in this file,
// it is intentional because it prevents trailing whitespace problems!
