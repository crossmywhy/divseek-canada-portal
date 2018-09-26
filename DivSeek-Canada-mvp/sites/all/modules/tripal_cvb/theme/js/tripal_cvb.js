/**
 * @file
 * Tripal CV browser Javascript library.
 */

(function ($) {
"use strict";

  Drupal.tripal_cvb = Drupal.tripal_cvb || {};

  /**
   * Returns the identifier part embeded into a class string.
   *
   * For instance, this function can extract '123' from the class string
   * 'tripal-cvb tripal-cvb-cvterm tripal-cvb-cvtermid-123' if the prefix is set
   * to 'tripal-cvb-cvtermid-'.
   *
   * @param string classes
   *   A string containing the class property of an HTML element.
   * @param string prefix
   *   The prefix of the class that contain the identifier.
   *
   * @return string
   *   The identifier part or false if nothing matched.
   */
  Drupal.tripal_cvb.getClassId = function (classes, prefix) {
    var re = new RegExp('(?:^|\\s)' + prefix + '(\\S+)');
    var id_match = re.exec(classes);
    return (id_match ? id_match[1] : false);
  }

  /**
   * Event handler that toggles the display of a subtree.
   *
   * The event is expected to occur on the first child (label) of an HTML
   * element. The function hides or display (toggle) the first "<ul>" child of
   * the parent element and update the parent class: "tripal-cvb-collapsed"
   * when the "<ul>" is hidden and "tripal-cvb-expanded" when the "<ul>" is
   * shown.
   *
   * Example of HTML structure where the span elements hold the events:
   * @code
   *   <div class="tripal-cvb-expanded">
   *     <span>click to toggle</span>
   *     <ul class="tripal-cvb-expanded">
   *       <li>
   *         <span>click to toggle subtree element a</span>
   *         <ul>
   *           <li>subtree leaf element 1</li>
   *           <li>subtree leaf element 2</li>
   *         </ul>
   *       </li>
   *       <li>leaf element b</li>
   *       <li>leaf element c</li>
   *     </ul>
   *   </div>
   * @endcode
   *
   * @param eventObject event
   *   A jQuery event object.
   */
  Drupal.tripal_cvb.toggleCVSubTree = function (event) {
    if ($(this).parent().is('.tripal-cvb-expanded')) {
      $(this)
        .parent()
          .removeClass('tripal-cvb-expanded')
          .addClass('tripal-cvb-collapsed');
      $(this).siblings('ul:first').hide();
    }
    else {
      $(this)
        .parent()
          .removeClass('tripal-cvb-collapsed')
          .addClass('tripal-cvb-expanded');
      $(this).siblings('ul:first').show();
    }
  }

  /**
   * Initializes the CV browser tree nodes.
   *
   * Adds the handler to load and toggle subtrees as well as CV term actions.
   */
  Drupal.tripal_cvb.initCVTreeNodes = function () {
    // Adds toggle to CV nodes.
    $('.tripal-cvb-has-children > .tripal-cvb-cv')
      .on('click', Drupal.tripal_cvb.toggleCVSubTree);

    // Adds subtree loading handlers to nodes with children.
    $('.tripal-cvb-has-children > .tripal-cvb-cvterm')
      .not('.tripal-cvb-onclick-processed')
      .addClass('tripal-cvb-onclick-processed')
      .on('click.tripal-cvb-expand', function () {
        var $cvterm_span = $(this);
        var $cvterm_li = $(this).parent();
        var term_classes = $cvterm_li
          .children('.tripal-cvb-cvterm:first')
          .prop('class');
        var cvterm_id = Drupal.tripal_cvb.getClassId(
          term_classes,
          'tripal-cvb-cvtermid-'
        );
        $cvterm_li
          .find('> .ajax-progress')
            .remove();
        $cvterm_li
          .removeClass('tripal-cvb-collapsed')
          .addClass('tripal-cvb-expanded')
          .prepend($('<div class="ajax-progress"><div class="throbber">&nbsp;</div></div>'));
        // Calls the function that returns the children CV Terms as JSON.
        $.ajax({
          url: Drupal.settings.basePath
            + 'tripal/cvb/cvterm/'
            + cvterm_id
            + '/children',
          type: 'GET',
          dataType: 'json',
          success: function (output) {
            if (output && ('object' == typeof output)
                && (Object.keys(output).length > 0)) {
              // Appends subtree.
              var $ul = $('<ul class="tripal-cvb"></ul>');
              // Appends subtree elements.
              $.each(output, function (child_cvterm_id, child_cvterm) {
                var li_class =
                'tripal-cvb'
                  + (0 < parseInt(child_cvterm.children_count) ?
                    ' tripal-cvb-has-children tripal-cvb-collapsed'
                    : ' tripal-cvb-leaf'
                  )
                  + ' tripal-cvb-relationship-'
                  + child_cvterm.relationship.replace(/\W+/g, '_');
                var cvterm_class =
                  'tripal-cvb-cvterm tripal-cvb-cvtermid-' + child_cvterm_id
                  + (child_cvterm.is_obsolete == '1' ?
                    ' tripal-cvb-obsolete'
                    : ''
                  );
                var cvterm_title = child_cvterm.definition;
                if (cvterm_title) {
                  cvterm_title =
                    '('
                    + child_cvterm.cv.replace('"', "'")
                    + ') '
                    + cvterm_title.replace('"', "'");
                }
                else {
                  cvterm_title = '';
                }
                var cvterm_name = child_cvterm
                  .name
                    .replace('&', '&amp;')
                    .replace('<', '&lt;')
                    .replace('>', '&lt;');
                var $subcvterm_span = $(
                  '<span class="'
                  + cvterm_class
                  + '" title="'
                  + cvterm_title
                  + '">'
                  + cvterm_name
                  + '</span>');
                // Associate CV term data to the span element.
                $subcvterm_span.data('cvterm', child_cvterm);
                var $subcvterm_li = $(
                  '<li class="'
                  + li_class
                  + '"></li>');
                $ul.append($subcvterm_li.append($subcvterm_span));
              });
              $cvterm_li.append($ul);
              Drupal.tripal_cvb.initCVTreeNodes();
            }
            $cvterm_span
              .off('click.tripal-cvb-expand')
              .on('click', Drupal.tripal_cvb.toggleCVSubTree);
            $cvterm_li
              .find('> .ajax-progress')
                .remove();
          },
          error: function (jqXHR, textStatus, errorThrown) {
            $cvterm_li
              .find('> .ajax-progress .throbber')
                .remove();
            $cvterm_li
              .find('> .ajax-progress')
                .append($('<div class="error">&nbsp;</div>'));
            alert(
              'Failed to get CVTerm data for cvterm_id '
              + cvterm_id
              + ': '
              + textStatus
            );
          }
        });
      });

    // Add actions.
    $('li.tripal-cvb')
      .not('.tripal-cvb-action-processed')
      .addClass('tripal-cvb-action-processed')
      .each(function (index, element) {
        var browser_classes = $(element)
            .parents('.tripal-cvb-browser:first')
            .prop('class');
        var browser_id = Drupal.tripal_cvb.getClassId(
          browser_classes,
          'tripal-cvb-browser-'
        );
        var $cvterm = $(element).children('.tripal-cvb-cvterm:first');
        var cvterm_data = $cvterm.data('cvterm');
        var term_classes = $cvterm.prop('class');
        var cvterm_id = cvterm_data.cvterm_id;
        var actions = [];
        try {
          actions = eval('tripal_cvb_' + browser_id.replace(/\W/g, '_'));
        }
        catch (error) {
          console.log(error);
        };
        $.each(actions, function (index, action) {
          var $action_element = $(
            '<span class="tripal-cvb-action tripal-cvb-action-'
            + index
            + ' tripal-cvb-action-'
            + action.action.replace(/:/g, '-').replace(/[^\w-]/g, '_')
            + '">'
            + action.title
            + '</span>'
          );
          var display_content = function (content) {};
          var get_content = function () {return '';};

          // Get target.
          var target_selector = $action_element;
          var target_match = action.target.match(/(.+):(.*)/);
          var target_type = action.target;
          var target_id = '';
          if (target_match) {
            target_type = target_match[1];
            target_id = target_match[2];
          }
          switch (target_type) {
            case 'dom':
              target_selector = target_id;
              break;

            case 'term':
              target_selector = $action_element;
              break;

            case 'region':
              var target_selector = '.region-' . target_id;
              if ('content' == target_id) {
                target_selector = '#content';
              }
              break;

            default:
              break;
          }

          // Get display function.
          switch (action.insert) {
            case 'replace':
              display_content = function (content) {
                $(target_selector).html(content);
                if ($(target_selector).is('.tripal-cvb-action')) {
                  $(target_selector).addClass('replaced');
                }
              };
              break;

            case 'append':
              display_content = function (content) {
                $(target_selector).append(content);
              }
              break;

            case 'class':
              // For term, add class to parent <li>.
              if ('term' == target_type) {
                target_selector = $(element);
              }
              display_content = function (content) {
                var item_class = jQuery(content).text().trim();
                if (null !== item_class.match(/^-?[_a-zA-Z]+[_a-zA-Z0-9-]*$/)) {
                  $(target_selector).addClass(item_class);
                }
                else {
                  console.log('ERROR: CVTerm action: invalid class selector returned will be ignored. No class added to the target element. Returned class string:' + item_class);
                }
              }
              break;

            default:
              alert('Unsupported insertion method: ' + action.insert);
              break;
          }

          // Get content fetching function.
          var action_parameters = action.action;
          switch (action.type) {
            case 'view':
              get_content = function () {
                $action_element.prepend($('<div class="ajax-progress"><div class="throbber">&nbsp;</div></div>'));
                var view_match = action_parameters.match(/(.+):(.+)/);
                if (view_match) {
                  var view_name = view_match[1];
                  var display_id = view_match[2];
                  $.ajax({
                    url: Drupal.settings.basePath + 'views/ajax',
                    type: 'get',
                    data: {
                      view_name: view_name,
                      view_display_id: display_id,
                      view_args: cvterm_id,
                    },
                    dataType: 'json',
                    success: function (response) {
                      if (response[1] !== undefined) {
                          display_content(response[1].data);
                          $action_element.find('.ajax-progress').remove();
                      }
                    },
                    error: function (jqXHR, textStatus, errorThrown) {
                      $action_element.find('.ajax-progress').remove();
                      console.log('ERROR: Tripal CV browser: Failed to get view content.');
                    }
                  });
                }
                else {
                  alert('Invalid view settings!');
                }
              }
              break;

            case 'path':
              // Drupal path: extract content from the URL.
              // Prepare call URL.
              action_parameters = Drupal.settings.basePath + action_parameters;
              // Replace the placeholder with the cvterm_id parameter.
              action_parameters = action_parameters.replace('%', cvterm_id);
              action_parameters = action_parameters.replace(/!cvterm_id/g, cvterm_data.cvterm_id);
              action_parameters = action_parameters.replace(/!cvterm/g, cvterm_data.name);
              action_parameters = action_parameters.replace(/!cv_id/g, cvterm_data.cv_id);
              action_parameters = action_parameters.replace(/!cv/g, cvterm_data.cv);
              action_parameters = action_parameters.replace(/!dbxref_id/g, cvterm_data.dbxref_id);
              action_parameters = action_parameters.replace(/!accession/g, cvterm_data.dbxref);
              action_parameters = action_parameters.replace(/!db/g, cvterm_data.db);
              get_content = function () {
                $action_element.prepend($('<div class="ajax-progress"><div class="throbber">&nbsp;</div></div>'));
                $.ajax({
                  url: action_parameters,
                  type: 'html',
                  success: function (response) {
                    if (response) {
                      // Extract #content part.
                      display_content($(response).find('#content').html());
                      $action_element.find('.ajax-progress').remove();
                    }
                  },
                  error: function (jqXHR, textStatus, errorThrown) {
                    $action_element.find('.ajax-progress').remove();
                    console.log('ERROR: Tripal CV browser: Failed to get path content.');
                  }
                });
              }
              break;

            case 'url':
              // Disable autorun for external links.
              action.autorun = false;
              // Get CV term data.
              var action_url = action.action;
              action_url = action_url.replace(/!cvterm/g, cvterm_data.name);
              action_url = action_url.replace(/!accession/g, cvterm_data.dbxref);
              action_url = action_url.replace(/!cv/g, cvterm_data.cv);
              action_url = action_url.replace(/!db/g, cvterm_data.db);
              // Generate a link.
              $action_element = $(
                '<a class="tripal-cvb-action" href="'
                + encodeURI(action_url)
                + '" title="'
                + action.title.replace('"', "'")
                + '">'
                + action.title
                + '</a>'
              );
              break;

            case 'js':
              // Call given javascript function with cvterm object as parameter.
              get_content = function () {
                $(this).prepend($('<div class="ajax-progress"><div class="throbber">&nbsp;</div></div>'));
                try {
                  eval(action.action + '(cvterm_data);');
                }
                catch (error) {
                  console.log(error);
                };
                $(this).remove('.ajax-progress');
              }
              break;

            default:
              break;
          }

          $(element).append(' ').append($action_element);

          if (!action.autorun || ('0' == action.autorun)) {
            $action_element.on('click.tripal-cvb-action', function () {
              $(this)
                .removeClass('not-run')
                .off('click.tripal-cvb-action');
              get_content();
            });
            $action_element.addClass('not-run');
          }
          else {
            $action_element.addClass('auto-run');
            get_content();
          }

        });
      });

  };

  /**
   * Popups a cvterm content in an alert box.
   *
   * This function is used for test purpose. It can be seen as a demo function
   * for Javascript type of CV term actions.
   *
   * @param object cvterm
   *   A cvterm object.
   */
  Drupal.tripal_cvb.cvtermDump = function (cvterm) {
    alert(
      'Term: ' + cvterm.name + "\n"
      + 'CV: ' + cvterm.cv + "\n"
      + 'Term definition: ' + cvterm.definition + "\n"
      + 'DB: ' + cvterm.db + "\n"
      + 'Term URL: ' + cvterm.urlprefix + cvterm.dbxref + "\n"
      + 'Obsolete: ' + (1 == cvterm.is_obsolete ? 'yes' : 'no') + "\n"
      + 'Relationship type: '
      + (1 == cvterm.is_relationshiptype ? 'yes' : 'no') + "\n"
      + 'Parental relationship: '
      + (null != cvterm.relationship ? cvterm.relationship : 'none') + "\n"
      + 'Children terms: '
      + cvterm.children_count + ' term' + (cvterm.children_count > 1 ? 's' : '')
      + 'Chado cvterm_id: ' + cvterm.cvterm_id + "\n"
      + 'Chado cv_id: ' + cvterm.cv_id + "\n"
      + 'Chado dbxref_id: ' + cvterm.dbxref_id + "\n"
    );
  };

  /**
   * Initializes CV browser when the page is loaded.
   */
  Drupal.behaviors.tripal_cvb = {
    attach: function (context, settings) {

      $(function () {
        Drupal.tripal_cvb.initCVTreeNodes();
      });

    }
  };

})(jQuery);
