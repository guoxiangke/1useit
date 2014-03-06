<?php
/** auth_drupal_sync_users.php
 *
 * This script is meant to be called from a system cronjob to
 * sync moodle user accounts with the drupal database.
 *
 * Recommended cron entry:
 * # 5 minutes past 4am
 * 5 4 * * * /usr/bin/php -c /etc/php4/cli/php.ini /var/www/moodle/auth/drupal/auth_drupal_sync_users.php
 *
 * Notes:
 *   - If you have a large number of users, you may want to raise the memory limits
 *     by passing -d memory_limit=256M
 *   - For debugging & better logging, you are encouraged to use in the command line:
 *     -d log_errors=1 -d error_reporting=E_ALL -d display_errors=0 -d html_errors=0
 *
 */


if (isset($_SERVER['REMOTE_ADDR'])) {
    error_log("should not be called from web server!");
    exit;
}

$nomoodlecookie = true; // cookie not needed

require_once(dirname(dirname(dirname(__FILE__))).'/config.php'); // global moodle config file.

require_once($CFG->libdir.'/blocklib.php');
require_once($CFG->dirroot.'/course/lib.php');
require_once($CFG->dirroot.'/mod/resource/lib.php');
require_once($CFG->dirroot.'/mod/forum/lib.php');

if (!is_enabled_auth('drupal')) {
    echo "Plugin not enabled!\n";
    die;
}

$auth_plugin = get_auth_plugin('drupal');
$auth_plugin->sync_users(true);

?>