<?php
/**
 * @file
 * Provide skins handling for block.module
 */

/**
 * @defgroup fusion_apply_block_module block.module handlers
 *
 * @{
 */

/**
 * Implements hook_fusion_apply_config_info().
 */
function block_fusion_apply_config_info() {
  $data['block']['form']['fusion_apply_ui_form'] = array(
    'preprocess_hook_callback' => 'block_fusion_apply_preprocess_hook_callback',
    'title' => t('block settings'),
    'collapsed' => FALSE,
  );
  $data['block']['preprocess']['block'] = array(
    'index_handler' => 'block_fusion_apply_preprocess_index_handler',
  );
  $data['block']['contextual_links']['block'] = array(
    'contextual_links_handler' => 'block_fusion_apply_contextual_links',
  );

  return $data;
}

/**
 * Fusion Apply preprocess hook callback.
 *
 * @param &$form
 *   Passes in the $form parameter from hook_form_alter().
 * @param $form_state
 *   Passes in the $form_state parameter from hook_form_alter().
 *
 * @return
 *   An array of preprocess hooks we wish to use.
 */
function block_fusion_apply_preprocess_hook_callback(&$form, $form_state) {
  $preprocess_hooks = array();

  if (empty($form['module']['#value']) && !empty($form['fusion_apply']['element']['#value'])) {
    list($module, $delta) = explode('__', $form['fusion_apply']['element']['#value'], 2);

    $result = db_select('block', 'b')
      ->fields('b')
      ->distinct()
      ->condition('b.module', $module)
      ->condition('b.delta', $delta)
      ->range(0, 1)
      ->execute();
    foreach ($result as $block) {
      $preprocess_hooks[] = 'block_'. $block->module;
    }
  }
  else {
    $preprocess_hooks[] = 'block_'. $form['module']['#value'];
  }
  $preprocess_hooks[] = 'block';

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
function block_fusion_apply_preprocess_index_handler(&$variables) {
  return array($variables['block']->module . '__' . $variables['block']->delta);
}

/**
 * Fusion Apply contextual links handler.
 *
 * @param &$variables
 *   Passes in the $variables parameter from fusion_apply_preprocess().
 *
 * @return
 *   An array. Each value is an array that forms the function arguments for
 *   menu_contextual_links(). For example:
 *   @code
 *     array(
 *       'admin/appearance/fusion/edit', array('system', 'navigation')),
 *     )
 *   @endcode
 */
function block_fusion_apply_contextual_links(&$variables) {
  $links = array();
  $links['fusion_apply-block'] = array(
    'admin/appearance/fusion/edit/nojs', array('block', $variables['block']->module . '__' . $variables['block']->delta),
  );
  return $links;
}

/**
 * @}
 */
