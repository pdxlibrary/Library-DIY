<?php

/**
 * Implements hook_fusion_apply_skin_info().
 */
function fusion_apply_test_subtheme_other_fusion_apply_skin_info() {
  $skins['fusion_apply_test_subtheme_other'] = array(
    'title' => 'Other subtheme skin',
    'default status' => 1,
  );
  return $skins;
}
