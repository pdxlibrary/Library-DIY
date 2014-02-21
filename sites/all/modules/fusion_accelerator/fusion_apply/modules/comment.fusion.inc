<?php
/**
 * @file
 * Provide skins handling for comment.module
 */

/**
 * @defgroup fusion_apply_comment_module comment.module handlers
 *
 * @{
 */

/**
 * Implements hook_fusion_apply_config_info().
 */
function comment_fusion_apply_config_info() {
  $data['comment']['form']['fusion_apply_ui_form'] = array(
    'preprocess_hook_callback' => 'comment_fusion_apply_preprocess_hook_callback',
    'title' => t('comment settings'),
    'fusion_apply_weight' => 2,
    'collapsed' => FALSE,
  );
  $data['comment']['preprocess']['comment_wrapper'] = array(
    'index_handler' => 'comment_fusion_apply_preprocess_index_handler',
  );
  $data['comment']['contextual_links']['comment_wrapper'] = array(
    'contextual_links_handler' => 'comment_fusion_apply_contextual_links',
  );

  return $data;
}

/**
 * Fusion Apply form index handler.
 *
 * @param $op
 *   What kind of action is being performed. Possible values:
 *   - 'form': the form elements for skin are being inserted in a form.
 *   - 'submit': the form has been submitted.
 * @param &$form
 *   - For 'form', passes in the $form parameter from hook_form_alter().
 *   - For 'submit', passes in the $form parameter from hook_form_submit().
 * @param $form_state
 *   - For 'form', passes in the $form_state parameter from hook_form_alter().
 *   - For 'submit', passes in the $form_state parameter from hook_form_submit().
 *
 * @return
 *   The index where we can find our values in Fusion Apply's data structure.
 */
function comment_fusion_apply_form_index_handler($op, &$form, &$form_state) {
  switch ($op) {
    case 'form':
      return $form['#node_type']->type;

    case 'submit':
      // Clear old variable before we set a new one if the node type has changed
      if ($form_state['values']['old_type'] != $form_state['values']['type']) {
        foreach ($form_state['values']['fusion_apply_settings']['comment_group'] as $theme_name => $theme_data) {
          $fusion_apply = new stdClass();
          $fusion_apply->theme = $theme_name;
          $fusion_apply->module = 'comment';
          $fusion_apply->element = $form_state['values']['old_type'];
          $fusion_apply->skins = array();

          fusion_apply_skin_save($fusion_apply);
        }
      }
    return $form_state['values']['type'];
  }
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
function comment_fusion_apply_preprocess_hook_callback(&$form, $form_state) {
  $preprocess_hooks = array();

  if (!isset($form['#node_type']->type) && !empty($form['fusion_apply']['element']['#value'])) {
    $preprocess_hooks[] = 'comment_wrapper_' . $form['fusion_apply']['element']['#value'];
  }
  else {
    $preprocess_hooks[] = 'comment_wrapper_' . $form['#node_type']->type;
  }
  $preprocess_hooks[] = 'comment_wrapper';

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
function comment_fusion_apply_preprocess_index_handler(&$variables) {
  return array($variables['node']->type);
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
function comment_fusion_apply_contextual_links(&$variables) {
  $links = array();
  $links['fusion_apply-comment'] = array(
    'admin/appearance/fusion/edit/nojs', array('comment', $variables['node']->type),
  );
  return $links;
}

/**
 * @}
 */
