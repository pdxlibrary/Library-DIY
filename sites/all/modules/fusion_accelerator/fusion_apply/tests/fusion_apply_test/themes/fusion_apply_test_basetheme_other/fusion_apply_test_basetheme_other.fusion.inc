<?php

/**
 * Implements hook_fusion_apply_skin_info().
 */
function fusion_apply_test_basetheme_other_fusion_apply_skin_info() {
  $skins['fusion_apply_test_basetheme_other'] = array(
    'title' => 'Other base theme skin',
    'default status' => 1,
  );
  return $skins;
}
