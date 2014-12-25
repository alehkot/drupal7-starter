<?php

/**
 * @file
 * Drush integration for Rules filter.
 */

/**
 * The Rules filter plugin URI.
 */
define('RULES_FILTER_PLUGIN_DOWNLOAD_URI', 'https://raw.githubusercontent.com/javve/list.js/v1.1.1/dist/list.min.js');

/**
 * Implements hook_drush_command().
 */
function rules_filter_drush_command() {
  $items = array();

  // The key in the $items array is the name of the command.
  $items['rules-filter-plugin'] = array(
    'description' => dt("Downloads the List.js plugin for Rules filter module."),
    'arguments' => array(
      'path' => dt('Optional. A path where to install the List.js plugin. If omitted Drush will use the default location.'),
    ),
  );
  return $items;
}

/**
 * Implements hook_drush_help().
 */
function rules_filter_drush_help($section) {
  switch ($section) {
    case 'drush:rules-filter-plugin':
      return dt("Downloads the List.js plugin, default location is sites/all/libraries.");
  }
}

/**
 * Implements drush_MODULE_post_COMMAND().
 */
function drush_rules_filter_post_pm_enable() {
  $extensions = func_get_args();

  // Deal with comma delimited extension list.
  if (strpos($extensions[0], ',') !== FALSE) {
    $extensions = explode(',', $extensions[0]);
  }

  if (in_array('rules_filter', $extensions) && !drush_get_option('skip')) {
    drush_rules_filter_plugin();
  }
}

/**
 * Commands to download the List.js plugin.
 */
function drush_rules_filter_plugin($path = 'sites/all/libraries') {

  // Create the libraries folder if it does not exist.
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created', array('@path' => $path)), 'notice');
  }

  // Create the plugin folder if it does not exist.
  $path = $path . '/listjs';
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created', array('@path' => $path)), 'notice');
  }

  // Set the directory to the download location.
  $olddir = getcwd();
  chdir($path);

  $filename = basename(RULES_FILTER_PLUGIN_DOWNLOAD_URI);

  // Remove any existing List.js plugin file.
  if (is_file($filename)) {
    drush_log(dt('A existing List.js plugin was overwritten at @path', array('@path' => $path)), 'notice');
    drush_op('unlink', $filename);
  }

  // Download the plugin file.
  if (!drush_shell_exec('wget ' . RULES_FILTER_PLUGIN_DOWNLOAD_URI)) {
    drush_shell_exec('curl -O ' . RULES_FILTER_PLUGIN_DOWNLOAD_URI);
  }

  // Set working directory back to the previous working directory.
  chdir($olddir);

  if (is_file(RULES_FILTER_LISTJS_PATH)) {
    drush_log(dt('List.js plugin has been downloaded to @path.', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to download the List.js plugin to @path', array('@path' => $path)), 'error');
  }
}
