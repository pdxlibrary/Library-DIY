<?php

/**
 * Implements hook_fusion_apply_group_PLUGIN_info().
 */
function rwdg_fusion_apply_group_rwdg_default_info() {
  $groups['rwdg_default'] = array(
    'title' => t('RWDG theme skins'),
    'description' => t('RWDG theme skins'),
    'weight' => -20,
  );  

  return $groups;
}

/**
 * Implements hook_fusion_apply_skin_PLUGIN_info().
 */
function rwdg_fusion_apply_skin_rwdg_default_info() {

  $skins = array();
  
  $skins['rwdg_block_colors'] = array(
    'title' => t('Block styling'),
    'type' => 'select',
    'attached' => array('css' => array('rwdg-default.css')),
    'description' => t('Change the background and text color of this block'),
    'group' => 'rwdg_default',
    'theme hooks' => array('block'),
    'default status' => TRUE,
    'options' => array(
      'rwdg-pink' => array(
        'title' => 'Pink background',
        'class' => array('rwdg-pink', 'rwdg-bg'),
      ),
      'rwdg-blue' => array(
        'title' => 'Blue background',
        'class' => array('rwdg-blue', 'rwdg-bg'),
      ),
      'rwdg-pink-title' => array(
        'title' => 'Pink title & list styling',
        'class' => array('rwdg-pink-title', 'rwdg-title-list'),
      ),
      'rwdg-blue-title' => array(
        'title' => 'Blue title & list styling',
        'class' => array('rwdg-blue-title', 'rwdg-title-list'),
      ),    
    ),
  );
  
  return $skins;
}
