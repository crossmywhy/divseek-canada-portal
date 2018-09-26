<?php

/**
 * @file
 * This file containd theming hooks.
 */

/**
 * Implements hook_preprocess_HOOK().
 *
 * Prepares the variables for the CV browser list page.
 *
 * @ingroup tripal_cvb
 */
function tripal_cvb_preprocess_tripal_cvb_list(&$variables) {
  // Set the breadcrumb.
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  drupal_set_breadcrumb($breadcrumb);

  $variables['cvb_table'] = entity_ui_controller('tripal_cvb')->overviewTable();
  $variables['pager'] = theme('pager');
}
