<?php

/**
 * @file
 * Administration pages for custom_breadcrumbs_taxonomy.
 */

/**
 * Form builder; Displays an edit form for a custom breadcrumb taxonomy term record.
 *
 * @ingroup forms
 * @see custom_breadcrumbs_form_validate()
 * @see custom_breadcrumbs_form_submit()
 */
function custom_breadcrumbs_taxonomy_term_form($form, &$form_state, $type) {
  $form = array();
  $bid = arg(5);
  $breadcrumb = NULL;
  if (isset($bid)) {
    drupal_set_title(t('Edit Custom Breadcrumb for Taxonomy Term'));
    $breadcrumbs = custom_breadcrumbs_load_breadcrumbs('custom_breadcrumbs_taxonomy', 'custom_breadcrumbs_taxonomy_term', array('bid' => $bid));
    $breadcrumb = array_pop($breadcrumbs);
  }
  else {
    drupal_set_title(t('Add Custom Breadcrumb for Taxonomy Term'));
  }
  // @todo This will not scale well for large taxonomies.
  // @todo Is it possible to provide an autocomplete for several vocabularies?
  $options = array();
  // Create taxonomy tree of all terms not in excluded vocabularies.
  $excluded = variable_get('custom_breadcrumbs_taxonomy_excluded_vocabs', array());
  $vocabularies = taxonomy_get_vocabularies();
  foreach ($vocabularies as $vocabulary) {
    if (in_array($vocabulary->vid, $excluded)) continue;
    $tree = taxonomy_get_tree($vocabulary->vid);
    if ($tree) {
      foreach ($tree as $term) {
        $choice = new stdClass();
        $choice->option = array($term->tid => str_repeat('-', $term->depth) . $term->name);
        $options[] = $choice;
      }
    }
  }
  $form['tid'] = array(
    '#type' => 'select',
    '#title' => t('Taxonomy Term'),
    '#required' => TRUE,
    '#options' => $options,
    '#description' => t('Select the taxonomy term to apply this breadcrumb to. Terms from <a href="@excluded">excluded vocabularies</a> will not be listed here.', array('@excluded' => url('admin/config/user-interface/custom-breadcrumbs'))),
    '#default_value' => isset($breadcrumb->tid) ?  $breadcrumb->tid : '',
    '#weight' => -10,
  );
  module_load_include('inc', 'custom_breadcrumbs', 'custom_breadcrumbs.admin');
  $form += custom_breadcrumbs_common_form_elements($bid, $breadcrumb);
  $taxonomy_breadcrumbs_mode = variable_get('custom_breadcrumbs_taxonomy_use_hierarchy', TRUE);
  if ($taxonomy_breadcrumbs_mode) {
    // Limit one title and path per taxonomy term.
    $form['titles'] = array(
      '#type' => 'textfield',
      '#title' => t('Title'),
      '#required' => FALSE,
      '#description' => t('Specify the Title to use for the term breadcrumb. Leave blank to use the term name. Disable the <em>use taxonomy hierarchy</em> option on the <a href="@link">Custom Breadcrumbs Settings</a> page if you would prefer to specify several titles/paths for the breadcrumb.', array('@link' => url('admin/config/user-interface/custom-breadcrumbs'))),
      '#default_value' => isset($breadcrumb->titles) ? $breadcrumb->titles : NULL,
    );
    $form['paths'] = array(
      '#type' => 'textfield',
      '#title' => t('Path'),
      '#required' => TRUE,
      '#description' => t('Specify the Drupal path that the taxonomy term will be linked to. Currently operating in taxonomy breadcrumbs mode. Disable the <em>use taxonomy hierarchy</em> option on the <a href="@link">Custom Breadcrumbs Settings</a> page if you would prefer to specify several titles/paths for the breadcrumb.', array('@link' => url('admin/config/user-interface/custom-breadcrumbs'))),
      '#default_value' => isset($breadcrumb->paths) ? $breadcrumb->paths : NULL,
    );
  }
  $form['#module'] = 'custom_breadcrumbs_taxonomy';
  $form['#infokey'] = 'taxonomy_term';
  $form['#submit'][] = 'custom_breadcrumbs_form_submit';
  $form['#validate'][] = 'custom_breadcrumbs_form_validate';

  return $form;
}

