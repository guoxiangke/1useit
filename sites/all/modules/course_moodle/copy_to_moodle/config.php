<?php
$CFG = new stdClass();

/**
 * Get CFG from drupal settings file.
 *
 * Running in a function so we are jailed.
 */
function setup_from_drupal_db($CFG) {
  if (!$_SERVER['HTTP_HOST']) {
    return FALSE;
  }

  $cfgfile = file_get_contents(conf_path() . "/settings.php");
  $cfgfile = preg_replace('#^ini_set.*#m', '', $cfgfile);
  $cfgfile = str_replace('<?php', '', $cfgfile);
  eval($cfgfile);
  $url = parse_url($db_url);
  $CFG->dbtype = $url['scheme'];
  $CFG->dbuser = $url['user'];
  $CFG->dbpass = $url['pass'];
  $CFG->dbhost = $url['host'];
  $CFG->dbname = trim($url['path'], '/');
  $CFG->drupal_db_prefix = $db_prefix;
  $CFG->prefix = $db_prefix . 'mdl_';
  if (isset($cookie_domain)) {
    $CFG->drupal_cookie_domain = $cookie_domain;
  }
  else {
    $CFG->drupal_cookie_domain = $_SERVER['HTTP_HOST'];
  }

  return TRUE;
}

/**
 * Taken from Drupal's bootstrap.inc.
 */
function conf_path($require_settings = TRUE, $reset = FALSE) {
  static $conf = '';

  if ($conf && !$reset) {
    return $conf;
  }

  $confdir = str_replace('/moodle', '', dirname(__FILE__)) . '/sites';
  $uri_string = $_SERVER['SCRIPT_NAME'] ? $_SERVER['SCRIPT_NAME'] : $_SERVER['SCRIPT_FILENAME'];
  $uri_string = substr($uri_string, 0, strpos($uri_string, '/moodle') +1);
  $uri = explode('/', $uri_string);
  $server = explode('.', implode('.', array_reverse(explode(':', rtrim($_SERVER['HTTP_HOST'], '.')))));
  for ($i = count($uri) - 1; $i > 0; $i--) {
    for ($j = count($server); $j > 0; $j--) {
      $dir = implode('.', array_slice($server, -$j)) . implode('.', array_slice($uri, 0, $i));
      if (file_exists("$confdir/$dir/settings.php") || (!$require_settings && file_exists("$confdir/$dir"))) {
        $conf = "$confdir/$dir";
        return $conf;
      }
    }
  }
  $conf = "$confdir/default";
  return $conf;
}


if (!setup_from_drupal_db($CFG)) {
  die('Could not create moodle config.php from drupal settings.php');
}

$CFG->dbpersist =  false;

// Check for protocol. So we can run moodle in whatever the request came in on.
// IIS ISAPI value is 'off'
$proto = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] != 'off') ? 'https' : 'http';
$CFG->wwwroot   = "$proto://". $_SERVER['HTTP_HOST'] .'/moodle';
$CFG->dirroot   = dirname(__FILE__);
$CFG->dataroot   = conf_path() . '/files/moodle';
$CFG->admin     = 'admin';

$CFG->directorypermissions = 00777;

// Change me
$CFG->passwordsaltmain = '$1$rIJCHALX$kodApu6laI7EFPfgla4Ga0';

require_once("$CFG->dirroot/lib/setup.php");
