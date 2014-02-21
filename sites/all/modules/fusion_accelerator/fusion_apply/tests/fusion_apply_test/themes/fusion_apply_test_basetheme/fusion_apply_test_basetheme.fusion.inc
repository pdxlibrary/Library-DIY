<?php

/**
 * Implements hook_fusion_apply_skin_info().
 */
function fusion_apply_test_basetheme_fusion_apply_skin_info() {
  $skins['fusion_apply_test_basetheme'] = array(
    'title' => 'Base theme skin',
    'default status' => 0,
    'status' => array(
      'fusion_apply_test_basetheme' => 1,
    ),
  );
  return $skins;
}
