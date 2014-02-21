
Custom Breadcrumbs 2.0
----------------------

Summary
-------
* Enable the module and any option submodules (see below for details)
* Assign 'administer custom breadcrumbs' permission to those roles that should
  be allowed to add/edit/delete custom breadcrumbs.
* Assign 'use php in custom breadcrumbs' to roles that should be allowed to use
  php to determine breadcrumb visibility.
* Go to Configuration > User Interface > Custom breadcrumbs Settings to select
  the 'Home' breacrumb text and possibly other global settings.
* Go to Structure > Custom breadcrumbs to add new breadcrumbs
* To add a breadcrumb, click on one of the tabs at the top of the page. For
  example, click 'Node' to create a custom breadcrumb based on node type.
* Fill in the required information for the breadcrumb (varies depending on 
  breadcrumb type, see below).
* For the titles, put each "crumb" one line after another (There is no need to
  put in "home")
* (optional) For each crumb title you can specify a title attribute ("tooltip")
  to add to the link. Separate the crumb title and the title attribute with a
  pipe (|) symbol:

  Item 1
  SubItem A|Title attribute for SubItemA (optional)
  SuperSubItem X

* For the paths, put the path to each crumb starting after the domain name.
  Don't include a leading or trailing slash.

  item1
  item-1/subitem-a
  item-1/subitem-a/supersubitem-x

* Click save to save the breadcrumb
* Visit the page and your breadcrumb should appear!

New Features
------------
Beginning with the 6.x-2.x release, custom breadcrumbs has many new features which are
available through optional modules in the custom breadcrumbs package. The base
module, required by all the others, is still custom_breadcrumbs. This module
handles custom breadcrumbs based on node type as described above. The following
optional modules can also be installed to provide custom breadcrumbs in a
variety of situations:

+ custom_breadcrumbs_views provides custom breadcrumbs on views pages.
  Once this module is enabled, a new "Views" tab will appear at
  admin/structure/custom_breadcrumbs. To add a views page breadcrumb, click on the
  tab and then select the view from list of available views. Fill in the
  visibility, title and paths sections as described above, and your breadcrumb
  should appear on your views page. Note that token substitution is possible
  with global and user tokens only. The $view object is available for use in
  the php_visibility section.

