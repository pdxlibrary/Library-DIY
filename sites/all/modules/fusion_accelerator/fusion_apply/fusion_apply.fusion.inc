<?php

/**
 * @file
 * Implements skin definition hooks.
 */

/**
 * Implements hook_fusion_apply_group_info().
 */
function fusion_apply_fusion_apply_group_info() {
  $groups['general'] = array(
    'title' => t('General'),
    'description' => t('Styles for content such as lists, buttons, margins, padding, etc.'),
    'weight' => -10,
  );
  $groups['box'] = array(
    'title' => t('Box styles'),
    'description' => t('Presentational styles for the container.'),
  );
  $groups['typography'] = array(
    'title' => t('Typography'),
    'description' => t('Fonts, styles, sizes and other typography related skins.'),
  );
  $groups['layout'] = array(
    'title' => t('Layout'),
    'description' => t('Grid, layout and other structural related skins.'),
  );
  return $groups;
}
