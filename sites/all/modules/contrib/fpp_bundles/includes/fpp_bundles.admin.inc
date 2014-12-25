<?php
/**
 * @file
 * Admin functions.
 */

/**
 * Form for add/edit the FPP bundle.
 *
 * @param string $action
 *   An action that been performed.
 * @param int $bid
 *   The bundle ID.
 *
 * @see fpp_bundles()
 *
 * @return array
 *   An array of form items.
 */
function _fpp_bundles_manage_bundle($action, $bid = NULL) {
  if ($action == FPP_BUNDLES_CREATE_BUNDLE_ACTION) {
    $bundle = new \FppBundle();

    if (isset($bid)) {
      drupal_goto(_fpp_bundles_get_menu_path($action));
    }
  }
  else {
    $bundle = fpp_bundles_db()->getBundleByDbIndex('primary', $bid);
  }

  if (!$bundle) {
    drupal_goto(FPP_BUNDLES_ADMIN_URL);
  }

  return drupal_get_form(__FUNCTION__ . '_form', $bundle, $action, drupal_set_title(_fpp_bundles_get_action_title($action)));
}

/**
 * Callback function for check bundles categories.
 *
 * @param string $string
 *   User input.
 */
function _fpp_bundles_autocomplete_category($string) {
  $matches = array();
  $result = fpp_bundles_db()
    ->bundleSelectQuery()
    ->condition('category', "%$string%", 'like')
    ->range(0, 15)
    ->execute();

  foreach ($result as $row) {
    $matches[$row->category] = $row->category;
  }

  drupal_json_output($matches);
}

/**
 * Settings form for the bundles actions.
 *
 * @param array $form
 *   Form items and configuration.
 * @param array $form_state
 *   Information about current state of the form.
 * @param \FppBundle $bundle
 *   Bundle object.
 * @param string $action
 *   Performing action.
 * @param string $page_title
 *   Human readable page name.
 *
 * @see _fpp_bundles_manage_bundle()
 *
 * @return array
 *   Drupal form.
 */
function _fpp_bundles_manage_bundle_form(array $form, array &$form_state, \FppBundle $bundle, $action, $page_title) {
  $machine_name = $bundle->getMachineName();
  $bundle_name = $bundle->getHumanName();
  $items = array();

  // Create hidden fields.
  foreach (array(
    'bid' => $bundle->getBundleId(),
    'action' => $action,
  ) as $name => $value) {
    $items[$name] = array(
      '#type' => 'hidden',
      '#default_value' => $value,
    );
  }

  $items['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name of the bundle'),
    '#required' => TRUE,
    '#default_value' => $bundle_name,
  );

  $items['machine'] = array(
    '#type' => 'machine_name',
    '#disabled' => (bool) $machine_name,
    '#maxlength' => 32,
    '#default_value' => $machine_name,
    '#machine_name' => array(
      'exists' => _fpp_bundles_callback_prefix('check_machine_name_existence'),
    ),
  );

  $items['category'] = array(
    '#type' => 'textfield',
    '#title' => t('Bundle category'),
    '#default_value' => $bundle->getCategoryName(),
    '#autocomplete_path' => _fpp_bundles_get_menu_path('autocomplete category'),
  );

  $items['level'] = array(
    '#type' => 'checkbox',
    '#title' => t('An entity will be shown in the general list.'),
    '#description' => '',
    '#default_value' => $bundle->isTopLevelPanel(),
  );

  $form['actions'] = array(
    '#type' => 'actions',
  );

  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => $page_title,
  );

  if ($action == FPP_BUNDLES_REMOVE_BUNDLE_ACTION) {
    $items = array_map('hide', $items);
    $form['warning'] = array(
      '#markup' => t('Do you really want to remove the "!name" FPP bundle? This action can not to be undone!', array(
        '!name' => "<b>$bundle_name</b>",
      )),
    );
  }

  return $items + $form;
}

/**
 * Drupal machine name validation callback.
 *
 * @param string $machine_name
 *   Generated machine name.
 *
 * @return bool
 *   Status of the name existence.
 */
function _fpp_bundles_check_machine_name_existence($machine_name) {
  if (isset(fpp_bundles_get_fieldable_panels_pane_entity()->bundles[$machine_name])) {
    return TRUE;
  }

  return FALSE;
}

/**
 * Validation handler for _fpp_bundles_manage_bundle_form().
 */
function _fpp_bundles_manage_bundle_form_validate(array $form, array &$form_state) {
  $values =& $form_state['values'];

  // Validation is necessary only when the data can be updated,
  // namely on "create" and "update" actions.
  if ($values['action'] != FPP_BUNDLES_REMOVE_BUNDLE_ACTION) {
    foreach (array('name', 'category') as $field_name) {
      $values[$field_name] = trim(filter_xss($values[$field_name]));
    }

    if (empty($values['name'])) {
      form_set_error('name', t('The name of bundle should not be empty and can contain only latin letters.'));
    }
    elseif (!strlen(preg_replace('/\d+|\s+/', '', $values['name']))) {
      form_set_error('name', t('The name of bundle should not contain only numbers.'));
    }
    elseif (strlen($values['name']) < FPP_BUNDLES_MINIMUM_CHARS_FOR_NAME) {
      form_set_error('name', t('The name of bundle should contain more than !number symbols.', array(
        '!number' => FPP_BUNDLES_MINIMUM_CHARS_FOR_NAME,
      )));
    }
  }

  try {
    $form_state['bundle'] = fpp_bundles_db()->prepareBundleData($values);
  }
  catch (\FppBundlesException $e) {
    form_set_error('name', $e->getMessage());
  }
}

/**
 * Submit handler for _fpp_bundles_manage_bundle_form().
 */
function _fpp_bundles_manage_bundle_form_submit(array $form, array &$form_state) {
  $values = $form_state['values'];
  // @todo Remove all fields of the bundle together with it.
  $status = fpp_bundles_db()->performAction($values['action'], $form_state['bundle']);
  $message = t('Cannot complete the "!action" operation.', array(
    '!action' => $values['action'],
  ));

  if ($status) {
    _fpp_bundles_clear_caches();

    $message = _fpp_bundles_get_action_success_message_by_name(
      $values['action'],
      $values['name']
    );
  }

  drupal_set_message($message);

  $form_state['redirect'] = FPP_BUNDLES_ADMIN_URL;
}