/**
 * Form builder; Displays an edit form for a custom breadcrumb taxonomy vocabulary record.
 *
 * @ingroup forms
 * @see custom_breadcrumbs_form_validate()
 * @see custom_breadcrumbs_form_submit()
 */
function custom_breadcrumbs_taxonomy_vocabulary_form($form, &$form_state, $type) {
  $form = array();
  $bid = arg(5);
  $breadcrumb = NULL;
  if (isset($bid)) {
    drupal_set_title(t('Edit Custom Breadcrumb for Taxonomy Vocabulary'));
    $breadcrumbs = custom_breadcrumbs_load_breadcrumbs('custom_breadcrumbs_taxonomy', 'custom_breadcrumbs_taxonomy_vocabulary', array('bid' => $bid));
    $breadcrumb = array_pop($breadcrumbs);
  }
  else {
    drupal_set_title(t('Add Custom Breadcrumb for Taxonomy Vocabulary'));
  }

  // Get a list of all vocabularies.
  $vocabularies = taxonomy_get_vocabularies();
  $excluded = variable_get('custom_breadcrumbs_taxonomy_excluded_vocabs', array());
  $options = array();
  foreach ($vocabularies as $vid => $vocabulary) {
    if (!in_array($vocabulary->vid, $excluded)) {
      $options[$vid] = $vocabulary->name;
    }
  }
  $form['vid'] = array(
    '#type' => 'select',
    '#title' => t('Vocabulary'),
    '#options' => $options,
    '#required' => TRUE,
    '#description' => t('Select the taxonomy vocabulary to apply this breadcrumb to. <a href="@excluded">Excluded vocabularies</a> will not be listed here.', array('@excluded' => url('admin/config/user-interface/custom-breadcrumbs'))),
    '#default_value' => isset($breadcrumb->vid) ? $breadcrumb->vid : NULL,
    '#weight' => -10,
  );

  module_load_include('inc', 'custom_breadcrumbs', 'custom_breadcrumbs.admin');
  $form += custom_breadcrumbs_common_form_elements($bid, $breadcrumb);

  $taxonomy_breadcrumbs_mode = variable_get('custom_breadcrumbs_taxonomy_use_hierarchy', TRUE);
  if ($taxonomy_breadcrumbs_mode) {
    // Limit one title and path per taxonomy vocabulary.
    $form['titles'] = array(
      '#type' => 'textfield',
      '#title' => t('Title'),
      '#required' => FALSE,
      '#description' => t('Specify the Title to use for the vocabulary breadcrumb. Leave blank to use the vocabulary name. Disable the <em>use taxonomy hierarchy</em> option on the <a href="@link">Custom Breadcrumbs Settings</a> page if you would prefer to specify several titles/paths for the breadcrumb.', array('@link' => url('admin/config/user-interface/custom-breadcrumbs'))),
      '#default_value' => isset($breadcrumb->titles) ? $breadcrumb->titles : NULL,
    );
    $form['paths'] = array(
      '#type' => 'textfield',
      '#title' => t('Path'),
      '#required' => TRUE,
      '#description' => t('Specify the Drupal path that the taxonomy vocabulary will be linked to. Currently operating in taxonomy breadcrumbs mode. Disable the <em>use taxonomy hierarchy</em> option on the <a href="@link">Custom Breadcrumbs Settings</a> page if you would prefer to specify several titles/paths for the breadcrumb.', array('@link' => url('admin/config/user-interface/custom-breadcrumbs'))),
      '#default_value' => isset($breadcrumb->paths) ? $breadcrumb->paths : NULL,
    );
  }
  $form['#module'] = 'custom_breadcrumbs_taxonomy';
  $form['#infokey'] = 'taxonomy_vocabulary';
  $form['#submit'][] = 'custom_breadcrumbs_form_submit';
  $form['#validate'][] = 'custom_breadcrumbs_form_validate';
  return $form;
}
