<?php

/**
 * @file
 * Administration forms.
 *
 * Contains the functions used for administration of the Tripal CV Browser
 * module.
 *
 * @ingroup tripal_cvb
 */

/**
 * Administrative settings form.
 *
 * @return string
 *   the HTML content of the administration form.
 *
 * @ingroup tripal_cvb
 */
function tripal_cvb_admin_form($form, &$form_state) {
  $form = array();

  $settings = tripal_cvb_get_settings();

  // Check jQuery version.
  $jquery_version = variable_get('jquery_update_jquery_version', 0);
  if (!version_compare($jquery_version, 1.7, '>=')) {
    if (!$jquery_version) {
      $jquery_version = t('Drupal default');
    }
    drupal_set_message(
      'Tripal CV browser module requires a version of jQuery >= 1.7 while current enabled version of jQuery is ' . $jquery_version . '. Please setup a higher version of jQuery ('
      . l(
        t('jQuery update module settings'),
        'admin/config/development/jquery_update')
      . ') in order to have the browser properly working.',
      'warning');
  }

  // CV settings.
  $form['cvb_settings'] = array(
    '#type'        => 'fieldset',
    '#attributes'  => array('class' => array('container')),
    '#title'       => t('Global CV browser settings'),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );

  $form['cvb_settings']['show_cv'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Display CV name(s) as browser root(s)'),
    '#default_value' => $settings['show_cv'],
    '#required'      => 0,
  );

  $form['cvb_settings']['show_relationships'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Display standard relationships with parent (is_a, part_of, ...) in front of terms'),
    '#default_value' => $settings['show_relationships'],
    '#required'      => 0,
  );

  $form['save_settings'] = array(
    '#type'        => 'submit',
    '#value'       => t('Save configuration'),
    '#submit'      => array('tripal_cvb_admin_form_submit'),
  );

  $form['cvb_list'] = array(
    '#type'        => 'fieldset',
    '#attributes'  => array('class' => array('container')),
    '#title'       => t('Available CV browsers'),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );

  // Displays "add" link.
  $form['cvb_list']['cvb_add'] = array(
    '#markup' => l(t('+ Create a new CV Browser'), 'tripal_cvb/add'),
  );

  // Display list.
  $form['cvb_list']['cvbrowser_list'] = array(
    '#markup' => entity_ui_controller('tripal_cvb')->overviewTable(),
  );

  return $form;
}

/**
 * Check and applies CV browser configuration changes.
 *
 * @ingroup tripal_cvb
 */
function tripal_cvb_admin_form_submit($form_id, &$form_state) {

  $settings = tripal_cvb_get_settings();
  // Display CV.
  if (isset($form_state['values']['show_cv'])) {
    $settings['show_cv'] = $form_state['values']['show_cv'];
  }

  // Display relationships.
  if (isset($form_state['values']['show_relationships'])) {
    $settings['show_relationships'] = $form_state['values']['show_relationships'];
  }

  tripal_cvb_set_settings($settings);
}

/**
 * Returns the CV Browser add form.
 *
 * Callback for hook_menu().
 *
 * @return array
 *   The form structure.
 *
 * @ingroup callbacks
 */
function tripal_cvb_admin_add_page() {
  global $user;
  $browser = entity_create(
    'tripal_cvb',
    array(
      'cvbid' => '',
      'title' => 'CV Browser',
      'machine_name' => '',
      'description' => '',
      'root_type' => 'cvterm',
      'root_ids' => '',
      'show_cv' => 1,
      'show_relationships' => 1,
      'uid' => $user->uid,
      'type' => 'tripal_cvb',
    )
  );

  return drupal_get_form('tripal_cvb_form', $browser);
}

/**
 * Form constructor for the CV Browser add/edit form.
 *
 * @param array $form
 *   A Drupal form structure.
 * @param array $form_state
 *   A Drupal form state structure.
 * @param object $browser
 *   A CV Browser object (TripalCVBrowser).
 *
 * @see tripal_cvb_form_validate()
 * @see tripal_cvb_form_submit()
 * @see tripal_cvb_form_submit_delete()
 *
 * @ingroup tripal_cvb
 */
