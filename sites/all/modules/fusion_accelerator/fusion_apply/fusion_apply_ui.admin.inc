<?php

/**
 * @file
 * Admin page callbacks for the Fusion Apply module.
 */

/**
 * Menu callback; Updates the skin configuration's status.
 */
function fusion_apply_ui_skin_status_set($skin, $status) {
  // We require a token in the query string to prevent CSFR attacks.
  if (!isset($_GET['token']) || !drupal_valid_token($_GET['token'], 'admin/appearance/fusion/' . ($status ? 'enable' : 'disable') . '/' . $skin->sid)) {
    return MENU_ACCESS_DENIED;
  }

  // Load an uncached version of the skin configuration object.
  $skin = fusion_apply_skin_load_unchanged($skin->sid);
  // Let's save some time in fusion_apply_skin_save() by setting $skin->original here.
  $skin->original = clone($skin);

  // Update the status and save the skin configuration.
  $skin->status = $status ? 1 : 0;
  fusion_apply_skin_save($skin);
  if ($status) {
    drupal_set_message(t('Skin configuration has been enabled.'));
  }
  else {
    drupal_set_message(t('Skin configuration has been disabled.'));
  }

  // Return to the skin configuration overview page.
  drupal_goto('admin/appearance/fusion/skins');
}

/**
 * Implements hook_fusion_apply_ui_operations().
 */
function fusion_apply_ui_fusion_apply_ui_operations() {
  $operations = array(
    'enable' => array(
      'label' => t('Enable selected skin configuration'),
      'callback' => 'fusion_apply_ui_mass_update',
      'callback arguments' => array('updates' => array('status' => 1)),
    ),
    'disable' => array(
      'label' => t('Disable selected skin configuration'),
      'callback' => 'fusion_apply_ui_mass_update',
      'callback arguments' => array('updates' => array('status' => 0)),
    ),
    'delete' => array(
      'label' => t('Delete selected skin configuration'),
      'callback' => NULL,
    ),
  );
  return $operations;
}

/**
 * List skin administration filters that can be applied.
 *
 * @return
 *   An array of filters.
 */
function fusion_apply_ui_filters() {
  // Theme filter.
  $themes = list_themes();
  ksort($themes);

  $options = array('[any]' => t('any'));
  foreach ($themes as $theme) {
    if (!$theme->status) {
      continue;
    }
    $options[$theme->name] = $theme->info['name'];
  }

  $filters['theme'] = array(
    'title' => t('theme'),
    'options' => $options,
  );

  // Type filter.
  $config = fusion_apply_get_config_info();

  $options = array('[any]' => t('any'));
  foreach ($config as $type => $data) {
    $options[$type] = $type;
  }

  $filters['module'] = array(
    'title' => t('type'),
    'options' => $options,
  );

  // Skin filter.
  $skin_info = fusion_apply_get_skin_info();

  $options = array('[any]' => t('any'), '_additional' => t('Additional classes'));
  foreach ($skin_info as $skin => $info) {
    $options[$skin] = $info['title'];
  }

  $filters['skin'] = array(
    'title' => t('skin'),
    'options' => $options,
  );

  // Status filter.
  $filters['status'] = array(
    'title' => t('status'),
    'options' => array(
      '[any]' => t('any'),
      '1' => t('enabled'),
      '0' => t('disabled'),
    ),
  );

  return $filters;
}

/**
 * Apply filters for skin configuration administration filters based on session.
 *
 * @param $query
 *   A SelectQuery to which the filters should be applied.
 */
function fusion_apply_ui_build_filter_query(SelectQueryInterface $query) {
  // Build query
  $filter_data = isset($_SESSION['fusion_apply_ui_overview_filter']) ? $_SESSION['fusion_apply_ui_overview_filter'] : array();
  foreach ($filter_data as $index => $filter) {
    list($key, $value) = $filter;
    $query->condition('s.' . $key, $value);
  }
}

/**
 * Form builder for the Fusion Apply administration filters form.
 *
 * @ingroup forms
 */
