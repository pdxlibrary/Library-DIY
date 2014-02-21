<?php
/**
 * @file
 * This file contains no working PHP code; it exists to provide additional documentation
 * for doxygen as well as to document hooks in the standard Drupal manner.
 */

/**
 * @mainpage Fusion Apply API Manual
 *
 * Topics:
 * - @ref fusion_apply_hooks
 */

/**
 * @defgroup fusion_apply_hooks Fusion Apply's hooks
 * @{
 * Hooks that can be implemented by other modules in order to implement the
 * Fusion Apply API.
 */

/**
 * Configure skins for this module.
 *
 * This hook should be placed in MODULENAME.skins.inc and it will be auto-loaded.
 * This must either be in the same directory as the .module file or in a subdirectory
 * named 'includes'.
 *
 * The configuration info is keyed by the MODULENAME. In the case of $data['block']
 * 'block' is the name of the module.
 *
 * There are two section to the configuration array:
 * - When you specify a "form", Fusion Apply will insert its skins selector into the form
 *   with the specified form_id. Example: $data[MODULENAME]['form'][FORM_ID] = ...
 *   You can specify multiple forms that Fusion Apply should add its skins selector to. A
 *   good example where this would be needed is blocks where you have a different
 *   form_id for adding a new block than when editing an existing block.
 * - When you specify "preprocess", Fusion Apply will create a $vars['fusion_apply'] variable
 *   containing the appropriate skin classes for the specified preprocess hook.
 *   Example: $data[MODULENAME]['preprocess'][PREPROCESS_HOOK] = ...
 *
 * Form options:
 * - "index_handler" is required. It specifies a function that returns an index where
 *   Fusion Apply can find the values in its data structure.
 * - "access_handler" specifies a function that returns TRUE if you wish to grant access
 *   to Fusion Apply, or FALSE if not.
 * - "data_handler" specifies a function that returns the data used to populate the form.
 *   This is useful in cases where a module caches data (like panels and views) and has
 *   an option to cancel changes.
 * - "submit_handler" specifies a function that process the form data and saves it.
 * - "preprocess_hook" is required. Each skin states which preprocess hooks it will
 *   work for. This parameter will limit the available skins by the specified
 *   preprocess hook.
 * - "title" overrides the default title on the Fusion Apply fieldset.
 * - "description" overrides the default description that provides additional
 *   information to the user about this Apply selector.
 * - "weight" overrides the order where Apply's selector appears on the form.
 * - "collapsed" sets whether the fieldset appears collapsed or not. Defaults to TRUE.
 * - "selector_weight" overrides the weight of the selector field inside the fieldset.
 *   This is useful, for instance, if you have multiple modules add selectors to the
 *   same form.
 * - "selector_title" overrides the title of the selector field inside the fieldset.
 *
 * Preprocess options:
 * - "indexhandler" is required. It specifies a function that returns an index where
 *   Fusion Apply can find the values in its data structure.
 */
function hook_fusion_apply_config() {
  $data['example']['form']['block_admin_configure'] = array(
    'index_handler' => 'example_fusion_apply_index_handler',
    'preprocess_hook' => 'block',
    'title' => t('Fusion Apply settings'),
    'description' => t('Here you can manage which skins, if any, you want to apply.'),
    'weight' => 1,
    'collapsed' => TRUE,
    'selector_weight' => 0,
    'selector_title' => t('Choose Skin'),
  );
  $data['example']['form']['block_add_block_form'] = array(
    'index_handler' => 'example_fusion_apply_index_handler',
    'title' => t('Fusion Apply settings'),
    'description' => t('Here you can manage which skins, if any, you want to apply to this block.'),
    'weight' => -10,
    'collapsed' => FALSE,
  );

  $data['example']['preprocess']['block'] = array(
    'index_handler' => 'block_fusion_apply_preprocess_handler_block',
  );

  return $data;
}