function tripal_cvb_form(array $form, array &$form_state, $browser) {

  $form_state['cvbrowser'] = $browser;

  // This is necessary for field_default_form() not to yell at us.
  $form['#parents'] = array();

  // These variables are needed to build field forms.
  // Substitute with any entity type.
  $entity_type = 'tripal_cvb';
  // Substitute with the entity's bundle.
  $bundle_name = 'tripal_cvb';
  // Not really necessary but we keep the parameter names consistent with the
  // field_default_form() signature.
  $entity = $browser;
  // Substitute as appropriate.
  $langcode = LANGUAGE_NONE;

  // $items is used to provide default values for the field. You can set it to
  // NULL or an empty array if desired.
  $items = field_get_items($entity_type, $entity, 'cvterm_action');

  // We need the field and field instance information to pass along to
  // field_default_form().
  $field = field_info_field('cvterm_action');
  $instance = field_info_instance(
    $entity_type,
    'cvterm_action',
    $bundle_name
  );

  // This is where the cool stuff happens.
  $field_form = field_default_form(
    $entity_type,
    $entity,
    $field,
    $instance,
    $langcode,
    $items,
    $form,
    $form_state
  );

  // Machine name stuff.
  $machine_name_pattern = '[^0-9a-zA-Z-]+';
  $machine_name_replacement = '-';

  $form = array(
    'title' => array(
      '#type'          => 'textfield',
      '#title'         => t('Name of the CV Browser'),
      '#description'   => t('This label is for administrative purpose and will be used to identify the following browser settings on administration interfaces.'),
      '#required'      => TRUE,
      '#default_value' => $browser->title,
      '#field_suffix'  => '<br/>',
      '#weight'        => 0,
    ),
    'machine_name' => array(
      '#type' => 'machine_name',
      '#default_value' => $browser->machine_name,
      '#maxlength' => 16,
      '#machine_name' => array(
        'exists' => 'tripal_cvb_name_exists',
        'source' => array('title'),
        'label' => t('CV Browser machine name (unique identifier)'),
        'replace_pattern' => $machine_name_pattern,
        'replace' => $machine_name_replacement,
      ),
      '#required'         => TRUE,
      '#element_validate' => array('tripal_cvb_machine_name_validate'),
      '#weight'           => 1,
    ),
    'root_type' => array(
      '#type'          => 'select',
      '#title'         => t('Root object type'),
      '#description'   => t('Type of the root items which identifiers are specified below.'),
      '#required'      => TRUE,
      '#options'       => array(
        'cvterm' => t('CV Term'),
        'cv'     => t('CV'),
      ),
      '#default_value' => $browser->root_type,
      '#weight'        => 2,
    ),
    'root_ids' => array(
      '#type'          => 'textfield',
      '#title'         => t('Root object identifiers'),
      '#description'   => t('Identifiers of CV or CV term object to use as tree browser root. Multiple identifiers must be separated by "+" signs.'),
      '#required'      => TRUE,
      '#default_value' => $browser->root_ids,
      '#weight'        => 3,
    ),
    'show_cv' => array(
      '#type'          => 'checkbox',
      '#title'         => t('Display CV on browser page.'),
      '#description'   => t('Displays the CV name(s) on the browser page as tree root(s).'),
      '#required'      => FALSE,
      '#default_value' => $browser->show_cv,
      '#weight'        => 4,
    ),
    'show_relationships' => array(
      '#type'          => 'checkbox',
      '#title'         => t('Display relationship types on browser page.'),
      '#description'   => t('Displays the relationship types between a child and its parent (ie. "is_a", "part of", ...).'),
      '#required'      => FALSE,
      '#default_value' => $browser->show_relationships,
      '#weight'        => 5,
    ),
    'description' => array(
      '#type'          => 'textarea',
      '#title'         => t('CV Browser description'),
      '#description'   => t('This field is for administration purpose only.'),
      '#required'      => FALSE,
      '#default_value' => $browser->description,
      '#weight'        => 6,
    ),
    'uid' => array(
      '#type' => 'value',
      '#value' => $browser->uid,
      '#weight'        => 8,
    ),
  );

  // Adding it to the form.
  $form += $field_form;

  field_attach_form(
    'tripal_cvb',
    $browser,
    $form,
    $form_state
  );

  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type'     => 'submit',
    '#value'    => t('Save'),
    '#submit'   => $submit + array('tripal_cvb_form_submit'),
  );

  // Show Delete button if we edit a CV Browser.
  $browser_id = entity_id('tripal_cvb', $browser);
  if (!empty($browser_id)
      && tripal_cvb_access('edit', $browser)) {
    $form['actions']['delete'] = array(
      '#type'   => 'submit',
      '#value'  => t('Delete'),
      '#submit' => array('tripal_cvb_form_submit_delete'),
    );
  }

  return $form;
}

