<?php
// $Id: w3c_validator_site.pages.inc,v 1.1.2.4 2009/12/22 10:00:56 pl2 Exp $

/**
 * @file
 * Page callbacks for the w3c site validator module.
 */

/**
 * Main site validator form.
 *
 * @param string $form_state
 * @return void
 */
function w3c_validator_site_content_validator($form_state) {
  $endpoint = variable_get('w3c_validator_api_endpoint_uri', 'http://validator.w3.org/check');
  if (empty($endpoint)) {
    drupal_set_message(t("W3C validator hasn't been configured yet."), 'warning');
    if (user_access('administer w3c_validator')) {
      return array(
        'warning' => array(
          '#value' =>  t('Endpoint must be set before you can use it. Configure W3C Validator at the <a href="!url">settings page</a>', array('!url' => url('admin/settings/w3c_validator'))),
        ),
      );
    }
    else {
      return array();
    }
  }

  $form = array();
  $type_options = array(
    'node' => t('Nodes'),
    );

  if (module_exists('views')) {
    $type_options['view'] = t('Views');
  }

  $form['validate_elements'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Validate elements'),
    '#description' => t('Type of site elements that will be validated.'),
    '#options' => $type_options,
    '#default_value' => array(),
  );

  $content_types = node_get_types('names');
  $form['content_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content types'),
    '#description' => t('Node types that will be validated if node validation is requested. If none is selected all content types will be validated.'),
    '#options' => $content_types,
    '#default_value' => array(),
  );

  $method = variable_get('w3c_validator_method', 'w3c_markup_validator');
  $form['unpublished'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include unpublished nodes'),
    '#description' => t('Only available when using the Tidy library validation method.'),
    '#default_value' => FALSE,
    '#disabled' => $method != 'tidy',
  );


  $form['validate'] = array(
    '#type' => 'submit',
    '#value' => t('Validate'),
  );

  return $form;
}

/**
 * Create result set and set a batch of validation operations.
 *
 * @param string $form
 * @param string $form_state
 * @return void
 */
function w3c_validator_site_content_validator_submit($form, &$form_state) {
  $config = array(
    'validate_elements' => $form_state['values']['validate_elements'],
    'content_types' => $form_state['values']['content_types'],
    );

  // If no content type is selected all will be checked
  if ($config['validate_elements']['node']) {
    $selected = FALSE;
    foreach ($config['content_types'] as $type => $enabled) {
      if ($enabled) {
        $selected = TRUE;
      }
    }
    if (!$selected) {
      foreach ($config['content_types'] as $type => $enabled) {
        $config['content_types'][$type] = TRUE;
      }
    }
    $config['unpublished'] = $form_state['values']['unpublished'];
  }

  $result_set = new stdClass();
  $result_set->date = time();
  $result_set->config = serialize($config);
  $result_set->global_validate = 1;
  drupal_write_record('validator_results_sets', $result_set);
  $result_set->config = unserialize($result_set->config);

  $rsid = $result_set->rsid;

  if (!$rsid) {
    drupal_set_message(t('Error when trying to create result set. Aborting.'));
    return;
  }

  $batch = w3c_validator_site_get_validation_batch($result_set);

  batch_set($batch);
}

/**
 * Main settings form.
 *
 * @param string $form_state
 * @return void
 */
function w3c_validator_settings($form_state) {
  $form = array();

  // TODO: Option to run a complete site validation on cron run and email results.

  return system_settings_form($form);
}

/**
 * Page callback that displays all validation sets.
 *
 * @return void
 */
function w3c_validator_site_result_sets_page() {
  $output = '';


  if (db_result(db_query("SELECT COUNT(*) FROM {validator_results_sets}"))) {
    $output .= drupal_get_form('w3c_validator_site_clean_results');
  }

  $sql = "SELECT * FROM {validator_results_sets} ORDER BY rsid DESC";
  $result = pager_query($sql, 30);

  $headers = array(t('Date'), t('Validation result'), t('URL count'), t('Invalid results'));
  $rows = array();

  while ($row = db_fetch_object($result)) {
    $page_count = db_result(db_query("SELECT COUNT(*) FROM {validator_results_url} WHERE rsid = %d", $row->rsid));
    $invalid_page_count = db_result(db_query("SELECT COUNT(*) FROM {validator_results_url} WHERE rsid = %d AND validity = 0", $row->rsid));

    $rows[] = array(
      'data' => array(
        l(format_date($row->date), 'admin/content/validator/results/'. $row->rsid),
        $row->global_validate == 1 ? t('Valid') : t('Not valid'),
        $page_count,
        $invalid_page_count,
      ),
      'class' => $row->global_validate ? 'ok' : 'error',
    );
  }

  $output .= theme('table', $headers, $rows, array('class' => 'system-status-report'));

  $output .= theme('pager');
  return $output;
}

