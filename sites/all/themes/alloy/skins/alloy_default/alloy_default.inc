<?php

/**
 * Implements hook_fusion_apply_group_PLUGIN_info().
 */
function alloy_fusion_apply_group_alloy_default_info() {
  $groups['alloy_default'] = array(
    'title' => t('Alloy theme skins'),
    'description' => t('Alloy theme skins'),
    'weight' => -20,
  );  

  return $groups;
}

/**
 * Implements hook_fusion_apply_skin_PLUGIN_info().
 */
function alloy_fusion_apply_skin_alloy_default_info() {

  $skins = array();
  
  $skins['alloy_block_colors'] = array(
    'title' => t('Block styling'),
    'type' => 'select',
    'attached' => array('css' => array('alloy-default.css')),
    'description' => t('Change the background and text color of this block'),
    'group' => 'alloy_default',
    'theme hooks' => array('block'),
    'default status' => TRUE,
    'options' => array(
      'alloy-white' => array(
        'title' => 'White background, no shadow',
        'class' => array('alloy-white', 'alloy-bg'),
      ),
      'alloy-white-shadow' => array(
        'title' => 'White background, with shadow',
        'class' => array('alloy-white-shadow', 'alloy-bg'),
      ),
      'alloy-blue-title' => array(
        'title' => 'Blue title',
        'class' => array('alloy-blue-title'),
      ),    
      'alloy-green-list' => array(
        'title' => 'Alloy green bullets',
        'class' => array('alloy-green-list'),
      ),    
    ),
  );
  
  return $skins;
}
