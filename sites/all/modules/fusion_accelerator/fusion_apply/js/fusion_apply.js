
(function ($) {

// Make sure our objects are defined.
Drupal.CTools = Drupal.CTools || {};
Drupal.Fusion = Drupal.Fusion || {};
Drupal.Fusion.editUrl = 'admin/appearance/fusion';
Drupal.Fusion.infoUrl = 'admin/appearance/fusion/skins';

Drupal.behaviors.Fusion = {
  attach: function(context, settings) {
    for (var i in settings.fusion_apply['areas']) {
      var $module = settings.fusion_apply['areas'][i]['module'];
      var $elements = settings.fusion_apply['areas'][i]['elements'];
      var $id = settings.fusion_apply['areas'][i]['id'];

      var $region = $('.fusion_apply-id-' + $id).once('fusion_apply-region', function() {});
      if (settings.fusion_apply['areas'][i]['classes'] == undefined) {
        settings.fusion_apply['areas'][i]['classes'] = $($region).attr('class');
      }

      if ($region.length > 0) {
        var $links = '';
        for (var $j in $elements) {
          var $classes = '';
          if ($j == 0) {
            $classes += ' first';
          }
          if ($j == $elements.length - 1) {
            $classes += ' last';
          }
          if ($elements.length > 1) {
            $links += '<li class="fusion_apply-link-' + $j + $classes + '"><a href="' + settings.basePath + Drupal.Fusion.editUrl + '/' + $module + '/' + $elements[$j] + '/' + $elements +'" class="fusion_apply-link ctools-use-dialog">' + Drupal.t('Apply a skin') + ' ' + (parseInt($j) + 1) + '</a></li>';
          }
          else {
            $links = '<li class="fusion_apply-link-0 first last"><a href="' + settings.basePath + Drupal.Fusion.editUrl + '/' + $module + '/' + $elements[$j] +'" class="fusion_apply-link ctools-use-dialog">' + Drupal.t('Edit skin') + '</a></li>';
          }
        }

        var $wrapper_classes = '';
        if ($module == 'page') {
          $wrapper_classes += ' fusion_apply-links-wrapper-page';
        }

        $region.prepend('<div class="fusion_apply-links-wrapper' + $wrapper_classes + '"><ul class="fusion_apply-links">' + $links + '</ul></div>');
        $region.get(0).fusion_apply = { 'module': $module, 'elements': $elements, 'id': $id };

        Drupal.behaviors.Dialog($region);
      };
    }

    $('div.fusion_apply-links-wrapper', context).once('fusion_apply-links-wrapper', function () {
      var $wrapper = $(this);
      var $region = $wrapper.closest('.fusion_apply-region');
      var $links = $wrapper.find('ul.fusion_apply-links');
      var $trigger = $('<a class="fusion_apply-links-trigger" href="#" />').text(Drupal.t('Configure')).click(
        function () {
          $wrapper.find('ul.fusion_apply-links').stop(true, true).slideToggle(100);
          $wrapper.toggleClass('fusion_apply-links-active');
          return false;
        }
      );

      // Attach hover behavior to trigger and ul.fusion_apply-links.
      $trigger.add($links).hover(
        function () { $region.addClass('fusion_apply-region-active'); },
        function () { $region.removeClass('fusion_apply-region-active'); }
      );
      // Hide the contextual links when user rolls out of the .fusion_apply-links-region.
      $region.bind('mouseleave', Drupal.Fusion.hideLinks).click(Drupal.Fusion.hideLinks);
      // Prepend the trigger.
      $links.end().prepend($trigger);
    });

    // Add a close handler to the dialog.
    if (Drupal.Dialog.dialog && !Drupal.Dialog.dialog.hasClass('fusion_apply-dialog-processed')) {
      Drupal.Dialog.dialog.addClass('fusion_apply-dialog-processed').bind('dialogbeforeclose', function(event, ui) {
        // Reset all the applied style changes.
        for (var i in Drupal.settings.fusion_apply['areas']) {
          var $id = Drupal.settings.fusion_apply['areas'][i]['id'];
          var $classes = Drupal.settings.fusion_apply['areas'][i]['classes'];
          $('.fusion_apply-id-' + $id).attr('class', $classes);
        }
      });
    }
  }
}

/**
 * Disables outline for the region contextual links are associated with.
 */
Drupal.Fusion.hideLinks = function () {
  $(this).closest('.fusion_apply-region')
    .find('.fusion_apply-links-active').removeClass('fusion_apply-links-active')
    .find('ul.fusion_apply-links').hide();
};

Drupal.behaviors.FusionLivePreview = {
  attach: function(context, settings) {
    $('#fusion_apply-ui-form .fusion_apply-ui-current-theme :input:not(.fusion_apply-live-preview-processed)', context).addClass('fusion_apply-live-preview-processed').change(function () {
      var $tag = $(this).attr('tagName');
      $tag = $tag.toLowerCase();

      var $module = $('#fusion_apply-ui-form #edit-module').val();
      var $element = $('#fusion_apply-ui-form #edit-element').val();
      var $elements = $('#fusion_apply-ui-form #edit-elements').val();
      if (!$elements) {
        $elements = $element;
      }

      var $name = $(this).attr('name');
      $name = $name.replace(/fusion_apply_settings\[.*_group\]\[[^\]]*\]\[([^\]]*)\]/, '$1');

      var $classes = '';
      var $add_classes = $(this).val();

      if ($tag == 'select') {
        $(this).find('option').each(function() {
          $classes += ' ' + $(this).attr('value');
        });
      }
      else if ($tag == 'input') {

      }

      // Use AJAX to grab the CSS and JS filename.
      $.ajax({
        type: 'GET',
        dataType: 'json',
        url: Drupal.settings.basePath + Drupal.Fusion.infoUrl + '/' + $name + '/' + $add_classes,
        success: function($data) {

          var $command = {
            command: 'fusion_applyAfterupdate',
            module: $module,
            elements: $elements,
            classes: {
              remove: $classes,
              add: $add_classes
            },
            css: $data.css,
            js: $data.js,
            nosave: true
          };

          Drupal.CTools.AJAX.commands.fusion_applyAfterupdate($command);
        }
      });
    });
  }
}

/**
 * AJAX responder command to dismiss the modal.
 */
Drupal.CTools.AJAX.commands.fusion_applyAfterupdate = function(command) {
  if (command.module && command.elements && (command.classes.remove || command.classes.add)) {
    if (command.css) {
      for (var j in command.css) {
        $(document.createElement('link')).attr({href: Drupal.settings.basePath + command.css[j].path, media: command.css[j].media, rel: 'stylesheet', type: 'text/css'}).appendTo('head');
      }
    }
    if (command.js) {
      for (var j in command.js) {
        $.getScript(Drupal.settings.basePath + command.js[j].path);
      }
    }

    for (var i in Drupal.settings.fusion_apply['areas']) {
      if (Drupal.settings.fusion_apply['areas'][i]['module'] == command.module && Drupal.settings.fusion_apply['areas'][i]['elements'] == command.elements) {
        $('.fusion_apply-id-' + Drupal.settings.fusion_apply['areas'][i]['id']).removeClass(command.classes.remove).addClass(command.classes.add);
        if (command.nosave == undefined || command.nosave == false) {
          Drupal.settings.fusion_apply['areas'][i]['classes'] = $('.fusion_apply-id-' + Drupal.settings.fusion_apply['areas'][i]['id']).attr('class');
        }
      }
    }
  }
}

})(jQuery);
