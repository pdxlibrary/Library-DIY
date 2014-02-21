
(function ($) {

/**
 * Provides a toggle for region label blocks.
 */
Drupal.behaviors.FusionLabels = {
  attach: function(context, settings) {

    $('div#fusion-label-toggle').toggle(function() {
      $(this).toggleClass('grid-on');
    },
    function() {
      $(this).toggleClass('grid-on');
    });
  }
};

/**
 * Applies the grid overlay.
 */
Drupal.behaviors.fusionGridMask = {
  attach: function (context, settings) {
    // Exit if grid mask not enabled
    if ($('body.grid-mask-enabled').size() == 0) {
      return;
    }

    var grid_width_pos = parseInt($('body').attr('class').indexOf('grid-width-')) + 11;
    var grid_width = $('body').attr('class').substring(grid_width_pos, grid_width_pos + 2);
    var grid = '<div id="grid-mask-overlay" class="full-width"><div class="region">';
    for (i = 1; i <= grid_width; i++) {
      grid += '<div class="block grid' + grid_width + '-1"><div class="gutter"></div></div>';
    }
    grid += '</div></div>';
    $('body.grid-mask-enabled').prepend(grid);
    $('#grid-mask-overlay .region').addClass('grid' + grid_width + '-' + grid_width);
    $('#grid-mask-overlay .block .gutter').height($('body').height());
  }
};

/**
 * Provides a toggle for grid overlay.
 */
Drupal.behaviors.fusionGridMaskToggle = {
  attach: function (context, settings) {
    // Exit if grid mask not enabled
    if ($('body.grid-mask-enabled').size() == 0) {
      return;
    }

    $('body.grid-mask-enabled').prepend('<div id="grid-mask-toggle">grid</div>');

    $('div#grid-mask-toggle')
    .toggle( function () {
      $(this).toggleClass('grid-on');
      $('body').toggleClass('grid-mask');
    },
    function() {
      $(this).toggleClass('grid-on');
      $('body').toggleClass('grid-mask');
    });
  }
};

/**
 * Provides a toggle for grid overlay.
 */
Drupal.behaviors.FusionLabelsToggle = {
  attach: function (context, settings) {
    $('#fusion-label-toggle').toggle(
      function() {
        $('div.block-fusion-labels').show();
      },
      function() {
        $('div.block-fusion-labels').hide();
      }
    );
  }
};

})(jQuery);