<?php

$mini = new stdClass;
$mini->disabled = FALSE;
$mini->api_version = 1;
$mini->name = 'fusion_apply_ui_test_mini_panel';
$mini->category = '';
$mini->admin_title = 'Fusion Apply UI Test mini panel';
$mini->admin_description = 'Fusion Apply UI Test mini panel.';
$mini->requiredcontexts = array();
$mini->contexts = array();
$mini->relationships = array();
$display = new panels_display;
$display->layout = 'onecol';
$display->layout_settings = array();
$display->panel_settings = array(
  'style_settings' => array(
    'default' => NULL,
    'middle' => NULL,
  ),
);
$display->cache = array();
$display->title = '';
$display->content = array();
$display->panels = array();
  $pane = new stdClass;
  $pane->pid = 'new-1';
  $pane->panel = 'middle';
  $pane->type = 'block';
  $pane->subtype = 'system-user-menu';
  $pane->shown = TRUE;
  $pane->access = array();
  $pane->configuration = array(
    'override_title' => 0,
    'override_title_text' => '',
  );
  $pane->cache = array();
  $pane->style = array(
    'settings' => NULL,
  );
  $pane->css = array();
  $pane->extras = array();
  $pane->position = 0;
  $display->content['new-1'] = $pane;
  $display->panels['middle'][0] = 'new-1';
$display->hide_title = PANELS_TITLE_FIXED;
$display->title_pane = '0';
$mini->display = $display;
