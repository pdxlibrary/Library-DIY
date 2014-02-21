This module provides simulated support for CSS3 media queries for Internet
Explorer versions 6-8.  Later versions natively support media queries, as do
modern versions of Firefox (1+), Safari (2+) and Chrome.

For more information on media queries, please see the W3C spec:
http://www.w3.org/TR/css3-mediaqueries/

For full documentation, please see:
http://fusiondrupalthemes.com/support/theme-developers/compatibility-standards/responsive-fusion-themes-and-internet-explorer

-------------------------------------------------------------------------------
INSTALLATION

1. Download the css3-mediaqueries-js library:
   http://code.google.com/p/css3-mediaqueries-js/

2. Create a new folder: sites/all/libraries/css3-mediaqueries

3. Save the library file inside the folder.
   NOTE: Do not rename the file; it must be called css-mediaqueries.js.

4. Download the Libraries module (7.x-2.x branch) for Drupal:
   http://www.drupal.org/project/libraries

5. Add the Libraries module to /sites/all/modules.

6. Enable the Libraries module, and the "Responsive Layouts for IE 6-8" module.


-------------------------------------------------------------------------------
FAQ

Q. Why not use respond.js?
A. Respond.js only provides support for min-width and max-width.  This library
   is larger than respond.js, but provides more complete support for media
   query expressions.

Q. What version of css3-mediaqueries-js do I require?
A. At this time, that library has not been versioned.

Q. Why won't css3-mediaqueries-js aggregate like the rest of my JavaScript?
A. This is a requirement of the library.  It also allows us to privilege this
   script over others, which will improve front end performance in most cases.


-------------------------------------------------------------------------------
CREDIT

- css3-mediaqueries-js by Wouter van der Graaf.
  http://code.google.com/p/css3-mediaqueries-js/

- Module developed by Jason Yergeau (aquariumtap).

- Sponsored by Top Notch Themes.
  http://www.topnotchthemes.com
