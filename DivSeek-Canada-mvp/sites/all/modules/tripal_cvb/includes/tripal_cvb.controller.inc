<?php

/**
 * @file
 * TripalCVBrowserController class definition.
 */

/**
 * Entity controller for CV browser entities (TripalCVBrowser).
 */
class TripalCVBrowserController extends EntityAPIControllerExportable {

  /**
   * Creates a new CV browser.
   */
  public function create(array $values = array()) {
    global $user;
    $settings = tripal_cvb_get_settings();

    $values += array(
      'cvbid' => 0,
      'title' => 'CV Browser',
      'machine_name' => '',
      'description' => '',
      'root_type' => 'cvterm',
      'root_ids' => '0',
      'show_cv' => $settings['show_cv'],
      'show_relationships' => $settings['show_relationships'],
      'uid' => $user->uid,
      'type' => 'tripal_cvb',
    );
    return parent::create($values);
  }

  /**
   * Fills CV browser field settings.
   *
   * @see https://api.drupal.org/api/drupal/modules%21field%21theme%21field.tpl.php/7.x
   */
  public function buildContent(
    $entity,
    $view_mode = 'full',
    $langcode = NULL,
    $content = array()
  ) {

    $content['machine_name'] = array(
      '#weight'             => 1,
      '#theme'              => 'field',
      '#title'              => t('Machine name'),
      '#access'             => TRUE,
      '#label_display'      => 'inline',
      '#view_mode'          => 'full',
      '#language'           => LANGUAGE_NONE,
      '#field_translatable' => TRUE,
      '#field_name'         => 'machine_name',
      '#field_type'         => 'text',
      '#entity_type'        => 'tripal_cvb',
      '#bundle'             => 'tripal_cvb',
      '#items'              => array(array('value' => $entity->machine_name)),
      '#formatter'          => 'text_default',
      0                     => array(
        '#markup' => check_plain($entity->machine_name),
      ),
    );

    $content['description'] = array(
      '#weight'             => 2,
      '#theme'              => 'field',
      '#title'              => t('Description'),
      '#access'             => TRUE,
      '#label_display'      => 'above',
      '#view_mode'          => 'full',
      '#language'           => LANGUAGE_NONE,
      '#field_translatable' => TRUE,
      '#field_name'         => 'description',
      '#field_type'         => 'text',
      '#entity_type'        => 'tripal_cvb',
      '#bundle'             => 'tripal_cvb',
      '#items'              => array(array('value' => $entity->description)),
      '#formatter'          => 'text_default',
      0                     => array(
        '#markup' => check_plain($entity->description),
      ),
    );

    $root_types = array(
      'cv' => 'CV',
      'cvterm' => 'CV term',
    );
    $content['root_type'] = array(
      '#weight'             => 3,
      '#theme'              => 'field',
      '#title'              => t('Root type'),
      '#access'             => TRUE,
      '#label_display'      => 'inline',
      '#view_mode'          => 'full',
      '#language'           => LANGUAGE_NONE,
      '#field_translatable' => TRUE,
      '#field_name'         => 'root_type',
      '#field_type'         => 'text',
      '#entity_type'        => 'tripal_cvb',
      '#bundle'             => 'tripal_cvb',
      '#items'              => array(array('value' => $entity->root_type)),
      '#formatter'          => 'text_default',
      0                     => array(
        '#markup' => check_plain($root_types[$entity->root_type]),
      ),
    );

    $content['root_ids'] = array(
      '#weight'             => 4,
      '#theme'              => 'field',
      '#title'              => t('Root identifiers'),
      '#access'             => TRUE,
      '#label_display'      => 'inline',
      '#view_mode'          => 'full',
      '#language'           => LANGUAGE_NONE,
      '#field_translatable' => TRUE,
      '#field_name'         => 'root_ids',
      '#field_type'         => 'text',
      '#entity_type'        => 'tripal_cvb',
      '#bundle'             => 'tripal_cvb',
      '#items'              => array(array('value' => $entity->root_ids)),
      '#formatter'          => 'text_default',
      0                     => array(
        '#markup' => check_plain($entity->root_ids),
      ),
    );

    $content['show_cv'] = array(
      '#weight'             => 4,
      '#theme'              => 'field',
      '#title'              => t('Show CV'),
      '#access'             => TRUE,
      '#label_display'      => 'inline',
      '#view_mode'          => 'full',
      '#language'           => LANGUAGE_NONE,
      '#field_translatable' => TRUE,
      '#field_name'         => 'show_cv',
      '#field_type'         => 'text',
      '#entity_type'        => 'tripal_cvb',
      '#bundle'             => 'tripal_cvb',
      '#items'              => array(array('value' => $entity->show_cv)),
      '#formatter'          => 'text_default',
      0                     => array(
        '#markup' => check_plain($entity->show_cv),
      ),
    );

    $content['show_relationships'] = array(
      '#weight'             => 5,
      '#theme'              => 'field',
      '#title'              => t('Show relationships'),
      '#access'             => TRUE,
      '#label_display'      => 'inline',
      '#view_mode'          => 'full',
      '#language'           => LANGUAGE_NONE,
      '#field_translatable' => TRUE,
      '#field_name'         => 'show_relationships',
      '#field_type'         => 'text',
      '#entity_type'        => 'tripal_cvb',
      '#bundle'             => 'tripal_cvb',
      '#items'              => array(array('value' => $entity->show_relationships)),
      '#formatter'          => 'text_default',
      0                     => array(
        '#markup' => check_plain($entity->show_relationships),
      ),
    );

    return parent::buildContent($entity, $view_mode, $langcode, $content);
  }

  /**
   * Loads CV browser.
   */
  public function load($ids = FALSE, $conditions = array()) {
    $browser = parent::load($ids, $conditions);
    return $browser;
  }

  /**
   * Saves a CV browser data.
   */
  public function save($browser, DatabaseTransaction $transaction = NULL) {
    $return_value = NULL;
    $return_value = parent::save($browser, $transaction);

    // Update path alias.
    if ($return_value) {
      // Get connection URI.
      $browser_uri = 'tripal_cvb/' . $browser->cvbid;
      // Get previous alias if one.
      $old_path = path_load(
        array('source' => $browser_uri)
      );
      // And remove it.
      if ($old_path) {
        path_delete($old_path['pid']);
      }
      // Add new alias.
      $alias =
        'tripal-cvb-'
        . $browser->machine_name;
      $path_alias = array(
        'source' => $browser_uri,
        'alias' => $alias,
      );
      path_save($path_alias);
    }

    return $return_value;
  }

  /**
   * Removes connections and their associated data.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    // Remove previous path aliases.
    foreach ($ids as $id) {
      $browser_uri = 'tripal_cvb/' . $id;
      // Get previous alias if one.
      $old_path = path_load(
        array('source' => $browser_uri)
      );
      // And remove it.
      if ($old_path) {
        path_delete($old_path['pid']);
      }
    }
    return parent::delete($ids, $transaction);
  }

}