/**
 * Define the API version of Fusion Apply your code is compatible with.
 *
 * This is required when creating a new Fusion Apply plugin. It checks to make sure
 * your Skins are compatible with the installed version of Fusion Apply and takes care
 * of loading the include files.
 *
 * @return
 *   An associative array describing Fusion Apply API integration:
 *   - directory: (optional) The name of a sub-directory, in which include files
 *     containing skin or group definitions may be found.
 *   - path: (optional) The path to the directory containing the directory
 *     specified in 'directory'. Defaults to the path of the module or theme
 *     implementing the hook.
 *   In case no Fusion Apply plugin include files exist for your implementation, simply
 *   define the function with an empty function body.
 *
 * The "hook" prefix is substituted with the name of the module or theme that
 * implements it, e.g. THEME_fusion_apply_api_VERSION() in template.php, or
 * MODULE_fusion_apply_api_VERSION() in MODULE.module.
 *
 * VERSION is normally identical to Fusion Apply's major version; e.g., "2".
 */
function hook_fusion_apply_api_VERSION() {
  return array(
    'path' => drupal_get_path('module', 'mymodule'),
    'directory' => 'skins',
  );
}

/**
 * Define the skin(s) for this plugin.
 *
 * Each skin definition consists of properties that define its form element and
 * settings that are needed to make it work, such as the class(es) Fusion Apply should
 * apply, which files it should load, whether or not it should be disabled by
 * default and which theme hook(s) it was designed to work with.
 *
 * Each skin name must be unique. Skins cannot have the same name even if they
 * are located in different plugins. It is recommended to prefix the name of
 * each skin with the name of the theme or module implementing it.
 *
 * Skin settings:
 * - title (required): Title of the skin form element.
 * - description (optional): Description of the skin form element.
 * - group (optional): The group the skin is attached to; defaults to a Fusion Apply
 *   provided group labeled "General."
 * - type (optional): The type of form element. Allowed values:
 *   - checkboxes (default): Useful when single or multiple options can be
 *     chosen. This type does not need to be set manually. Fusion will apply
 *     it by default.
 *   - select: Useful when a single option can be chosen, but many exist.
 *   - radios: Useful when a single option can be chosen and only a few options
 *     exist.
 * - weight (discouraged): Sets the weight of the skin inside the group; NULL
 *   by default. weight should not be set on each individual skin. Instead, it
 *   should be used sparingly where positioning a skin at the very top or
 *   bottom is desired.
 * - attached (optional): A array containing information about CSS and
 *   JavaScript files the skin requires. Each entry is an array keyed by type:
 *   - css (optional): Maps to the functionality of drupal_add_css() with one
 *     exception: paths are automatically assumed relative to the plugin
 *     directory, unless external. Examples:
 *     - Simple:
 *       'css' => array('css/skin-name.css')
 *     - Advanced:
 *       'css' => array(
 *         array('css/skin-name-ie.css', array(
 *           'media' => 'screen',
 *           'browsers' => array('IE' => 'lte IE 8'),
 *         ),
 *       )
 *   - js (optional): Maps to the functionality of drupal_add_js() with one
 *     exception: paths are automatically assumed as relative to the plugin
 *     directory, unless external. Examples:
 *     - Simple:
 *       'js' => array('js/skin-name.js')
 *     - Advanced:
 *       'js' => array(
 *         array(
 *           'js/skin-name-advanced.js',
 *           array(
 *             'scope' => 'footer',
 *             'group' => JS_THEME,
 *         ),
 *       )
 * - options (required): An array containing one or more options (also arrays)
 *   for the user to choose from when applying skins. Each option key should be
 *   a machine name describing the option. An option should including the
 *   following keys:
 *   - title (required): The option label.
 *   - class (required): An array containing one or more classes the skin
 *     should apply. All classes should be entered as an array: For example:
 *     'class' => array('class-b', 'class-b')
 *   - attached (optional): Same syntax as above, but applies to a specific
 *     option only.
 * - theme hooks (optional): An array containing certain allowed theme hooks,
 *   which allow you to limit where the skin can be used. Allowed options
 *   include: block, block__MODULE, comment_wrapper,comment__wrapper_NODETYPE,
 *   node, node__NODETYPE, region, region__REGIONNAME, panels_display,
 *   panels_region, panels_pane, views_view, views_view__STYLENAME,
 *   views_view__DISPLAYNAME, and views_view__VIEWNAME.
 * - default status (optional): Skins are disabled by default to keep UI
 *   clutter to a minimum. In some cases, like contrib themes, it makes sense to
 *   enable skins which are required to make the theme work properly by default.
 *   Setting this property to 1 will cause it to be enabled by default for all
 *   installed themes.
 * - status: (optional) An associative array whose keys are theme names and
 *   whose corresponding values denote the desired default status for the
 *   particular theme.
 *
 * The "hook" prefix is substituted with the name of the module or theme
 * implementing it.
 */