function fusion_apply_ui_filter_form() {
  $session = &$_SESSION['fusion_apply_ui_overview_filter'];
  $session = is_array($session) ? $session : array();
  $filters = fusion_apply_ui_filters();

  $i = 0;
  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Show only items where'),
    '#theme' => 'exposed_filters__fusion_apply',
  );
  foreach ($session as $filter) {
    list($type, $value) = $filter;
    $value = $filters[$type]['options'][$value];
    $t_args = array('%property' => $filters[$type]['title'], '%value' => $value);

    if ($i++) {
      $form['filters']['current'][] = array('#markup' => t('and where %property is %value', $t_args));
    }
    else {
      $form['filters']['current'][] = array('#markup' => t('where %property is %value', $t_args));
    }
    // Remove the option if it is already being filtered on.
    unset($filters[$type]);
  }

  $form['filters']['status'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('clearfix')),
    '#prefix' => ($i ? '<div class="additional-filters">' . t('and where') . '</div>' : ''),
  );
  $form['filters']['status']['filters'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('filters')),
   );

  foreach ($filters as $key => $filter) {
    $names[$key] = $filter['title'];
    $form['filters']['status']['filters'][$key] = array(
      '#type' => 'select',
      '#title' => $filter['title'],
      '#options' => $filter['options'],
      '#default_value' => '[any]',
    );
  }

  $form['filters']['status']['actions'] = array(
    '#type' => 'actions',
    '#id' => 'fusion_apply-exposed-filters',
    '#attributes' => array('class' => array('container-inline')),
  );
  if (count($filters)) {
    $form['filters']['status']['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => count($session) ? t('Refine') : t('Filter'),
    );
  }
  if (count($session)) {
    $form['filters']['status']['actions']['undo'] = array('#type' => 'submit', '#value' => t('Undo'));
    $form['filters']['status']['actions']['reset'] = array('#type' => 'submit', '#value' => t('Reset'));
  }

  drupal_add_js('misc/form.js');

  return $form;
}

/**
 * Mass update skin configurations, updating all skin configurations in the
 * $skins array with the field values in $updates.
 *
 * IMPORTANT NOTE: This function is intended to work when called
 * from a form submit handler. Calling it outside of the form submission
 * process may not work correctly.
 *
 * @param array $skins
 *   Array of skin configuration sids to update.
 * @param array $updates
 *   Array of key/value pairs with skin configuration field names and the
 *   value to update that field to.
 */
function fusion_apply_ui_mass_update($skins, $updates) {
  // We use batch processing to prevent timeout when updating a large number
  // of skins.
  if (count($skins) > 10) {
    $batch = array(
      'operations' => array(
        array('_fusion_apply_ui_mass_update_batch_process', array($skins, $updates))
      ),
      'finished' => '_fusion_apply_ui_mass_update_batch_finished',
      'title' => t('Processing'),
      // We use a single multi-pass operation, so the default
      // 'Remaining x of y operations' message will be confusing here.
      'progress_message' => '',
      'error_message' => t('The update has encountered an error.'),
      // The operations do not live in the .module file, so we need to
      // tell the batch engine which file to load before calling them.
      'file' => drupal_get_path('module', 'fusion_apply_ui') . '/fusion_apply_ui.admin.inc',
    );
    batch_set($batch);
  }
  else {
    foreach ($skins as $sid) {
      _fusion_apply_ui_mass_update_helper($sid, $updates);
    }
    drupal_set_message(t('The update has been performed.'));
  }
}

/**
 * Helper function for skin configuration mass updates.
 */
function _fusion_apply_ui_mass_update_helper($sid, $updates) {
  drupal_static_reset('fusion_apply_skin_load_multiple');
  $skin = fusion_apply_skin_load($sid);
  // For efficiency manually store the original skin configuration before
  // applying any changes.
  $skin->original = clone $skin;
  foreach ($updates as $name => $value) {
    $skin->$name = $value;
  }
  fusion_apply_skin_save($skin);
  return $skin;
}

/**
 * Batch operation for skin configuration mass updates.
 */
function _fusion_apply_ui_mass_update_batch_process($skins, $updates, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = count($skins);
    $context['sandbox']['skins'] = $skins;
  }

  // Process skins in groups of 5.
  $count = min(5, count($context['sandbox']['skins']));
  for ($i = 1; $i <= $count; $i++) {
    // For each sid, load the skin configuration, reset the values, and save it.
    $sid = array_shift($context['sandbox']['skins']);
    $skin = _fusion_apply_ui_mass_update_helper($sid, $updates);

    // Store result for post-processing in the finished callback.
    // @todo
    $context['results'][] = l($skin->title, 'node/' . $skin->sid);

    // Update our progress information.
    $context['sandbox']['progress']++;
  }

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Batch 'finished' callback for skin configuration mass updates.
 */
function _fusion_apply_ui_mass_update_batch_finished($success, $results, $operations) {
  if ($success) {
    drupal_set_message(t('The update has been performed.'));
  }
  else {
    drupal_set_message(t('An error occurred and processing did not complete.'), 'error');
    $message = format_plural(count($results), '1 item successfully processed:', '@count items successfully processed:');
    $message .= theme('item_list', array('items' => $results));
    drupal_set_message($message);
  }
}

