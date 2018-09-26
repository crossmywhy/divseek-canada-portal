<?php

/**
 * @file
 * API functions provided by Tripal CV Browser module.
 *
 * Provides an application programming interface (API) for working with Tripal
 * CV Borwser.
 *
 * @ingroup tripal_cvb
 */

/**
 * @defgroup tripal_cvb_api API of Tripal CV Browser module
 * @ingroup tripal_cvb
 * @{
 * Provides an application programming interface (API) for working with Tripal
 * CV Borwser.
 * @}
 */

/**
 * Returns default settings.
 *
 * Returns an array containing the default settings used by this Tripal CV
 * browser installation.
 *
 * @return array
 *   The Tripal CV browser default setting array. These settings may differ from
 *   those ones of current configuration.
 *
 * @see tripal_cvb_get_settings()
 * @ingroup tripal_cvb_api
 */
function tripal_cvb_get_default_settings() {
  return array(
    'show_cv' => TRUE,
    'show_relationships' => TRUE,
  );
}

/**
 * Returns CV settings.
 *
 * Returns an array containing the settings used by this Tripal CV Browser
 * installation.
 *
 * @param bool $reset
 *   Clear current settings and reload them from database.
 *
 * @return array
 *   Current Tripal CV browser setting array.
 *
 * @ingroup tripal_cvb_api
 */
function tripal_cvb_get_settings($reset = FALSE) {
  static $settings;
  // If not initialized, get it from cache if available.
  if (!isset($settings) || $reset) {
    if (!$reset
        && ($cache = cache_get('tripal_cvb_settings'))
        && !empty($cache->data)) {
      $settings = $cache->data;
    }
    else {
      // Not available in cache, get it from saved settings or defaults.
      $settings = variable_get(
        'tripal_cvb_settings',
        tripal_cvb_get_default_settings()
      );
      cache_set('tripal_cvb_settings', $settings);
    }
  }

  return $settings;
}

/**
 * Saves CV settings.
 *
 * Saves the CV settings used by this Tripal CV Browser installation.
 *
 * @param array $settings
 *   A new setting array.
 *
 * @ingroup tripal_cvb_api
 */
function tripal_cvb_set_settings($settings = array()) {
  // Set default settings if some are missing.
  $settings += tripal_cvb_get_default_settings();
  $settings = variable_set('tripal_cvb_settings', $settings);

  // Reset cache and static variable.
  $settings = tripal_cvb_get_settings(TRUE);
}

/**
 * Returns a list of children CV terms of the given CV term.
 *
 * Outputs a JSON hash containing CV term children data associated by cvterm_id.
 * Provided cvterm fields are:
 *  * cvterm_id: Chado CV term cvterm_id;
 *  * cv_id: Chado CV term cv_id;
 *  * cv: CV term CV name;
 *  * name: CV term name;
 *  * definition: CV term definition;
 *  * dbxref_id: Chado CV term dbxref_id;
 *  * dbxref: Chado CV term accession on its database;
 *  * db: Chado CV term database name;
 *  * urlprefix: URL prefix to access term description on its associated
 *    database (append the accession to access term);
 *  * is_obsolete: if non-0 ("1"), term is considered as obsolete;
 *  * is_relationshiptype: if non-0 ("1"), term is considered as qualifying a
 *    relationship;
 *  * relationship: relationship (name) with parent;
 *  * children_count: number of children terms.
 *
 * @param int $cvterm_id
 *   a Chado CV term identifier.
 */
function tripal_cvb_get_cvterm_info_json($cvterm_id) {

  $cvterm_data = chado_select_record(
    'cvterm',
    array('*'),
    array(
      'cvterm_id' => $cvterm_id,
    )
  );

  if (is_array($cvterm_data)) {
    $cvterm_data = current($cvterm_data);
  }

  drupal_json_output($cvterm_data);
}

/**
 * Returns a list of children CV terms of the given CV term.
 *
 * @param int $cvterm_id
 *   a Chado CV term identifier.
 *
 * @return array
 *   an array of CVTerms.
 */
