<?php

/**
 * @file
 * CV browser user interface controller class definition.
 */

/**
 * User interface controller for CV browser.
 */
class TripalCVBrowserUIController extends EntityDefaultUIController {

  /**
   * Returns an overview table of available CV browsers.
   *
   * @return string
   *   A rendered (HTML) CV browser overview table.
   */
  public function overviewTable($conditions = array()) {
    drupal_add_css(drupal_get_path('module', 'tripal_cvb') . '/theme/css/tripal_cvb.css');
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $this->entityType);

    // Add all conditions to query.
    foreach ($conditions as $key => $value) {
      $query->propertyCondition($key, $value);
    }

    if ($this->overviewPagerLimit) {
      $query->pager($this->overviewPagerLimit);
    }
    $results = $query->execute();

    if (!empty($results)) {
      // Load data about each connection.
      $entities = entity_load('tripal_cvb', array_keys($results['tripal_cvb']));

      // Create the table.
      $rows = array();
      foreach ($entities as $cvbid => $entity) {

        $title = l($entity->title, 'tripal_cvb/' . $entity->cvbid);
        $operations =
          '<ul class="links inline"><li class="view first">'
          . l(t('view'), 'tripal/cvbrowser/browser/' . $entity->machine_name)
          . '</li><li class="edit">'
          . l(t('edit'), 'tripal_cvb/' . $entity->cvbid . '/edit')
          . '</li><li class="delete last">'
          . l(t('delete'), 'tripal_cvb/' . $entity->cvbid . '/delete')
          . '</li></ul>';
        $rows[$cvbid] = array(
          'title'              => $title,
          'root_type'          => $entity->root_type,
          'root_ids'           => $entity->root_ids,
          'show_cv'            => $entity->show_cv ? t('yes') : t('no'),
          'show_relationships' => $entity->show_relationships ? t('yes') : t('no'),
          'operations'         => $operations,
        );
      }

      return theme(
        'table',
        array(
          'header' =>
            array(
              'title'              => t('Name'),
              'root_type'          => t('Root type'),
              'root_ids'           => t('Root identifiers'),
              'show_cv'            => t('Display CV'),
              'show_relationships' => t('Display relationships'),
              'operations'         => t('Operations'),
            ),
          'rows' => $rows,
          'attributes' => array(
            'class' => array('tripal-cvb-browser-list'),
          ),
        )
      );
    }
    else {
      return '';
    }
  }

}