/**
 * Form submission handler for fusion_apply_ui_filter_form().
 */
function fusion_apply_ui_filter_form_submit($form, &$form_state) {
  $filters = fusion_apply_ui_filters();
  switch ($form_state['values']['op']) {
    case t('Filter'):
    case t('Refine'):
      // Apply every filter that has a choice selected other than 'any'.
      foreach ($filters as $filter => $options) {
        if (isset($form_state['values'][$filter]) && $form_state['values'][$filter] != '[any]') {
          // Flatten the options array to accommodate hierarchical/nested options.
          $flat_options = form_options_flatten($filters[$filter]['options']);
          // Only accept valid selections offered on the dropdown, block bad input.
          if (isset($flat_options[$form_state['values'][$filter]])) {
            $_SESSION['fusion_apply_ui_overview_filter'][] = array($filter, $form_state['values'][$filter]);
          }
        }
      }
      break;
    case t('Undo'):
      array_pop($_SESSION['fusion_apply_ui_overview_filter']);
      break;
    case t('Reset'):
      $_SESSION['fusion_apply_ui_overview_filter'] = array();
      break;
  }
}

/**
 * Menu callback: skin configurations administration.
 *
 * @ingroup forms
 */
function fusion_apply_ui_list($form, &$form_state) {
  if (isset($form_state['values']['operation']) && $form_state['values']['operation'] == 'delete') {
    return fusion_apply_ui_multiple_delete_confirm($form, $form_state, array_filter($form_state['values']['skins']));
  }
  $form['filter'] = fusion_apply_ui_filter_form();
  $form['#submit'][] = 'fusion_apply_ui_filter_form_submit';
  $form['admin'] = fusion_apply_ui_admin_skins();

  return $form;
}

/**
 * Form builder: Builds the skin configuration administration overview.
 *
 * @ingroup forms
 */
