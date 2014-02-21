<?php

/**
 * Implements hook_fusion_apply_skin_PLUGIN_info().
 */
function fusion_apply_test_fusion_apply_skin_example_info() {
  $skins['fusion_apply_test_example'] = array(
    'title' => t('Example skin plugin'),
  );
  return $skins;
}
