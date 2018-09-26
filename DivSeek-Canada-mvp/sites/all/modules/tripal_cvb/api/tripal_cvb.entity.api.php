<?php

/**
 * @file
 * Provides an application programming interface (API) for CV Browser entities.
 */

/**
 * @defgroup tripal_cvb_api Tripal CV Browser API
 * @ingroup tripal_cvb
 * @{
 * Provides an application programming interface (API) for working with Tripal
 * CV Browser entitites
 * @}
 */

/**
 * Load a CV Browser.
 */
function tripal_cvb_load($cvbid, $reset = FALSE) {
  $browsers = tripal_cvb_load_multiple(array($cvbid), array(), $reset);
  return reset($browsers);
}

/**
 * Load multiple CV Browsers based on certain conditions.
 */
function tripal_cvb_load_multiple($cvbid = array(), $conditions = array(), $reset = FALSE) {
  return entity_load('tripal_cvb', $cvbid, $conditions, $reset);
}

/**
 * Save CV Browser.
 */
function tripal_cvb_save($browser) {
  entity_save('tripal_cvb', $browser);
}

/**
 * Delete single CV Browser.
 */
function tripal_cvb_delete($browser) {
  entity_delete('tripal_cvb', entity_id('tripal_cvb', $browser));
}

/**
 * Delete multiple CV Browsers.
 */
function tripal_cvb_delete_multiple($cvbids) {
  entity_delete_multiple('tripal_cvb', $cvbids);
}