function fusion_apply_ui_admin_skins() {
  // Build the 'Update options' form.
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options = array();
  foreach (module_invoke_all('fusion_apply_ui_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $options,
    '#default_value' => 'enable',
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#validate' => array('fusion_apply_ui_admin_skins_validate'),
    '#submit' => array('fusion_apply_ui_admin_skins_submit'),
  );

  $header = array(
    'theme' => array('data' => t('Theme'), 'field' => 's.theme'),
    'type' => array('data' => t('Type'), 'field' => 's.module'),
    'element' => array('data' => t('Element ID'), 'field' => 's.element'),
    'skin' => array('data' => t('Skin'), 'field' => 's.skin'),
    'status' => array('data' => t('Status'), 'field' => 's.status'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('fusion_apply_skins', 's')->extend('PagerDefault')->extend('TableSort');
  fusion_apply_ui_build_filter_query($query);

  $sids = $query
    ->fields('s', array('sid'))
    ->limit(50)
    ->orderByHeader($header)
    ->execute()
    ->fetchCol();
  $skins = fusion_apply_skin_load_multiple($sids);

  $themes = list_themes();
  $skin_info = fusion_apply_get_skin_info();
  $destination = drupal_get_destination();
  $options = array();
  foreach ($skins as $skin) {
    $operations = array(
      'edit' => array(
        'title' => t('edit'),
        'href' => 'admin/appearance/fusion/edit/nojs/' . $skin->module . '/' . $skin->element,
        'query' => $destination,
      ),
      'status' => array(
        'title' => $skin->status ? t('disable') : t('enable'),
        'href' => 'admin/appearance/fusion/' . ($skin->status ? 'disable' : 'enable') . '/' . $skin->sid,
        'query' => $destination + array(
          'token' => drupal_get_token('admin/appearance/fusion/' . ($skin->status ? 'disable' : 'enable') . '/' . $skin->sid),
        ),
      ),
      'delete' => array(
        'title' => t('delete'),
        'href' => 'admin/appearance/fusion/delete/'. $skin->sid,
        'query' => $destination,
      ),
    );
    $options[$skin->sid] = array(
      'theme' => isset($themes[$skin->theme]) ? $themes[$skin->theme]->info['name'] : '<em>' . $skin->theme . '</em>',
      'type' => $skin->module,
      'element' => $skin->element,
      'skin' => $skin->skin == '_additional' ? '<em>' . t('Additional classes') . '</em>' : (isset($skin_info[$skin->skin]) ? $skin_info[$skin->skin]['title'] : '<em>' . $skin->skin . '</em>'),
      'status' => $skin->status ? t('enabled') : t('disabled'),
      'operations' => array(
        'data' => array(
          '#theme' => 'links__fusion_apply_ui_operations',
          '#links' => $operations,
          '#attributes' => array('class' => array('links', 'inline')),
        ),
      ),
    );
  }

  $form['skins'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No content available.'),
  );

  $form['pager'] = array('#markup' => theme('pager'));
  return $form;
}

/**
 * Form validation handler for fusion_apply_ui_list().
 *
 * Check if any skin settings have been selected to perform the chosen
 * 'Update option' on.
 */
function fusion_apply_ui_admin_skins_validate($form, &$form_state) {
  // Error if there are no items to select.
  if (!is_array($form_state['values']['skins']) || !count(array_filter($form_state['values']['skins']))) {
    form_set_error('', t('No items selected.'));
  }
}

/**
 * Form submission handler for fusion_apply_ui_list().
 *
 * Execute the chosen 'Update option' on the selected skin settings.
 */
function fusion_apply_ui_admin_skins_submit($form, &$form_state) {
  $operations = module_invoke_all('fusion_apply_ui_operations');
  $operation = $operations[$form_state['values']['operation']];
  // Filter out unchecked nodes
  $nodes = array_filter($form_state['values']['skins']);
  if ($function = $operation['callback']) {
    // Add in callback arguments if present.
    if (isset($operation['callback arguments'])) {
      $args = array_merge(array($nodes), $operation['callback arguments']);
    }
    else {
      $args = array($nodes);
    }
    call_user_func_array($function, $args);

    cache_clear_all();
  }
  else {
    // We need to rebuild the form to go to a second step. For example, to
    // show the confirmation form for the deletion of nodes.
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Form builder for the confirmation form when deleting multiple skin settings.
 *
 * @param $skins
 *   An array of skins to delete.
 *
 * @ingroup forms
 */
function fusion_apply_ui_multiple_delete_confirm($form, &$form_state, $skins) {
  $themes = list_themes();

  $form['skins'] = array('#prefix' => '<ul>', '#suffix' => '</ul>', '#tree' => TRUE);
  // array_filter returns only elements with TRUE values
  $original_skins = fusion_apply_skin_load_multiple(array_keys($skins));
  foreach ($skins as $sid => $value) {
    $form['skins'][$sid] = array(
      '#type' => 'hidden',
      '#value' => $sid,
      '#prefix' => '<li>',
      '#suffix' => t('Skin %skin for element %element of type %type for the %theme theme', array('%skin' => $original_skins[$sid]->skin, '%element' => $original_skins[$sid]->element, '%type' => $original_skins[$sid]->module, '%theme' => $themes[$original_skins[$sid]->theme]->info['name'])) ."</li>\n",
    );
  }
  $form['operation'] = array('#type' => 'hidden', '#value' => 'delete');
  $form['#submit'][] = 'fusion_apply_ui_multiple_delete_confirm_submit';
  $confirm_question = format_plural(count($skins),
                                  'Are you sure you want to delete this item?',
                                  'Are you sure you want to delete these items?');
  return confirm_form($form,
                    $confirm_question,
                    'admin/appearance/fusion', t('This action cannot be undone.'),
                    t('Delete'), t('Cancel'));
}

/**
 * Form submission handler for fusion_apply_ui_multiple_delete_confirm().
 */
function fusion_apply_ui_multiple_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    fusion_apply_skin_delete_multiple(array_keys($form_state['values']['skins']));
    $count = count($form_state['values']['skins']);
    if ($count == 1) {
      watchdog('content', 'Deleted 1 skin configuration.');
    }
    else {
      watchdog('content', 'Deleted @count skin configurations.', array('@count' => $count));
    }
    drupal_set_message(format_plural($count, 'Deleted 1 skin configuration.', 'Deleted @count skin configurations.'));
  }
  $form_state['redirect'] = 'admin/appearance/fusion';
}

/**
 * Menu callback: skins administration.
 */
function fusion_apply_ui_admin_skin_infos($form, $form_state) {
  $skins = fusion_apply_get_skin_info();
  if (empty($skins)) {
    $form['skins_empty'] = array(
      '#markup' => t('You do not currently have any skin sets to manage.'),
    );
    return $form;
  }

  $groups = fusion_apply_get_group_info();

  uasort($skins, 'fusion_apply_ui_sort_by_title');
  $form['skins'] = array('#tree' => TRUE);

  // Iterate through each of the skins.
  foreach ($skins as $name => $skin) {
    $extra = array();

    // Generate link for skin's configuration page
    $extra['links']['configure'] = array(
      '#type' => 'link',
      '#title' => t('Configure'),
      '#href' => 'admin/appearance/fusion/skins/settings/' . $name,
      '#options' => array('attributes' => array('class' => array('skin-link', 'skin-link-configure'))),
    );

    // Create a row entry for this skin.
    $group = $groups[$skin['group']]['title'];
    $form['skins'][$group][$name] = _fusion_apply_ui_admin_skin_infos_build_row($skin, $extra);
  }

  // Add basic information to the fieldsets.
  foreach (element_children($form['skins']) as $group) {
    $form['skins'][$group] += array(
      '#type' => 'fieldset',
      '#title' => $group,
      '#collapsible' => TRUE,
      '#theme' => 'fusion_apply_ui_admin_skin_infos_fieldset',
      '#header' => array(
        // array('data' => t('Enabled'), 'class' => array('checkbox')),
        t('Skin Name'),
        t('Source'),
        t('Hooks'),
        array('data' => t('Settings')),
      ),
    );
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['#action'] = url('admin/appearance/fusion/skins');

  return $form;
}

/**
 * Build a table row for the skin info listing page.
 */
function _fusion_apply_ui_admin_skin_infos_build_row($skin, $extra) {
  // Add in the defaults.
  $extra += array(
    'enabled' => FALSE,
    'disabled' => FALSE,
    'links' => array(),
  );
  $form = array(
    '#tree' => TRUE,
  );

  $form['name'] = array(
    '#markup' => $skin['title'],
  );
  $form['description'] = array(
    '#markup' => t($skin['description']),
  );

  // Grab source info.
  $info = system_get_info($skin['source']['type'], $skin['source']['name']);

  $form['source name'] = array(
    '#markup' => $skin['source']['name'],
  );

  $theme_hooks = array();
  foreach ($skin['theme hooks'] as $theme_hook) {
    $theme_hooks[] = $theme_hook == '*' ? t('all hooks') : $theme_hook;
  }
  $form['theme hooks'] = array(
    '#markup' => implode('<br />', $theme_hooks),
  );

  // $form['enable'] = array(
  //   '#type' => 'checkbox',
  //   '#title' => t('Enable'),
  //   '#default_value' => $extra['enabled'],
  // );
  // if ($extra['disabled']) {
  //   $form['enable']['#disabled'] = TRUE;
  // }

  // Build operation links.
  $form['links']['configure'] = (isset($extra['links']['configure']) ? $extra['links']['configure'] : array());

  return $form;
}

/**
 * Sort skin infos by title.
 *
 * @param $a
 *   The first skin info to compare.
 * @param $b
 *   The second skin info to compare.
 *
 * @return
 *   Returns < 0 if $a is less than $b; > 0 if $a is greater than $b, and 0
 *   if they are equal.
 */
function fusion_apply_ui_sort_by_title($a, $b) {
  return strcasecmp($a['title'], $b['title']);
}

/**
 * Returns HTML for the skin info listing form.
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @ingroup themeable
 */
function theme_fusion_apply_ui_admin_skin_infos_fieldset($variables) {
  $form = $variables['form'];

  // Individual table headers.
  $rows = array();
  
  // Iterate through all the plugins.
  foreach (element_children($form) as $skin_name) {
    // Stick it into $skinset for easier accessing.
    $skin_info = $form[$skin_name];

    $columns = array();
    
    // Enabled.
    // unset($skin_info['enable']['#title']);
    // $columns[] = array(
    //   'class' => array('checkbox'), 
    //   'data' => drupal_render($skin_info['enable'])
    // );    

    // Skin name.
    $columns[] = theme('fusion_apply_ui_admin_skin_infos_summary', array(
      'name' => drupal_render($skin_info['name']),
      'description' => drupal_render($skin_info['description']),
    ));

    // Source.
    $columns[] = drupal_render($skin_info['source name']);

    // Theme hooks.
    $columns[] = drupal_render($skin_info['theme hooks']);

    // Settings.
    $columns[] = array(
      'class' => array('configure'),
      'data' => drupal_render($skin_info['links']['configure']),
    );

    $rows[] = $columns;
  }

  return theme('table', array('header' => $form['#header'], 'rows' => $rows));
}


/**
 * Returns HTML for a message about incompatible skinsets.
 *
 * @param $variables
 *   An associative array containing:
 *   - message: The form array representing the currently disabled modules.
 *
 * @ingroup themeable
 */
function theme_fusion_apply_ui_admin_skin_infos_summary($variables) {
  return '<div class="skin-infos-summary">'. $variables['name'] .'<div class="description">'. $variables['description'] .'</div></div>';
}

/**
 * Returns HTML for a definition list.
 *
 * @param $variables
 *   An associative array containing:
 *   - definitions: An array of definition descriptions, keyed by definition terms.
 *   - attribute: An array of HTML attributes to apply to the dl tag.
 *
 * @ingroup themeable
 */
function theme_fusion_apply_ui_admin_dl($variables) {
  $definitions = $variables['definitions'];
  $attributes = $variables['attributes'];

  $output = '<dl' . drupal_attributes($attributes) . ">\n";
  foreach ($definitions as $term => $description) {
    $output .= '<dt>' . $term . "</dt>\n";
    $output .= '<dd>' . $description . "</dd>\n";
  }
  $output .= "</dl>\n";

  return $output;
}

/**
 * Menu callback; displays a listing of all skins in a skinset, allowing you
 * to enable or disable them individually for each theme.
 *
 * @ingroup forms
 * @see fusion_apply_ui_admin_skin_info_settings_validate()
 * @see fusion_apply_ui_admin_skin_info_settings_submit()
 * @see fusion_apply_ui_admin_skin_info_settings_reset()
 */
function fusion_apply_ui_admin_skin_info_settings($form, &$form_state, $skin_info) {
  $themes = list_themes();
  if (isset($form_state['storage']['disable_sids'])) {
    foreach ($form_state['storage']['themes'] as $theme) {
      $items[] = $themes[$theme]->info['name'];
    }
    $message = t('Would you like to disable the skin configurations for the selected themes?') . theme('item_list', array('items' => $items));
    // Insert a confirmation form.
    return confirm_form(
      $form,
      t('Disable all skin configurations for selected themes?'),
      'admin/appearance/fusion/skins',
      $message,
      t('Yes'),
      t('No')
    );
  }

  // Output skin details.
  $form['title'] = array(
    '#markup' => '<h2>' . $skin_info['title'] . '</h2>',
  );
  $form['description'] = array(
    '#markup' => '<div class="description">' . $skin_info['description'] . '</div>',
  );

  $theme_hooks = array();
  foreach ($skin_info['theme hooks'] as $theme_hook) {
    $theme_hooks[] = $theme_hook == '*' ? t('all hooks') : $theme_hook;
  }

  $modules = system_rebuild_module_data();
  $source = !empty($modules[$skin_info['source']['name']]->info['name']) ? $modules[$skin_info['source']['name']]->info['name'] : $skin_info['source']['name'];

  $groups = fusion_apply_get_group_info();
  $group = !empty($groups[$skin_info['group']]['title']) ? $groups[$skin_info['group']]['title'] : $skin_info['group'];

  $details = array(
    t('Source') => t('%source !type', array('%source' => $source, '!type' => $skin_info['source']['type'] == 'module' ? t('module') : t('theme'))),
    t('Theme hooks') => implode(', ', $theme_hooks),
    t('Group') => $group,
  );

  $form['details'] = array(
    '#markup' => theme('fusion_apply_ui_admin_dl', array('definitions' => $details, 'attributes' => array('class' => 'details'))),
  );

  $form['status'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Themes'),
    '#description' => t('This skin will be availble as an option for the selected themes.'),
  );

  $options = array();
  $default_value = array();
  $status = fusion_apply_skin_info_status_get($skin_info);
  ksort($status);
  foreach ($status as $theme => $enabled) {
    if (isset($themes[$theme]) && $themes[$theme]->status) {
      $options[$theme] = $themes[$theme]->info['name'];
    }
    else {
      // Hidden field to keep contents of themes that aren't enabled, or that
      // don't exist on this site but are set by the skin plugin.
      $form['status'][$theme] = array(
        '#type' => 'value',
        '#value' => $theme,
      );
    }
    if ($enabled) {
      $default_value[] = $theme;
    }
  }
  $form['status']['#options'] = $options;
  $form['status']['#default_value'] = $default_value;

  $form['skin'] = array(
    '#type' => 'hidden',
    '#value' => $skin_info['name'],
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['actions']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
    '#submit' => array('fusion_apply_ui_admin_skin_info_settings_reset'),
  );

  // Store $skin_info for use in submit function.
  $form_state['#skin_info'] = $skin_info;

  return $form;
}

/**
 * Process fusion_apply_ui_admin_skin_info_settings() form submissions.
 *
 * Reset status for this skin to default.
 */
function fusion_apply_ui_admin_skin_info_settings_reset($form, &$form_state) {
  variable_del('fusion_apply_skin_' . $form_state['values']['skin'] . '_status');

  drupal_set_message(t("%skin's statuses have been reset to their defaults.", array('%skin' => $form_state['values']['skin'])));
  $form_state['redirect'] = 'admin/appearance/fusion/skins';
}

/**
 * Helper function for fusion_apply_ui_admin_skin_info_settings_submit().
 *
 * Replace all values with 1 if content exists, or 0 otherwise.
 */
function _fusion_apply_ui_admin_bool($val) {
  return $val ? 1 : 0;
}

/**
 * Process fusion_apply_ui_admin_skin_info_settings() form submissions.
 */
function fusion_apply_ui_admin_skin_info_settings_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#id'] == 'edit-save') {
    // Make sure we don't disable skins for which configuration exists. Ask to
    // disable all related skin configurations so we can disable the skin.

    $themes = array();
    $status = fusion_apply_skin_info_status_get($form_state['#skin_info']);
    foreach ($form_state['values']['status'] as $theme => $enabled) {
      $default_enabled = isset($status[$theme]) ? $status[$theme] : 0;
      if ($default_enabled && !$enabled) {
        $themes[] = $theme;
      }
    }

    if (count($themes)) {
      $theme_info = list_themes();
      $disable_sids = array();
      $rebuild = FALSE;

      foreach ($themes as $theme)  {
        $params = array(
          'theme' => $theme,
          'skin' => $form_state['values']['skin'],
          'status' => 1,
        );
        $sids = fusion_apply_skin_get_sids($params);
        if (count($sids)) {
          $form_state['storage']['themes'][] = $theme;
          $disable_sids += $sids;
          $rebuild = TRUE;
        }
      }
      if ($rebuild) {
        $form_state['storage']['skin'] = $form_state['values']['skin'];
        $form_state['storage']['status'] = $form_state['values']['status'];
        $form_state['storage']['disable_sids'] = $disable_sids;
        $form_state['rebuild'] = TRUE;
        return;
      }
    }
  }

  if (isset($form_state['storage']['disable_sids']) && count($form_state['storage']['disable_sids'])) {
    $skin = $form_state['storage']['skin'];
    $status = array_map('_fusion_apply_ui_admin_bool', $form_state['storage']['status']);

    // Disable any skin configurations for skins that are being disabled.
    db_update('fusion_apply_skins')
      ->fields(array('status' => 0))
      ->condition('sid', $form_state['storage']['disable_sids'])
      ->execute();
    // Clear the fusion_apply_skin_load_multiple cache.
    drupal_static_reset('fusion_apply_skin_load_multiple');

    $theme_info = list_themes();
    foreach ($form_state['storage']['themes'] as $theme) {
      drupal_set_message(t('All enabled skin configurations for skin %skin and theme %theme have been disabled.', array('%skin' => $skin, '%theme' => isset($theme_info[$theme]->info['name']) ? $theme_info[$theme]->info['name'] : $theme)));
    }
  }
  else {
    $skin = $form_state['values']['skin'];
    $status = array_map('_fusion_apply_ui_admin_bool', $form_state['values']['status']);
  }

  // Save the new status.
  fusion_apply_skin_info_status_set($form_state['#skin_info'], $status);

  drupal_set_message(t("Statuses for skin %skin have been updated.", array('%skin' => $skin)));
  $form_state['redirect'] = 'admin/appearance/fusion/skins';
}

/**
 * Form builder for the skin settings export form.
 *
 * @param $theme
 *   (optional) The name of the theme to export skin settings for. If no
 *   theme name is provided a theme selector is shown.
 *
 * @ingroup forms
 */
function fusion_apply_ui_export_form($form, &$form_state, $theme = NULL) {
  $form = array();
  $themes = list_themes();

  if ($theme) {
    // Export an individual theme.
    $theme = str_replace('-', '_', $theme);
    $params = array(
      'theme' => $theme,
    );
    $skins = fusion_apply_skin_load_multiple(fusion_apply_skin_get_sids($params));

    // Convert classes to arrays.
    $code = '';
    foreach ($skins as $skin) {
      unset($skin->sid);
      unset($skin->rdf_mapping);
      $code .= '$skins[] = '. var_export((array) $skin, TRUE) .";\n";
    }

    $lines = substr_count($code, "\n") + 1;

    $form['theme'] = array(
      '#type' => 'textfield',
      '#title' => t('Theme'),
      '#value' => $themes[$theme]->info['name'],
      '#disabled' => TRUE,
    );

    $form['fusion_apply_settings'] = array(
      '#type' => 'textarea',
      '#title' => t('Fusion Apply settings'),
      '#default_value' => $code,
      '#rows' => min($lines, 150),
    );
  }
  else {
    // Give the option for which theme to export.

    $options = array();
    ksort($themes);
    $current_theme = fusion_apply_current_theme(TRUE);

    // Put default theme at the top.
    $options[$current_theme] = $themes[$current_theme]->info['name']. ' [' . t('default') . ']';

    foreach ($themes as $theme) {
      if (!empty($theme->info['hidden'])) {
        continue;
      }

      if ($theme->name == $current_theme) {
        // Do nothing.
      }
      elseif ($theme->status) {
        $options[$theme->name] = $theme->info['name'] . ' [' . t('enabled') . ']';
      }
      else {
        $options[$theme->name] = $theme->info['name'];
      }
    }

    $form['theme'] = array(
      '#type' => 'select',
      '#title' => t('Theme'),
      '#description' => t('Theme to export the Fusion Apply settings for.'),
      '#options' => $options,
      '#required' => TRUE,
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Export'),
    );
  }

  return $form;
}

/**
 * Form validation handler for fusion_apply_ui_export_form().
 */
function fusion_apply_ui_export_form_validate(&$form, &$form_state) {
  if (!empty($form_state['values']['theme']) && preg_match('/[^a-zA-Z0-9_]/', $form_state['values']['theme'])) {
    form_error($form['theme'], t('The theme name must be alphanumeric and can contain underscores only.'));
  }
}

/**
 * Form submission handler for fusion_apply_ui_export_form().
 */
function fusion_apply_ui_export_form_submit(&$form, &$form_state) {
  drupal_goto('admin/appearance/fusion/list/export/' . str_replace('_', '-', $form_state['values']['theme']));
}

/**
 * Form builder for the Fusion Apply settings import form.
 *
 * @ingroup forms
 */
function fusion_apply_ui_import_form($form, &$form_state) {
  $form['skin_configurations'] = array(
    '#type' => 'textarea',
    '#title' => t('Skin configurations'),
    '#description' => t('Paste skin coonfigurations here.'),
    '#rows' => 16,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );

  return $form;
}

/**
 * Form validation handler for fusion_apply_ui_import_form().
 */
function fusion_apply_ui_import_form_validate(&$form, &$form_state) {
  if (empty($form_state['values']['skin_configurations'])) {
    // Error.
    form_error($form['skin_configurations'], t('These are not valid skin configurations.'));
    return;
  }

  $skins = '';
  ob_start();
  eval($form_state['values']['skin_configurations']);
  ob_end_clean();

  foreach ($skins as $key => $skin) {
    if (!is_array($skin)) {
      form_error($form['skin_configurations'], t('These are not valid skin configurations.'));
      break;
    }
    $skins[$key] = (object) $skin;
    if (!fusion_apply_skin_validate($skins[$key])) {
      form_error($form['skin_configurations'], t('These are not valid skin configurations.'));
    }
  }

  $form_state['skins'] = &$skins;
}

/**
 * Form submission handler for fusion_apply_ui_import_form().
 */
function fusion_apply_ui_import_form_submit(&$form, &$form_state) {
  foreach ($form_state['skins'] as $skin) {
    // Find existing skin configuration and grab its sid.
    $params = array(
      'theme' => $skin->theme,
      'module' => $skin->module,
      'element' => $skin->element,
      'skin' => $skin->skin,
    );
    $sids = fusion_apply_skin_get_sids($params);
    if (!empty($sids)) {
      $skin->sid = reset($sids);
    }

    // Save skin configuration.
    if (!fusion_apply_skin_save($skin)) {
      drupal_set_message(t('Not all skin configurations could be saved!'), 'error', FALSE);
    }
  }

  drupal_set_message(t('The skin configurations have been saved.'));
  drupal_goto('admin/appearance/fusion/list');
}

/**
 * Form builder for the skin settings delete confirmation form.
 *
 * @param $theme
 *   The name of the theme to delete a setting for.
 * @param $module
 *   The module to delete a setting for.
 * @param $element
 *   The ID of the setting to delete.
 *
 * @ingroup forms
 */
function fusion_apply_ui_delete_confirm($form, &$form_state, $skin) {
  $form['#skin'] = $skin;
  // Always provide skin configuration sid in the same form key as in the skin
  // configuration edit form.
  $form['sid'] = array(
    '#type' => 'value',
    '#value' => $skin->sid,
  );

  $themes = list_themes();
  return confirm_form($form,
    t('Are you sure you want to delete this skin configuration?'),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/appearance/fusion',
    t('This action cannot be undone.<br />Theme: %theme<br />Module: %module<br />Element: %element<br />Skin: %skin', array('%theme' => $themes[$skin->theme]->info['name'], '%module' => $skin->module, '%element' => $skin->element, '%skin' => $skin->skin)),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Form submission handler for fusion_apply_ui_delete_confirm().
 */
function fusion_apply_ui_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    fusion_apply_skin_delete($form_state['values']['sid']);
    watchdog('content', 'Deleted a skin configuration.');
    drupal_set_message(t('A skin configuration has been deleted.'));
  }

  $form_state['redirect'] = 'admin/appearance/fusion';
}
