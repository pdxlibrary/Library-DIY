<?php

/**
 * @file
 * Default views.
 */

/**
 * Implementation of hook_views_default_views().
 */
function fusion_apply_ui_test_views_default_views() {
  static $views;

  if (isset($views)) {
    return $views;
  }

  $files = file_scan_directory(drupal_get_path('module', 'fusion_apply_ui_test') . '/views_default', '/\.inc$/');
  foreach ($files as $filepath => $file) {
    include $filepath;
    if (isset($view)) {
      $views[$view->name] = $view;
    }
  }
  return $views;
}
