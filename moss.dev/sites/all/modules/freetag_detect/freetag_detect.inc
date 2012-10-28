<?php

/**
 * AJAX callback, get tokens that match in the vocabulary to the body being passed in
 * kind of expensive.
 */
function freetag_detect_ajax_callback () {
  $clickedId = $_POST['id'];
  // @todo see if we can get this from the field settings
  $case_sensitive = intval($_POST['case_sensitive']);

  if($case_sensitive) {
    $content = $_POST['content'];
  } else {
    $content = drupal_strtolower($_POST['content']);
  }




  if(strlen($_POST['existing'])) {
    if(!$case_sensitive) {
      $existing = explode(',',drupal_strtolower(trim($_POST['existing'])));
    } else {
      $existing = explode(',',trim($_POST['existing']));
    }
  } else {
    $existing = array();
  }


  // @todo better detection of field name, but this works for now, we could probably pass it in at the top of the stack
  $field_name = preg_replace('/^edit-/', '', $clickedId);
  $field_name = preg_replace('/-und-scan-button$/', '', $field_name);

  // Make sure the field exists and is a taxonomy field.
  if (!($field = field_info_field(str_replace('-','_',$field_name))) || $field['type'] !== 'taxonomy_term_reference') {
    // Error string. The JavaScript handler will realize this is not JSON and
    // will display it as debugging information.
    print t('Taxonomy field @field_name not found.', array('@field_name' => $field_name));
    exit;
  }


  $vids = array();
  $vocabularies = taxonomy_vocabulary_get_names();
  foreach ($field['settings']['allowed_values'] as $tree) {
    $vids[] = $vocabularies[$tree['vocabulary']]->vid;
  }

  // not sure what the most efficient way here is
  // get all the terms and see what shows up i guess.
  $query = db_select('taxonomy_term_data', 't');
  $query->addTag('translatable');
  $query->addTag('term_access');

  $terms = $query
    ->fields('t', array('tid', 'name'))
    ->condition('t.vid', $vids)
    ->execute()
    ->fetchAllKeyed();

  $data = array();


  if(strlen($_POST['existing'])) {
    $existing['terms']= explode(',', trim($_POST['existing']));
    $data['terms'] = $existing['terms'];
  } else {
    $existing = array();
    $data['terms'] = array();
  }

  // again, not sure the best method here either, lets loop
  foreach ($terms as $term) {
    if(!$case_sensitive) {
      $lterm = drupal_strtolower($term);
    } else {
      $lterm = $term;
    }
    $preg_term = preg_quote($lterm);
    if ( preg_match("/[^a-zA-Z]{$preg_term}[^a-zA-Z]/", $content) ) {
      if(!in_array($lterm, $existing)) {
        $data['terms'][] = $term;
      }
    }
  }

  // and set the update field name
  $data['fieldId'] = str_replace('-scan-button','',$clickedId);
  drupal_json_output($data);
}
