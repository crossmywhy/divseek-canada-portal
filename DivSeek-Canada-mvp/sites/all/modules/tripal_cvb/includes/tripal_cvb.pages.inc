<?php

/**
 * @file
 * CV Browser page functions.
 */

/**
 * Returns a CV Browser detail page.
 *
 * Callback for hook_menu().
 *
 * @return array
 *   The form structure.
 *
 * @ingroup callbacks
 */
function tripal_cvb_view($cvbrowser, $view_mode = 'full', $langcode = NULL) {
  drupal_add_css(drupal_get_path('module', 'tripal_cvb') . '/theme/css/tripal_cvb.css');
  drupal_set_title(entity_label('tripal_cvb', $cvbrowser));
  return entity_view(
    'tripal_cvb',
    array(entity_id('tripal_cvb', $cvbrowser) => $cvbrowser),
    'full'
  );
}