function hook_fusion_apply_skin_info() {
  $skins['fusion_apply_menus'] = array(
    'title' => t('Menu styles'),
    'description' => t('Select a style to use for the main navigation.'),
    'type' => 'select',
    'group' => 'fusion_apply_menus',
    'theme hooks' => array('block__menu', 'block__menu_block'),
    'attached' => array(
      'css' => array('css/nav.css'),
    ),
    'options' => array(
      'one_level' => array(
        'title' => t('Standard (1 level) - No colors'),
        'class' => array('nav'),
      ),
      'menu_a' => array(
        'title' => t('Standard (1 level) - Green'),
        'class' => array('nav', 'nav-a'),
        'attached' => array('css' => array('css/nav-colors.css')),
      ),
      'menu_b' => array(
        'title' => t('Standard (1 level) - Blue'),
        'class' => array('nav', 'nav-b'),
        'attached' => array('css' => array('css/nav-colors.css')),
      ),
      'menu_c' => array(
        'title' => t('Dropdowns - No colors'),
        'class' => array('nav', 'nav-dd'),
        'attached' => array(
          'css' => array('css/nav-dd.css'),
          'js' => array('js/dropdown.js'),
        ),
      ),
      'menu_d' => array(
        'title' => t('Dropdowns - Green'),
        'class' => array('nav', 'nav-dd', 'nav-a'),
        'attached' => array(
          'css' => array('css/nav-dd.css'),
          'js' => array('js/dropdown.js'),
        ),
      ),
      'menu_e' => array(
        'title' => t('Dropdowns - Blue'),
        'class' => array('nav', 'nav-dd', 'nav-b'),
        'attached' => array(
          'css' => array('css/nav-dd.css'),
          'js' => array('js/dropdown.js'),
        ),
      ),
    ),
    // Optional: Specify a global default status for this skin, making it
    // available or unavailable to all themes.
    'default status' => 0,
    // Optional: Specify a default status for a particular theme. This mainly
    // makes sense for skins provided by themes only.
    'status' => array(
      'bartik' => 1,
      'garland' => 0,
      // In case you are registering a skin for your base theme, then you likely
      // do not know which sub themes are going to use your base theme. By
      // setting the global default status to 0 (as above) and enabling the skin
      // for your base theme itself, the skin's status will be automatically
      // inherited to all sub themes of your base theme.
      'mybasetheme' => 1,
    ),
  );
  return $skins;
}

/**
 * Define one or more skins in an atomic Fusion plugin file.
 *
 * This hook works identically to hook_fusion_apply_skin_info(), but allows to place
 * one or more related skin definitions into a separate plugin file.
 *
 * For example, considering a module or theme with the name "extension" that
 * contains an include file:
 * @code
 * extension/skins/example/example.inc
 * @encode
 * The "hook" prefix is substituted with the name of the module or theme
 * implementing it ("extension"), and PLUGIN is substituted with the name of the
 * include file ("example"), e.g., THEME_fusion_apply_skin_PLUGIN_info() or
 * MODULE_fusion_apply_skin_PLUGIN_info(). For above example, the function name would
 * be: extension_fusion_apply_skin_example_info().
 */