function tripal_cvb_get_cvterm_children($cvterm_id) {
  $cvterm_data = array();

  // @see tripal_cvb_browser_render()
  $sql_query = '
    SELECT
      cvt.cvterm_id AS "cvterm_id",
      cvt.cv_id AS "cv_id",
      cv.name AS "cv",
      cvt.name AS "name",
      cvt.definition AS "definition",
      cvt.dbxref_id AS "dbxref_id",
      dbx.accession AS "dbxref",
      db.name AS "db",
      db.urlprefix AS "urlprefix",
      cvt.is_obsolete AS "is_obsolete",
      cvt.is_relationshiptype AS "is_relationshiptype",
      cvtrcvt.name AS "relationship",
      (SELECT COUNT(1)
       FROM {cvterm_relationship} cvtr2
       WHERE cvtr2.object_id = cvtr.subject_id) AS "children_count"
    FROM {cvterm} cvt
      JOIN {cvterm_relationship} cvtr ON cvtr.subject_id = cvt.cvterm_id
      JOIN {cvterm} cvtrcvt ON cvtr.type_id = cvtrcvt.cvterm_id
      JOIN {cv} cv ON cv.cv_id = cvt.cv_id
      JOIN {dbxref} dbx ON dbx.dbxref_id = cvt.dbxref_id
      JOIN {db} db ON db.db_id = dbx.db_id
    WHERE cvtr.object_id = :object_cvterm_id
    ORDER BY cvt.name ASC
  ';
  $relationship_records = chado_query(
    $sql_query,
    array(':object_cvterm_id' => $cvterm_id)
  );

  foreach ($relationship_records as $relationship) {
    $cvterm_data[$relationship->cvterm_id] = $relationship;
  }

  return $cvterm_data;
}

/**
 * Returns a list of children CV terms of the given CV term.
 *
 * Outputs a JSON hash containing CV term children data associated by cvterm_id.
 *
 * @param int $cvterm_id
 *   a Chado CV term identifier.
 */
function tripal_cvb_get_cvterm_children_json($cvterm_id) {
  drupal_json_output(tripal_cvb_get_cvterm_children($cvterm_id));
}

/**
 * Renders the CV Browser page.
 *
 * @param string $browser_type
 *   The type of object to browse. Supported types are 'cv' and 'cvterm'.
 * @param mixed $root_ids
 *   Single or multiple (array) Chado identifiers for the type of object
 *   to browse.
 *
 * @return string
 *   The CV Browser page.
 *
 * @throws Exception
 *   Throw an exception is the value type is not correct.
 */
function tripal_cvb_cv_render($browser_type, $root_ids) {

  $settings = tripal_cvb_get_settings();

  if (!isset($browser_type)) {
    $browser_type = 'cv';
  }
  if (!isset($root_ids)) {
    $root_ids = '';
  }

  if ('browser' == $browser_type) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'tripal_cvb');
    $query->propertyCondition('machine_name', $root_ids);
    $results = $query->execute();
    if (!empty($results)) {
      $entities = entity_load('tripal_cvb', array(key($results['tripal_cvb'])));
      $browser = current($entities);
    }
  }
  else {
    $browser = entity_create(
      'tripal_cvb',
      array(
        'root_type' => $browser_type,
        'root_ids' => $root_ids,
        'show_cv' => $settings['show_cv'],
        'show_relationships' => $settings['show_relationships'],
      )
    );
  }

  if (!isset($browser)) {
    $browser = entity_create('tripal_cvb', array());
  }

  return tripal_cvb_browser_render($browser);
}

/**
 * Renders the CV Browser page.
 *
 * @param TripalCVBrowser $browser
 *   A CV Browse object (tripal_cvb) to render.
 *
 * @return string
 *   The CV Browser page.
 *
 * @throws Exception
 *   Throw an exception is the value type is not correct.
 */
