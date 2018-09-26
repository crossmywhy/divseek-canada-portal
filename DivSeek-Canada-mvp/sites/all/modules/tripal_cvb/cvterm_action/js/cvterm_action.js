/**
 * @file
 * Tripal CV term action field Javascript library.
 */

(function ($) {
"use strict";

  Drupal.cvterm_action = Drupal.cvterm_action || {};

  /**
   * Updates action field visibility.
   *
   * Updates action setting field visibility according to the selected action
   * type.
   */
  Drupal.cvterm_action.updateActionFields = function (action_type_field) {
    var $action_type_field = $(action_type_field);
    var $view_fields = $action_type_field
      .parents('.cvterm-action-widget')
        .find(
          "div[class$='-action-view-name'].form-item, div[class$='-action-display-id'].form-item, div[class*='-action-view-name '].form-item, div[class*='-action-display-id '].form-item"
        );
    var $action_field = $action_type_field
      .parents('.cvterm-action-widget')
        .find("div[class$='-action'].form-item, div[class*='-action '].form-item");
    var $autorun_field = $action_type_field
      .parents('.cvterm-action-widget')
        .find(
          "div[class$='-autorun'].form-item, div[class*='-autorun '].form-item"
        );
    var $target_fields = $action_type_field
      .parents('.cvterm-action-widget')
        .find(
          "div[class$='-target-type'].form-item, div[class$='-target-id'].form-item, div[class*='-target-type '].form-item, div[class*='-target-id '].form-item"
        );
    var target_type_field = $action_type_field
      .parents('.cvterm-action-widget')
        .find('select.cvterm-action-target-type').get();
    var $insertion_field = $action_type_field
      .parents('.cvterm-action-widget')
        .find(
          "div[class$='-insert'].form-item, div[class*='-insert '].form-item"
        );

    switch ($action_type_field.val()) {
      case 'view':
        $view_fields.css({'display': 'inline-block'});
        $action_field.hide();
        $autorun_field.show();
        $target_fields.show();
        Drupal.cvterm_action.updateTargetFields(target_type_field);
        $insertion_field.show();
        break;

      case 'url':
        $view_fields.css({'display': 'none'});
        $action_field.show();
        $autorun_field.hide();
        $target_fields.hide();
        $insertion_field.hide();
        break;

      case 'js':
        $view_fields.css({'display': 'none'});
        $action_field.show();
        $autorun_field.show();
        $target_fields.hide();
        $insertion_field.hide();
        break;

      default:
        $view_fields.css({'display': 'none'});
        $action_field.show();
        $autorun_field.show();
        $target_fields.show();
        Drupal.cvterm_action.updateTargetFields(target_type_field);
        $insertion_field.show();
        break;
    }
  }

  /**
   * Updates target field visibility.
   *
   * Updates target setting field visibility according to the selected target
   * type.
   */
  Drupal.cvterm_action.updateTargetFields = function (target_type_field) {
    var $target_type_field = $(target_type_field);
    var $target_id_field = $target_type_field
      .parents('.cvterm-action-widget')
        .find("div[class$='-target-id'].form-item, div[class*='-target-id '].form-item");
    switch ($target_type_field.val()) {
      case 'term':
        $target_id_field.hide();
        break;

      default:
        $target_id_field.show();
        break;
    }
  }

  /**
   * Initializes the action form widget.
   *
   * Adds the event handlers to modify the widget depending on the action type
   * selected.
   */
  Drupal.cvterm_action.initActionFormWidget = function () {
    $('.cvterm-action-widget')
      .not('.cvterm-action-widget-processed')
      .addClass('cvterm-action-widget-processed')
      .each(function (index, element) {
        var $element = $(element);
        $element
          .find('select.cvterm-action-type')
          .on('change', function (event) {
            Drupal.cvterm_action.updateActionFields(this);
          })
        $element
          .find('select.cvterm-action-target-type')
          .on('change', function (event) {
            Drupal.cvterm_action.updateTargetFields(this);
          })
      });
  }

  /**
   * Initializes CV term action fields when the page is loaded.
   */
  Drupal.behaviors.cvterm_action = {
    attach: function (context, settings) {

      $(function () {
        Drupal.cvterm_action.initActionFormWidget();
        $('.cvterm-action-widget select.cvterm-action-type')
          .each(function (index, element) {
            Drupal.cvterm_action.updateActionFields(element);
          });
        $('.cvterm-action-widget select.cvterm-action-target-type')
          .each(function (index, element) {
            Drupal.cvterm_action.updateTargetFields(element);
          });
      });

    }
  };

})(jQuery);