function hook_fusion_apply_skin_PLUGIN_info() {
  $skins['extension_example_menus'] = array(
    'title' => t('Example menu styles'),
    'type' => 'select',
    'group' => 'fusion_apply_menus',
    'theme hooks' => array('block__menu', 'block__menu_block'),
    'attached' => array(
      'css' => array('css/nav.css'),
    ),
    'options' => array(
      'menu_a' => array(
        'title' => t('Standard (1 level) - Green'),
        'class' => array('nav', 'nav-a'),
        'attached' => array('css' => array('css/nav-colors.css')),
      ),
      'menu_b' => array(
        'title' => t('Standard (1 level) - Blue'),
        'class' => array('nav', 'nav-b'),
        'attached' => array('css' => array('css/nav-colors.css')),
      ),
    ),
  );
  return $skins;
}

/**
 * Perform alterations on skins.
 *
 * @param $skins
 *   An array of skin information exposed by hook_fusion_apply_skin_info()
 *   implementations.
 */
function hook_fusion_apply_skin_info_alter(&$skins) {
  // Remove restrictions on theme hooks the skin works with.
  unset($skins['fusion_apply_menus']['theme hooks']);
}

/**
 * Defines group(s) that will contain skins.
 *
 * Groups are form element containers used to organize skins categorically. If
 * you do not define a group, your skins will appear in Fusion Apply's default group,
 * which is labeled "General." Fusion implements 4 default groups, which may be
 * used in any skin implementation. For more information, see skins/default.inc.
 *
 * Each group name must be unique. It is recommended to prefix the name of each
 * group with the name of the theme or module name implementing it, followed by
 * the name of the group. Note that you cannot define 2 groups with the same
 * name, even if they are in different plugins.
 *
 * Group properties:
 * - title (required): Brief title of the tab.
 * - description (optional): Description of the group for administration page.
 * - weight (discouraged): Weight of the tab group; 0 by default.
 *
 * The "hook" prefix is substituted with the name of the module or theme that
 * implements it.
 *
 * @see fusion_apply_default_fusion_apply_group_info()
 */
function hook_fusion_apply_group_info() {
  $group['fusion_apply_menus'] = array(
    'title' => t('Menus'),
    'description' => t('Menu and navigation styles.'),
  );

  return $groups;
}

/**
 * Define one or more skin groups in an atomic plugin file.
 *
 * This hook works identically to hook_fusion_apply_group_info(), but allows to place
 * one or more related skin group definitions into a separate plugin file.
 *
 * For example, considering a module or theme with the name "extension" that
 * contains an include file:
 * @code
 * extension/skins/example/example.inc
 * @encode
 * The "hook" prefix is substituted with the name of the module or theme
 * implementing it ("extension"), and PLUGIN is substituted with the name of the
 * include file ("example"), e.g., THEME_fusion_apply_group_PLUGIN_info() or
 * MODULE_fusion_apply_group_PLUGIN_info(). For above example, the function name would
 * be: extension_fusion_apply_group_example_info().
 */
function hook_fusion_apply_group_PLUGIN_info() {
  $group['extension_example_menus'] = array(
    'title' => t('Menus'),
    'description' => t('Menu and navigation styles.'),
  );
  return $groups;
}

/**
 * Perform alterations on groups.
 *
 * @param $groups
 *   An array of group information exposed by hook_fusion_apply_group_info()
 *   implementations.
 */
function hook_fusion_apply_group_info_alter(&$groups) {
  // Show this tab group at the top of the Fusion Apply settings form.
  $groups['fusion_apply_menus']['weight'] = -1;
}

/**
 * @}
 */