/**
 * CV Browser machine_name validation handler.
 */
function tripal_cvb_machine_name_validate($element, &$form_state, $form) {
  if (empty($element['#value'])) {
    form_error($element, t('The machine name field is required.'));
  }
  elseif (16 < strlen($element['#value'])) {
    form_error($element, t('The machine name field must be non longer than 16 characters.'));
  }
}

/**
 * CV Browser submit handler.
 */
function tripal_cvb_form_submit($form, &$form_state) {
  $browser = $form_state['cvbrowser'];
  entity_form_submit_build_entity('tripal_cvb', $browser, $form, $form_state);

  tripal_cvb_save($browser);

  $browser_uri = entity_uri(
    'tripal_cvb',
    $browser
  );

  $form_state['redirect'] = $browser_uri['path'];

  drupal_set_message(
    t(
      'CV Browser %title saved.',
      array('%title' => entity_label('tripal_cvb', $browser))
    )
  );
}

/**
 * Sends to the CV Browser delete form.
 */
function tripal_cvb_form_submit_delete($form, &$form_state) {
  $browser = $form_state['cvbrowser'];
  $browser_uri = entity_uri(
    'tripal_cvb',
    $browser
  );
  $form_state['redirect'] = $browser_uri['path'] . '/delete';
}

/**
 * Delete confirmation form.
 */
function tripal_cvb_delete_form($form, &$form_state, $browser) {
  $form_state['cvbrowser'] = $browser;
  // Always provide entity id in the same form key as in the entity edit form.
  $browser_uri = entity_uri(
    'tripal_cvb',
    $browser
  );
  return confirm_form(
    $form,
    t(
      'Are you sure you want to delete CV Browser %title?',
      array(
        '%title' => entity_label(
          'tripal_cvb',
          $browser
        ),
      )
    ),
    $browser_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function tripal_cvb_delete_form_submit($form, &$form_state) {
  $browser = $form_state['cvbrowser'];
  tripal_cvb_delete($browser);

  drupal_set_message(
    t(
      'CV Browser %title deleted.',
      array(
        '%title' => entity_label(
          'tripal_cvb',
          $browser
        ),
      )
    )
  );

  $form_state['redirect'] = '<front>';
}

/**
 * Access callback for CV browser interface.
 */
function tripal_cvb_permission_access($roles) {

  foreach ($roles as $role) {
    if (user_access($role)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * Access callback for CV browser entities.
 */
function tripal_cvb_access($op, $browser, $account = NULL, $entity_type = NULL) {
  global $user;

  if (!isset($account)) {
    $account = $user;
  }

  switch ($op) {
    case 'create':
      return user_access('administer tripal cv browser', $account)
        || user_access('administer tripal_cvb entities', $account)
        || user_access('create tripal_cvb entities', $account);

    case 'view':
      return user_access('administer tripal cv browser', $account)
        || user_access('administer tripal_cvb entities', $account)
        || user_access('view any tripal_cvb entity', $account);

    case 'edit':
    case 'update':
      return user_access('administer tripal cv browser')
        || user_access('administer tripal_cvb entities', $account)
        || user_access('edit any tripal_cvb entity');

    case 'delete':
      return user_access('administer tripal cv browser')
        || user_access('administer tripal_cvb entities', $account)
        || user_access('edit any tripal_cvb entity');
  }
}

/**
 * Callback to check if a browser machine name is already in use.
 */
function tripal_cvb_name_exists($value) {
  $name_exists = db_query_range(
    'SELECT 1 FROM {tripal_cvb} WHERE machine_name = :name',
    0,
    1,
    array(':name' => $value)
  )->fetchField();

  return $name_exists;
}