/**
 * Form to clean all validation results.
 *
 * @param string $form_state
 * @return void
 */
function w3c_validator_site_clean_results($form_state) {
  $form = array();

  $form['clean_all_results'] = array(
    '#type' => 'submit',
    '#value' => t('Clean all results'),
  );

  return $form;
}

/**
 * Clean all validation results.
 *
 * @param string $form
 * @param string $form_state
 * @return void
 */
function w3c_validator_site_clean_results_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == t('Clean all results')) {
    drupal_set_message(t('Cleaning all validation results'));
    db_query("DELETE vrs, vru, vrm
      FROM {validator_results_sets} vrs
      LEFT JOIN {validator_results_url} vru ON vru.rsid = vrs.rsid
      LEFT JOIN {validator_results_messages} vrm ON vrm.urlid = vru.urlid");
  }
}

/**
 * Page callback to display details for a result set.
 *
 * @param string $result_set
 * @return void
 */
function w3c_validator_site_result_set_details_page($result_set) {
  $output = '';

  $output .= '<p><strong>'. t('Date') .':</strong> '. format_date($result_set->date) .'</p>';

  $validate_elements = array();
  foreach ($result_set->config['validate_elements'] as $type => $enabled) {
    if ($enabled) {
      $validate_elements[] = $type;
    }
  }
  $output .= '<p><strong>'. t('Validation type') .':</strong> '. implode(', ', $validate_elements) .'</p>';

  $content_types = array();
  foreach ($result_set->config['content_types'] as $type => $enabled) {
    if ($enabled) {
      $content_types[] = $type;
    }
  }
  $output .= '<p><strong>'. t('Content types') .':</strong> '. implode(', ', $content_types) .'</p>';

  $output .= '<p><strong>'. t('Global validation result') .':</strong> '. ($result_set->global_validate ? t('All valid') : t('Invalid results')) .'</p>';

  $url_count = db_result(db_query("SELECT COUNT(*) FROM {validator_results_url} WHERE rsid = %d", $result_set->rsid));
  $output .= '<p><strong>'. t('URL count') .':</strong> '. sprintf("%01d", $url_count) .'</p>';
  $error_sum = db_result(db_query("SELECT SUM(error_count) FROM {validator_results_url} WHERE rsid = %d", $result_set->rsid));
  $output .= '<p><strong>'. t('Error count') .':</strong> '. sprintf("%01d", $error_sum) .'</p>';
  $warning_sum = db_result(db_query("SELECT SUM(warning_count) FROM {validator_results_url} WHERE rsid = %d", $result_set->rsid));
  $output .= '<p><strong>'. t('Warning count') .':</strong> '. sprintf("%01d", $warning_sum) .'</p>';

  $sql = "SELECT * FROM {validator_results_url} WHERE rsid = %d";
  $result = pager_query($sql, 50, 0, NULL, $result_set->rsid);
  $headers = array(t('URL'), t('Valid'), t('Errors'), t('Warnings'), t('Doctype'));
  $rows = array();

  $output .= drupal_get_form('w3c_validator_site_repeat_validation_set', $result_set);

  while ($row = db_fetch_object($result)) {
    $rows[] = array(
      'data' => array(
        l($row->url, 'admin/content/validator/results/'. $result_set->rsid .'/'. $row->urlid),
        $row->error_count ? t('Not valid') : ($row->warning_count ? t('With warnings') : t('Valid')),
        check_plain($row->error_count),
        check_plain($row->warning_count),
        check_plain($row->doctype),
      ),
      'class' => $row->error_count ? 'error' : ($row->warning_count ? 'warning' : 'ok'),
    );
  }

  $output .= theme('table', $headers, $rows, array('class' => 'system-status-report'));

  $output .= theme('pager');

  // Set a proper breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Results'), 'admin/content/validator/results');
  drupal_set_breadcrumb($bc);

  return $output;
}