+ custom_breadcrumbs_paths provides custom breadcrumbs on nodes and views at
  a specified path (url). Once this module is enabled, a new "Path" tab will
  appear at admin/structure/custom_breadcrumbs.  To add a breadcrumb for a node
  or a view at a specific path, just enter the Drupal path in the Specific
  Path section. Fill in the visibility, title and paths sections as
  described above, and save the breadcrumb. Now your breadcrumb should appear
  on the node or view at the specific path that you selected. Note that custom
  breadcrumbs does not check the validity of the entered path. When entering a
  path for a particular language (see below), do not specify the two-letter
  language prefix. Custom breadcrumbs will assume the correct prefix according
  to the selected language. To use '*' as a wildcard, go to custom breadcrumbs
  configuration page at admin/config/user-interface/custom-breadcrumbs and select the
  'Use wildcard pattern matching in paths' option in the advanced settings
  section. When this option is enabled, the breadcrumb that best matches the
  path will be selected. The best match is determined by the depth at which
  the first wildcard appears. For example, if the path is path/to/some/content
  and breadcrumbs have been defined for path/to/* and path/to/some/*, the
  latter will be chosen as the best match.

+ custom_breadcrumbs_taxonomy provides custom breadcrumbs on taxonomy term
  pages, views, and for nodes that are assigned a taxonomy vocabulary or term.
  Once this module is enabled, two new tabs will appear appear at 
  admin/structure/custom_breadcrumbs: Term and Vocabulary. Breadcrumb generation
  can be handled in two different ways. If 'use the taxonomy term hierarchy'
  is checked at custom breadcrumbs configuration page, then breadcrumbs will
  be generated similarly to the taxonomy_breadcrumb module. Otherwise,
  breadcrumb generation will be according to the standard custom breadcrumbs
  approach.

  In taxonomy breadcrumb mode, the breadcrumb trail is automatically
  constructed based on the taxonomy term hierarchy:
  [HOME] >> [VOCABULARY] >> TERM >> [TERM] >> [TITLE]. In this mode the
  breadcrumb titles are the term and vocabulary names. The paths these titles
  are linked to can be assigned via the Term and Vocabulary tabs at
  admin/structure/custom_breadcrumbs. Providing a path for a vocabulary will enable
  the [VOCABULARY] portion of the breadcrumb.  The path for a term can
  similarly be set, but if one is not provided the default taxonomy/term/tid
  (where tid is a number, the taxonomy term id) will be used. Select the types
  of nodes to include or exclude at the custom breadcrumbs configuration
  settings page. The option to add the node title at the end of the breadcrumb
  trail can also be enabled on that page. There is also an option to append
  the current taxonomy term to the breadcrumb on taxonomy term pages
  (defined to be any page with drupal path */taxonomy/term/*).

  In the standard custom breadcrumbs mode, you can provide the titles and paths
  for constructing the breadcrumb trail on nodes that have defined taxonomy
  terms. Note that if a node has more than one term, the lightest term in the
  lightest vocabulary with a defined custom breadcrumb will be used.

  Note: do not use this module and the taxonomy_breadcrumb module at the same
  time. Custom_breadcrumbs_taxonomy has extended the functionality of 
  taxonomy_breadcrumb, so that module is not needed if you are using 
  custom_breadcrumbs.

  While at admin/config/user-interface/custom-breadcrumbs go ahead and enable any
  additional taxonomy breadcrumb options that suits your needs. If you are
  using views to override taxonomy term pages, then be sure to enable the
  "Use taxonomy breadcrumbs for views" option.

+ custom_breadcrumbsapi provides a simple api that allows custom breadcrumbs to
  be defined for module pages implementing the api. Module developers need to
  provide a modulename_custom_breadcrumbsapi() function that returns an array
  containing the names of the module pages for which custom breadcrumbs may be
  defined.

  The following is an example that could be used with the forum module.

  /**
   *  Implementation of hook_custom_breadcrumbsapi().
   *  Allow custom breadcrumbs for the following module pages.
   */
  function forum_custom_breadcrumbsapi() {
    return array('forum listing');
  }

  Then, in the callback functions for each of those pages, the following line
  must be inserted within the function (preferably after defining $breadcrumb
  but before setting the breadcrumb):
  
  drupal_alter('breadcrumb', $breadcrumb, 'module_page_name');

  Continuing with the forum module example, 'module_page_name' would be
  replaced with 'forum listing'.
  
  custom_breadcrumbsapi can also provide custom breadcrumbs for modules 
  implementing theme templates (e.g. files ending in .tpl.php). To add a 
  custom breadcrumb when a specific theme template file is called, click
  on the module page tab at admin/structure/custom_breadcrumbs. Select the
  template file from the list of theme templates (determined from the 
  theme registry). Then fill in the usual custom breadcrumbs information
  such as titles as paths. If using a php snippet for breadcrumb visibility
  or to specify titles and paths (see below), you have access to the template
  variables through $variables, an associative array defined by the module
  providing the template. See the documentation in the template file for
  details. For example, if a template file uses the variable $foo, then
  access to that variable would be through $variables['foo'].

User Interface
--------------
The user interface has been modified for Custom Breadcrumbs 2.0. Breadcrumbs
from all custom breadcrumbs modules are tabulated at
admin/structure/custom_breadcrumbs. The table can be sorted according to
breadcrumb name, type, language (if locale is enabled) by clicking on the
column headers. The table can also be filtered to display breadcrumbs of a
specific type, language, or combination of the two.

A new custom breadcrumbs fieldset has  been added to node edit pages. All
defined breadcrumbs for a particular node are displayed here, with an option to
edit each.  If no breadcrumbs have been defined for a particular node, then a
link can be followed back to admin/structure/custom_breacrumbs to add a custom
breadcrumb.

Languages
---------
If the core Locale module is enabled, then an additional option to specify the
language for the breadcrumb is available when constructing the breadcrumb trail
(with any of the custom breadcrumb modules).

HOME breadcrumb
---------------
The text to display at beginning of the breadcrumb trail can be assigned from
the custom breadcrumb configuration settings page. Typically this is Home or
your site name. You can leave it blank to have no home breadcrumb. As with
normal crumb titles, you can optionally specify a title attribute ("tooltip")
for the crumb. Just separate the crumb text and the title attribute text with a
pipe (|) symbol (i.e. Home crumb text|attribute title text). There is
also an advanced setting to set the Home breadcrumb text on ALL pages, not
just those with defined custom breadcrumbs. You can also use this feature to
remove the home breadcrumb on all pages on the site - just enable the advanced
setting and then leave the home breadcrumb text blank.

