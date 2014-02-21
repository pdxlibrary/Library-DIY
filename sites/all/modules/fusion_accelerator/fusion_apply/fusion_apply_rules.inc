<?php

/**
 * @defgroup skin page rule handlers
 * @{
 */

/**
 * Implementation of hook_fusion_apply_config_info().
 */
function fusion_apply_fusion_apply_config_info() {
  $data['rules']['preprocess']['html'] = array(
    'index_handler' => 'rules_fusion_apply_preprocess_index_handler',
  );
  $data['rules']['preprocess']['region'] = array(
    'index_handler' => 'rules_fusion_apply_preprocess_index_handler',
  );
  if (module_exists('fusion_apply_ui')) {
    $data['rules']['form']['fusion_apply_rule_edit'] = array(
      'index_handler' => 'rules_fusion_apply_form_index_handler',
      'preprocess_hook_callback' => 'rules_fusion_apply_preprocess_hook_callback',
      'title' => t('rule settings'),
      'fusion_apply_weight' => 0,
      'collapsed' => FALSE,
    );
    $data['rules']['form']['fusion_apply_ui_form'] = array(
      'preprocess_hook_callback' => 'rules_fusion_apply_preprocess_hook_callback',
      'title' => t('rule settings'),
      'collapsed' => FALSE,
    );  
    $data['rules']['contextual_links']['html'] = array(
      'contextual_links_handler' => 'rules_fusion_apply_contextual_links',
    );
    $data['rules']['contextual_links']['region'] = array(
      'contextual_links_handler' => 'rules_fusion_apply_contextual_links',
    );
  }
  return $data;
}


/**
 * Fusion Apply form index handler.
 *
 * @param $op
 *   What kind of action is being performed. Possible values:
 *   - 'form': the form elements for Fusion Apply are being inserted in a form.
 *   - 'submit': the form has been submitted.
 * @param &$form
 *   - For 'form', passes in the $form parameter from hook_form_alter().
 *   - For 'submit', passes in the $form parameter from hook_form_submit().
 * @param $form_state
 *   - For 'form', passes in the $form_state parameter from hook_form_alter().
 *   - For 'submit', passes in the $form_state parameter from hook_form_submit().
 * @return
 *   The index where we can find our values in Fusion Apply's data structure.
 */
function rules_fusion_apply_form_index_handler($op, &$form, $form_state) {
  switch ($op) {
    case 'form':
      if (!empty($form['rule']['rid']['#value'])) {
        return $form['rule']['rid']['#value'];
      }
      else {
        return 0;
      }

    case 'submit':
      return $form_state['values']['rid'];
  }
}

/**
 * Fusion Apply preprocess_hook_callback.
 *
 * @param &$form
 *   Passes in the $form parameter from hook_form_alter().
 * @param $form_state
 *   Passes in the $form_state parameter from hook_form_alter().
 * @return
 *   The preprocess_hook we wish to use.
 */
function rules_fusion_apply_preprocess_hook_callback(&$form, $form_state) {
  $preprocess_hooks = array();

  if (!empty($form['rule'])) {
    $hooks = explode('__', $form['rule']['rule_type']['#value']);
  }
  else {
    $rule = fusion_apply_rule_load($form['fusion_apply']['element']['#value']);
    $hooks = explode('__', $rule->rule_type);
  }
  while (count($hooks)) {
    $preprocess_hooks[] = implode('__', $hooks);
    array_pop($hooks);
  }

  return $preprocess_hooks;
}

/**
 * Fusion Apply preprocess index handler.
 *
 * @param &$variables
 *   Passes in the $variables parameter from module_preprocess().
 * @return
 *   The index where we can find our values in Fusion Apply's data structure. If an
 *   array is returned, it will loop through each index in Fusion Apply's data
 *   structure and merge the returned classes.
 */
function rules_fusion_apply_preprocess_index_handler(&$variables) {
  if (!empty($variables['region'])) {
    $rule_type = 'region__' . $variables['region'];
  }
  else {
    $rule_type = 'page';
  }
  $rules = fusion_apply_rule_load_multiple(array(), array('rule_type' => $rule_type));

  // Find any page level Fusion Apply options and return an array of them.
  $indices = array();
  foreach ($rules as $rule) {
    if (fusion_apply_rule_is_visible($rule->rid)) {
      $indices[] = $rule->rid;
    }
  }
  return $indices;
}

/**
 * Fusion Apply contextual links handler.
 *
 * @param &$variables
 *   Passes in the $variables parameter from fusion_apply_preprocess().
 * @return
 *   An associative array. Each value is an array that forms the function
 *   arguments for menu_contextual_links(). For example:
 *   @code
 *    $links = array(
 *      'fusion_apply-modulename' => array(
 *        'admin/appearance/fusion/edit', array('system', 'navigation')),
 *      ),
 *      'fusion_apply-modulename-1' => array(
 *        'admin/appearance/fusion/edit', array('system', 'something-else')),
 *      ),
 *    );
 *   @endcode
 */
function rules_fusion_apply_contextual_links(&$variables) {
  if (!empty($variables['region'])) {
    $rule_type = 'region__' . $variables['region'];
  }
  else {
    $rule_type = 'page';
  }
  $rules = fusion_apply_rule_load_multiple(array(), array('rule_type' => $rule_type));
  $links = array();
  $counter = 1;

  foreach ($rules as $rule) {
    if (fusion_apply_rule_is_visible($rule->rid)) {
      $links['fusion_apply-rule-' . $counter++] = array(
        'admin/config/fusion/edit/nojs', array('rule', $rule->rid),
      );
    }
  }
  return $links;
}

/**
 * @}
 */
