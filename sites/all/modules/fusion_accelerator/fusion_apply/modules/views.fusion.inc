<?php

/**
 * @file
 * Provide skins handling for views.module
 */

/**
 * @defgroup fusion_apply_views_module views.module handlers
 *
 * @{
 */

/**
 * Implements hook_fusion_apply_config_info().
 */
function views_fusion_apply_config_info() {
  $data['views']['form']['fusion_apply_ui_form'] = array(
    'preprocess_hook_callback' => 'views_fusion_apply_preprocess_hook_callback',
    'title' => t('views style settings'),
    'collapsed' => FALSE,
  );
  $data['views']['preprocess']['views_view'] = array(
    'index_handler' => 'views_fusion_apply_preprocess_index_handler',
  );
  $data['views']['contextual_links']['page'] = array(
    'contextual_links_handler' => 'views_fusion_apply_contextual_links',
  );

  return $data;
}

/**
 * Fusion Apply form preprocess hook callback.
 *
 * @param &$form
 *   Passes in the $form parameter from hook_form_alter().
 * @param $form_state
 *   Passes in the $form_state parameter from hook_form_alter().
 *
 * @return
 *   An array of preprocess hooks we wish to use.
 */
function views_fusion_apply_preprocess_hook_callback(&$form, $form_state) {
  $preprocess_hooks = array('views_view');

  if (!empty($form_state['view']) && !empty($form_state['view']->name)) {
    $view = $form_state['view'];
  }
  elseif(isset($form['fusion_apply']['element']['#value'])) {
    list($element_info['view'], $element_info['display']) = explode('__', $form['fusion_apply']['element']['#value'], 2);
    if ($view = views_get_view($element_info['view'])) {
      $view->execute_display($element_info['display']);
    }
  }

  if (!empty($view)) {
    $display = $view->display[$view->current_display];

    // Create list of suggested templates.
    $preprocess_hooks = views_theme_functions('views_view', $view, $display);
    // Fetch additional style based suggested templates.
    $additional_hooks = views_theme_functions($display->display_plugin, $view, $display);

    $preprocess_hooks = array_merge($additional_hooks, $preprocess_hooks);
  }

  return $preprocess_hooks;
}

/**
 * Fusion Apply preprocess index handler.
 *
 * @param &$variables
 *   Passes in the $variables parameter from fusion_apply_preprocess().
 *
 * @return
 *   The index where we can find our values in Fusion Apply's data structure. If an
 *   array is returned, it will loop through each index in Fusion Apply's data
 *   structure and merge the returned classes. 
 */
function views_fusion_apply_preprocess_index_handler(&$variables) {
  return array($variables['view']->name .'__'. $variables['view']->current_display);
}

/**
 * Fusion Apply contextual links handler.
 */
function views_fusion_apply_contextual_links(&$variables) {
  $links = array();
  if (!empty($variables['page']['#views_contextual_links_info'])) {
    $links['fusion_apply-views'] = array(
      'admin/appearance/fusion/edit/nojs', array('views', $variables['page']['#views_contextual_links_info']['views_ui']['view_name'] . '__' . $variables['page']['#views_contextual_links_info']['views_ui']['view_display_id']),
    );
  }
  return $links;
}

/**
 * @}
 */