It is possible to translate the home reference title from custom breadcrumbs
using the i18n module. Just put this in your settings.php:

  $conf['i18n_variables'] = array(
    //custom breadcrumbs
    'custom_breadcrumb_home',
  );

Then you can change it for each language at
http://example.com/#lang-prefix#/admin/settings/custom-breadcrumbs.

See http://drupal.org/node/313272 for additional information.

Use PHP in breadcrumb titles and paths
--------------------------------------
If this advanced option is enabled at admin/settings/custom-breadcrumbs, then
users given 'use php in custom breadcrumbs' permission can include small php
code snippets (less than 250 characters)in the titles and/or paths fields of
the add breadcrumb form. Be careful when enabling this option, as the incorrect
use of php can break your site.

There are a couple of ways to use php in breadcrumbs and titles. One way is to
return an array of breadcrumb titles in the titles text field and a
corresponding array of breadcrumb paths in the paths text field such as

Titles:
<?php return array('title-1','title-2','title-3');?>

Paths:
<?php return array('path/to/title-1','path/to/title-2','path/to/title-3');?>

Sometimes, it may be more convient to assign the titles and paths in the same
code snippet, so you can also return an associate array with elements 'titles'
and 'paths' that contain the titles and paths arrays, respectively.
For example,

Titles:
<?php $titles = array('title-1','title-2','title-3');
$paths = array('path/to/title-1','path/to/title-2','path/to/title-3');
return array('titles' => $titles, 'paths' => $paths); ?>

(In this case, the paths text field will be ignored, so you can leave it
empty).

When defined, appropriate objects such as $node, $term, or $view, will be
available for these code snippets. Note that if this option is enabled and an
array is not returned, then the module defaults to the standard operation of
using each line of the titles and paths text fields to define a part of the
breadcrumb.

For longer code snippets (greater than 250 characters), you can save your code
snippet in an include file and use a php require_once statement in the titles
and/or paths section of your custom breadcrumb to include and evaluate your
code. See http://drupal.org/node/654766 for an example of this.

Add CSS classes to custom breadcrumb elements
---------------------------------------------

You can enable this feature on the custom breadcrumbs configuration screen
under the HTML element identifiers section. There are several options that
provide html class identifiers for theming custom breadcrumb links, including
add a 'custom-breadcrumbs-home' ID attribute to the home breadcrumb item,
adding numbered class attributes 'custom-breadcrumbs-item-N' for each
breadcrumb item, adding even and odd classes to breadcrumb items and storing an
identifier that is unique for each defined custom breadcrumbs. Using this
last option requires modifying your sites phptemplate_breadcrumb (or theme
override) function to actually add the class name to the breadcrumb container.
The class name is returned as a string by the function 
custom_breadcrumbs_unique_breadcrumb_id(). The identifier will be of the form
'custom-breadcrumbs-type-id'where type is the breadcrumb type (node, panels,
path, views or taxonomy) and id is the breadcrumb id number.
See http://drupal.org/node/643796#comment-2532998 for more information on this
feature.

Special Identifiers
-------------------
In Custom Breadcrumbs 2.0, Special identifiers are now provided as a separate,
optional module - custom_breadcrumbs_identifiers. At present, this module
provides the following identifiers:

<none>              - Produces a plain text crumb. This identifier should not
                      be used with the pipe (|) symbol.
<pathauto>          - Cleans the given path using your pathauto replacement
                      rules.
<book-hierarchy>    - Provides crumbs for each parent node of a book page.
                      Whatever is placed in the corresponding position of the
                      title area will be ignored. It should not be used with
                      the pipe (|) symbol.
<page-title>        - Provides a plain text crumb using the page title. Whatever
                      is placed in the corresponding position of the title area
                      will be ignored. It should not be used with the pipe (|)
                      symbol.
<menu-parent-trail> - Produces crumbs for each parent item for the given path.
                      The title information for this line will be ignored
                      because the menu link titles are used. If a path is not
                      provided following the pipe (|) symbol, the current path
                      with be used.

Additional special identifiers can be developed and added by contributed
modules that implement hook_cb_identifier_list(), to provide a description of
the identifer, and hook_cb_identifier_values(), to prepare the appropriate
crumb items. See the custom_breadcrumbs_identifiers.module for examples of
how to do this.

Identifiers should be added to the paths area in the following format:
identifier|path. To be recognized, the identifier must be enclosed in angular
brackets, and proceed any part of the path:

For example: <pathauto>|[ogname-raw]

Note that not all identifiers require the use of |path.

Authors
-------
bennybobw, dbabbage, Michelle, MGN
