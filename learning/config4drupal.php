<?php  // Moodle configuration file

unset($CFG);
global $CFG;
$CFG = new stdClass();

$CFG->dbtype    = 'mysqli';
$CFG->dblibrary = 'native';
$CFG->dbhost    = 'localhost';
$CFG->dbname    = 'guo_1userit';
$CFG->dbuser    = 'guo';
$CFG->dbpass    = 'wfKrcRsG7udXcRvZ';
$CFG->prefix    = 'mdl_';
$CFG->dboptions = array (
  'dbpersist' => 0,
  'dbsocket' => 0,
);

$CFG->wwwroot   = 'http://guob.dev.camplus.hk/learning';
$CFG->dataroot  = '/home/wwwroot/guob.dev.camplus.hk/moodledata';
$CFG->admin     = 'admin';

$CFG->directorypermissions = 0777;

global $DRUPAL_DB;

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

// There is no php closing tag in this file,
// it is intentional because it prevents trailing whitespace problems!