function tripal_cvb_browser_render(TripalCVBrowser $browser) {
  $root_ids = $browser->root_ids;

  if (!isset($root_ids)) {
    $root_ids = array(0);
  }
  if (!is_array($root_ids)) {
    $root_ids = str_replace(array('%2B', '%2b'), '+', $root_ids);
    $root_ids = explode('+', $root_ids);
  }

  // Separate litteral names and numeric identifiers.
  $selected_ids = array();
  $selected_names = array();
  foreach ($root_ids as $id) {
    if (preg_match('/^\d+$/', $id)) {
      $selected_ids[] = $id;
    }
    else {
      $selected_names[] = $id;
    }
  }

  // Make sure we got something.
  if (empty($selected_ids) && empty($selected_names)) {
    throw new Exception(t(
      "No identifier specified!"
    ));
  }

  if (!empty($selected_names) && !empty($selected_ids)) {
    drupal_set_message('Mixing names and identifiers will only retain objects matching both.', 'warning');
  }

  // Initialize stuff.
  $cv_terms = array();
  $where_clause = array();
  $values = array();

  // Check data type to build query.
  switch ($browser->root_type) {
    case 'cv':
      // We only want root terms of the given CVs.
      $where_clause[] = 'NOT EXISTS (
          SELECT TRUE
          FROM {cvterm_relationship} cvtr
            JOIN {cvterm} pcvt ON pcvt.cvterm_id = cvtr.object_id
          WHERE cvtr.subject_id = cvt.cvterm_id AND pcvt.cv_id = cvt.cv_id
          LIMIT 1
        )';
      if (!empty($selected_names)) {
        $where_clause[] = 'cv.name IN (:cv_names)';
        $values[':cv_names'] = $selected_names;
      }
      if (!empty($selected_ids)) {
        $where_clause[] = 'cvt.cv_id IN (:cv_ids)';
        $values[':cv_ids'] = $selected_ids;
      }
      break;

    case 'cvterm':
      if (!empty($selected_names)) {
        $where_clause[] = 'cvt.name IN (:cvterm_names)';
        $values[':cvterm_names'] = $selected_names;
      }
      if (!empty($selected_ids)) {
        $where_clause[] = 'cvt.cvterm_id IN (:cvterm_ids)';
        $values[':cvterm_ids'] = $selected_ids;
      }
      break;

    default:
      throw new Exception(t(
        "Unsupported object type @type!",
        array(
          '@type' => check_plain($browser->root_type),
        )
      ));
  }

  if ($browser->show_cv) {
    $order_by = ' ORDER BY cv.name ASC, cvt.name ASC';
  }
  else {
    $order_by = ' ORDER BY cvt.name ASC';
  }

  // @see tripal_cvb_get_cvterm_children()
  $sql_query = '
    SELECT
      cvt.cvterm_id AS "cvterm_id",
      cvt.cv_id AS "cv_id",
      cv.name AS "cv",
      cvt.name AS "name",
      cvt.definition AS "definition",
      cvt.dbxref_id AS "dbxref_id",
      dbx.accession AS "dbxref",
      db.name AS "db",
      db.urlprefix AS "urlprefix",
      cvt.is_obsolete AS "is_obsolete",
      cvt.is_relationshiptype AS "is_relationshiptype",
      NULL AS "relationship",
      (SELECT COUNT(1)
       FROM {cvterm_relationship} cvtr2
       WHERE cvtr2.object_id = cvt.cvterm_id) AS "children_count"
    FROM {cvterm} cvt
      JOIN {cv} cv ON cv.cv_id = cvt.cv_id
      JOIN {dbxref} dbx ON dbx.dbxref_id = cvt.dbxref_id
      JOIN {db} db ON db.db_id = dbx.db_id'
    . (empty($where_clause) ? '' : ' WHERE ')
    . implode(' AND ', $where_clause)
    . $order_by;

  $term_records = chado_query(
    $sql_query,
    $values
  );

  // Outputs a 404 for global browser without results.
  if (!$term_records->rowCount() && !$browser->cvbid) {
    switch ($browser->root_type) {
      case 'cv':
        $where_clause = array();
        $values = array();
        if (!empty($selected_names)) {
          $where_clause[] = 'cv.name IN (:cv_names)';
          $values[':cv_names'] = $selected_names;
        }
        if (!empty($selected_ids)) {
          $where_clause[] = 'cv.cv_id IN (:cv_ids)';
          $values[':cv_ids'] = $selected_ids;
        }
        $sql_query = 'SELECT TRUE FROM {cv} cv'
          . (empty($where_clause) ? '' : ' WHERE ')
          . implode(' AND ', $where_clause);
        break;

      case 'cvterm':
        $where_clause = array();
        $values = array();
        if (!empty($selected_names)) {
          $where_clause[] = 'cvt.name IN (:cvterm_names)';
          $values[':cvterm_names'] = $selected_names;
        }
        if (!empty($selected_ids)) {
          $where_clause[] = 'cvt.cvterm_id IN (:cvterm_ids)';
          $values[':cvterm_ids'] = $selected_ids;
        }
        $sql_query = 'SELECT TRUE FROM {cvterm} cvt'
          . (empty($where_clause) ? '' : ' WHERE ')
          . implode(' AND ', $where_clause);
        break;
    }
    $existing = chado_query($sql_query, $values);
    if (!$existing->rowCount()) {
      return MENU_NOT_FOUND;
    }
  }

  // Sort terms by CV.
  $cv_terms = array();
  foreach ($term_records as $term) {
    if (!array_key_exists($term->cv_id, $cv_terms)) {
      $cv_terms[$term->cv_id] = array();
    }
    $cv_terms[$term->cv_id][] = $term;
  }

  // Get actions.
  $actions = array();
  if (isset($browser->cvterm_action)
      && isset($browser->cvterm_action[LANGUAGE_NONE])) {
    $actions = $browser->cvterm_action[LANGUAGE_NONE];
  }

  return theme(
    'tripal_cvbrowser',
    array(
      'cv_terms' => $cv_terms,
      'browser' => $browser,
      'actions' => $actions,
    )
  );
}
