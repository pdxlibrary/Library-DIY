<?php

/**
 * @file
 * Common helper functions used by custom breadcrumbs submodules.
 */

/** * Determines if two paths match, allowing for wildcards and aliases.
 *
 * @param $curpath
 *   The current Drupal path.
 * @param $breadcrumb_path
 *   The path that the breadcrumb applies to.
 *
 * @return
 *    TRUE (1) if the paths match, FALSE (0) otherwise.
 */
function _custom_breadcrumbs_match_path($curpath, $breadcrumb_path) {
 
  $path = drupal_get_path_alias($curpath);
  // Compare with the internal and path alias (if any).
  $page_match = drupal_match_path(drupal_strtolower($path), drupal_strtolower($breadcrumb_path));
  if ($path != $curpath) {
    $page_match = $page_match || drupal_match_path(drupal_strtolower($curpath), drupal_strtolower($breadcrumb_path));
  }
  return $page_match;
}

/**
 * Constructs the view path replacing wildcards with arguments.
 *
 * @param $display
 *   The view $display object.
 * @param $viewsargs
 *   The $view->args array.
 *
 * @return $viewpath
 *   The complete path to the view.
 */
function _custom_breadcrumbs_construct_view_path($display) {
  // @todo changes needed for D7?
  $bits = explode('/', $display->display_options['path']);
  $args = arg();
  foreach ($bits as $pos => $bit) {
    if (!empty($args)) {
      $arg = array_shift($args);
      if ($bit == '%') {
        $bits[$pos] = $arg;
      }
    }
  }
  if (!empty($args)) {
    // Add any additional arguments to end of path.
    $bits = array_merge($bits, $args);
  }
  $viewpath = implode('/', $bits);
  return $viewpath;
}

/**
 *  Determines if a view display is appropriate for assigning a custom breadcrumb.
 *
 * @param $display
 *   The view $display object.
 *
 * @return
 *   TRUE if the display should be considered for a custom breadcrumb, FALSE otherwise.
 */
function _custom_breadcrumbs_allowed_display($display) {
  // @todo Changes needed for D7?
  $allowed_display_types = array('page', 'calendar', 'image_gallery');
  if (in_array($display->display_plugin, $allowed_display_types)) {
    if (!(isset($display->handler->view->is_attachment) && $display->handler->view->is_attachment)) {
      if (isset($display->display_options['path']) ) {
        if (module_exists('panels') && panels_get_current_page_display()) {
          return FALSE;
        }
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 *  Returns the appropriate type and value of each view argument.
 *
 * @param $display_argument_ids
 *   An array of ids for each argument of the view.
 * @param $viewargs
 *   The $display->handler->view->args array.
 *
 * @return $arg_values
 *   An associative array of two elements, 'types' and 'values', each an array with elements
 *   corresponding to the views arguments.
 */
function _custom_breadcrumbs_views_parse_args($arguments, $viewargs) {
  // @todo Changes needed for D7?
  $arg_values = array(
    'types' => array(),
    'values' => array(),
  );
  foreach ($arguments as $arg_id => $argument) {
    if (!empty($viewargs)) {
      $arg = array_shift($viewargs);
      $arg_id_3 = drupal_substr($arg_id, 0, 3);
      if (($arg_id_3 == 'tid') || (drupal_substr($arg_id, 0, 19) == 'term_node_tid_depth')) {
        $terms = custom_breadcrumbs_taxonomy_terms_parse_string($arg);
        $arg_values['types'][] = 'tid';
        $arg_values['values'][] = empty($terms['tids']) ? NULL : $terms['tids'][0];
      }
      elseif (drupal_substr($arg_id, 0, 4) == 'name') {
        if (drupal_substr($argument['table'], 0, 5) == 'term_') {
          $terms = taxonomy_get_term_by_name($arg);
          $arg_values['types'][] = 'tid';
          $arg_values['values'][] = empty($terms) ? NULL : $terms[0]->tid;
        }
      }
      elseif (($arg_id_3 == 'vid') || ($arg_id_3 == 'uid') || ($arg_id_3 == 'nid')) {
        $arg_values['types'][] = $arg_id_3;
        $arg_values['values'][] = $arg;
      }
    }
  }
  return $arg_values;
}

/**
 * Obtains the appropriate objects for token type replacement for a view display.
 *
 * @param $display
 *   The view $display object.
 *
 * @return $objs
 *   An associate array of objects to use for token replacement.
 */
function _custom_breadcrumbs_views_token_types($display) {
  // @todo Changes needed for D7?
  $objs = array();
  // Check to see if the current display has overriden the default arguments.
  $arguments = _custom_breadcrumbs_views_display_arguments($display);
  if (isset($arguments) && !empty($arguments)) {
    $viewargs = (isset($display->handler->view->args) && is_array($display->handler->view->args)) ? $display->handler->view->args : array();
    $arg_values = _custom_breadcrumbs_views_parse_args($arguments, $viewargs);
    foreach ($arg_values['types'] as $key => $type) {
      switch ($type) {
        case 'tid':
          $objs['taxonomy'] = taxonomy_term_load($arg_values['values'][$key]);
          break;
        case 'nid':
          $objs['node'] = node_load($arg_values['values'][$key]);
          break;
        case 'uid':
          $objs['user'] = user_load($arg_values['values'][$key]);
          break;
      }
    }
  }
  return $objs;
}

/**
 * Extracts the views display option arguments array from the display.
 *
 * @param $display
 *   The view $display object.
 *
 * @return $arguments
 *   The view display option arguments array.
 */
function _custom_breadcrumbs_views_display_arguments($display) {
  // @todo Changes for D7?
  $arguments = NULL;
  if (isset($display->handler->view->display[$display->id]->display_options['arguments'])) {
    $arguments =$display->handler->view->display[$display->id]->display_options['arguments'];
  }
  if (!isset($arguments) && isset($display->handler->view->display['default']->display_options['arguments'])) {
    $arguments = $display->handler->view->display['default']->display_options['arguments'];
  }
  return $arguments;
}

/**
 * Parses a comma or plus separated string of term IDs.
 *
 * @param $str_tids
 *   A string of term IDs, separated by plus or comma.
 *   comma (,) means AND
 *   plus (+) means OR
 *
 * @return an associative array with an operator key (either 'and'
 *   or 'or') and a tid key containing an array of the term ids.
 */
function custom_breadcrumbs_taxonomy_terms_parse_string($str_tids) {
  $terms = array(
    'operator' => '',
    'tids' => array(),
  );
  if (preg_match('/^([0-9]+[+ ])+[0-9]+$/', $str_tids)) {
    $terms['operator'] = 'or';
    // The '+' character in a query string may be parsed as ' '.
    $terms['tids'] = preg_split('/[+ ]/', $str_tids);
  }
  else if (preg_match('/^([0-9]+,)*[0-9]+$/', $str_tids)) {
    $terms['operator'] = 'and';
    $terms['tids'] = explode(',', $str_tids);
  }
  return $terms;
}