/**
 * Form to repeat validation for an entire set.
 *
 * @param string $form_state
 * @return void
 */
function w3c_validator_site_repeat_validation_set($form_state, $result_set) {
  $form = array();

  $form['repeat_this_validation'] = array(
    '#type' => 'submit',
    '#value' => t('Repeat this validation'),
  );

  return $form;
}

/**
 * Create a new result set cloned from the current one and batch process it.
 *
 * @param string $form
 * @param string $form_state
 * @return void
 */
function w3c_validator_site_repeat_validation_set_submit($form, &$form_state) {
  $result_set = $form['#parameters'][2];

  $config = $result_set->config;

  $new_result_set = new stdClass();
  $new_result_set->date = time();
  $new_result_set->config = serialize($config);
  $new_result_set->global_validate = 1;
  drupal_write_record('validator_results_sets', $new_result_set);
  $new_result_set->config = unserialize($new_result_set->config);

  $rsid = $new_result_set->rsid;

  if (!$rsid) {
    drupal_set_message(t('Error when trying to create result set. Aborting.'));
    return;
  }

  $batch = w3c_validator_site_get_validation_batch($new_result_set);

  batch_set($batch);
  batch_process();
}

/**
 * Page callback to display results details for a url.
 *
 * @param string $urlid
 * @return void
 */
function w3c_validator_site_result_url_page($result_url) {
  $output = '';

  drupal_set_title(t('Results for url %url', array('%url' => $result_url->url)));

  $output .= '<p><strong>'. t('URL') .':</strong> '. l($result_url->url, $result_url->url, array('attributes' => array('target' => '_new')))  .'</p>';

  $doctype_short = check_plain($result_url->doctype);
  if ($result_url->validity) {
    $result_str .= t('This page is valid %doctype_short', array('%doctype_short' => $doctype_short));
  }
  else {
    $result_str .= t('This page is not valid %doctype_short', array('%doctype_short' => $doctype_short));
  }

  $output .= '<p><strong>'. t('Result') .':</strong> '. $result_str .'</p>';

  $output .= '<p>'. l(t('Revalidate this url'), 'validator', array('query' => array('uri' => $result_url->url))) .'</p>';

  $sql = "SELECT * FROM {validator_results_messages} WHERE urlid = %d";
  $result = pager_query($sql, 10, 0, NULL, $result_url->urlid);
  $headers = array(t('Type'), t('Line'), t('Col'), t('Message'));
  $rows = array();

  while ($row = db_fetch_object($result)) {
    $rows[] = array(
      ucfirst(check_plain($row->type)),
      check_plain($row->line),
      check_plain($row->col),
      check_plain($row->message),
      );
  }

  $output .= theme('table', $headers, $rows);

  $output .= theme('pager');

  // Set a proper breadcrumb
  $result_set = w3c_validator_result_set_load($result_url->rsid);
  $date_str = format_date($result_set->date);
  $orig_path = $_GET['q'];
  menu_set_active_item('admin/content/validator/results/'. $result_url->rsid);
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Results'), 'admin/content/validator/results');
  $bc[] = l($date_str, 'admin/content/validator/results/'. $result_url->rsid);
  drupal_set_breadcrumb($bc);
  menu_set_active_item($orig_path);

  return $output;
}

/**
 * Validate views page displays.
 *
 * @param string $rsid
 * @return void
 */
function w3c_validator_site_validate_views($rsid) {
  if (!module_exists('views')) {
    return;
  }

  $views = views_get_all_views();

  $ops = array();

  foreach ($views as $view_name => $view) {
    if (!$view->disabled) {
      foreach ($view->display as $display_name => $display) {
        if ($display->display_plugin == 'page') {
          $path = $display->display_options['path'];
          if (strpos($path, '%') === FALSE) {
            $ops[] = array('w3c_validator_site_validate_path', array($rsid, $path));
          }
        }
      }
    }
  }

  return $ops;
}
