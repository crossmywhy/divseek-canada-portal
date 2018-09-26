<?php

/**
 * @file
 * TripalCVBrowser class definition.
 */

/**
 * Tripal CV Browser entity.
 */
class TripalCVBrowser extends Entity {

  /**
   * Returns CV Browser label.
   */
  protected function defaultLabel() {
    return $this->title;
  }

  /**
   * Returns CV Browser URI.
   */
  protected function defaultUri() {
    return array('path' => 'tripal_cvb/' . $this->identifier());
  }

}
