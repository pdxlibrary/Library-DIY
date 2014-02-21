<?php

/**
 * Implements hook_fusion_apply_skin_info().
 */
function fusion_apply_test_fusion_apply_skin_info() {
  $skins['fusion_apply_test_font'] = array(
    'title' => t('Font family'),
    'type' => 'select',
    'group' => 'typography',
    'theme hooks' => array('block', 'region'),
    'default status' => 1,
    'attached' => array(
      'css' => array('fusion_apply_test.css'),
      'js' => array('fusion_apply_test.js'),
    ),
    'options' => array(
      'font_1' => array(
        'title' => 'Arial, Helvetica, Nimbus Sans L, Liberation Sans, FreeSans',
        'class' => array('font-1'),
      ),
      'font_2' => array(
        'title' => 'Lucida Grande, Lucida Sans Unicode, DejaVu Sans, Tahoma',
        'class' => array('font-2'),
      ),
    ),
  );
  return $skins;
}
