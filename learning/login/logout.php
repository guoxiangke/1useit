<?php

// This file is part of Moodle - http://moodle.org/
//
// Moodle is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Moodle is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Moodle.  If not, see <http://www.gnu.org/licenses/>.

/**
 * Logs the user out and sends them to the home page
 *
 * @package    core
 * @subpackage auth
 * @copyright  1999 onwards Martin Dougiamas  http://dougiamas.com
 * @license    http://www.gnu.org/copyleft/gpl.html GNU GPL v3 or later
 */

require_once('../config.php');

$PAGE->set_url('/login/logout.php');
$PAGE->set_context(context_system::instance());

$sesskey = optional_param('sesskey', '__notpresent__', PARAM_RAW); // we want not null default to prevent required sesskey warning
$login   = optional_param('loginpage', 0, PARAM_BOOL);

// can be overridden by auth plugins
if ($login) {
    $redirect = get_login_url();
} else {
    $redirect = $CFG->wwwroot.'/';
}

if (!isloggedin()) {
    // no confirmation, user has already logged out
    require_logout();

        if( $drupal_dbh = drupal_db_conn() ){
          global $DRUPAL_DB;
          global $DRUPAL_COOKIE_PARAMS;
          foreach($_COOKIE as $key=>$val){
          if(preg_match('/^[SESS|SSESS]/',$key)){
           $drupal_sid = $val;
           $d_sql = 'SELECT sid from '.$DRUPAL_DB['default']['default']['prefix']."sessions WHERE sid='$drupal_sid'";
            //file_put_contents('/tmp/setcoookie.txt',$d_sql."\n" ,FILE_APPEND);
           $res = mysql_query( $d_sql,$drupal_dbh );
           if( mysql_num_rows($res) ){
            setcookie($key, '', (int) $_SERVER['REQUEST_TIME'] - 3600, '/', $DRUPAL_COOKIE_PARAMS['domain'], $DRUPAL_COOKIE_PARAMS['secure'], $DRUPAL_COOKIE_PARAMS['httponly']);
            unset($_COOKIE[$key]);
            
            $d_sql = 'DELETE from '.$DRUPAL_DB['default']['default']['prefix']."sessions WHERE sid='$drupal_sid'";
            mysql_query( $d_sql,$drupal_dbh );
            //file_put_contents('/tmp/setcoookie.txt',$d_sql."\n" ,FILE_APPEND);
            }
           }
          }
        }
        else{
          die('Can not connect to drupal database! '.__FUNCTION__.' ' .__LINE__);
        }

    redirect($redirect);

} else if (!confirm_sesskey($sesskey)) {
    $PAGE->set_title($SITE->fullname);
    $PAGE->set_heading($SITE->fullname);
    echo $OUTPUT->header();
    echo $OUTPUT->confirm(get_string('logoutconfirm'), new moodle_url($PAGE->url, array('sesskey'=>sesskey())), $CFG->wwwroot.'/');
    echo $OUTPUT->footer();
    die;
}

$authsequence = get_enabled_auth_plugins(); // auths, in sequence
foreach($authsequence as $authname) {
    $authplugin = get_auth_plugin($authname);
    $authplugin->logoutpage_hook();
}

require_logout();

redirect($redirect);
