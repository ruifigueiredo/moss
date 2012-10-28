<?php

/**
 * @file
 * Admin include file. 
 */

/**
 * Displays the form for the standard settings tab.
 *
 * @return
 *   array A structured array for use with Forms API.
 */
function kwresearch_admin_settings() {
 
	
  $sources = module_invoke_all('kwresearch_sources');
  if (is_array($sources) && count($sources) > 1) {
	  foreach ($sources AS $aid => $source) {
      $s[] = $source['title'];
	  }
	  $form['sources'] = array(
      '#type' => 'item',
      '#title' => t('Enabled keyword statistics sources'), 
      '#value' => theme_item_list($s), 
	    '#description' => t('Sources provide data for keyword stats reports.'), 
    );
  }
  else {
    $form['sources'] = array(
      '#type' => 'item',
      '#title' => t('Sources'), 
      '#value' => t('No keyword statistics sources have been enabled. To access keyword popularity data enable a data source such as the Keyword Research Google (Keyword Research sub-module), !scribe, !wordstream or !wordtracker modules.',
        array(
          '!scribe' => l('Scribe SEO', 'http://drupal.org/project/scribeseo'),
          '!wordstream' => l('WordStream', 'http://drupal.org/project/wordstream'),
          '!wordtracker' => l('Wordtracker', 'http://drupal.org/project/wordtracker'),
        )
      ), 
    );
    $sources = array();  	
  }

	/*
	if (!module_exists('scribeseo')) {
    $msg = t('You can also use the Scribe SEO API as a source for keyword statitics; !link.',
      array(
        '!link' => l(t('download and install Scribe SEO'), 'http://drupal.org/project/scribeseo', array('attributes' => array('target' => 'drupal'))),
      )
    );
  }
  elseif (!variable_get('scribeseo_apikey', '')) {
    $msg .= t('You can also use the Scribe SEO API as a source for keyword statitics. The Scribe module is installed, however an the API key is missing; !link.',
      array(
        '!link' => l(t('enable the Scribe API key'), 'admin/settings/scribeseo', array('attributes' => array('target' => 'drupal'))),
      )
    );  
  }
  else {
    $msg = t('The Scribe API is enabled as a keyword data source.');
  }
  $form['scribe_apikey'] = array(
    '#type' => 'item',
    '#title' => t('Scribe API Key'), 
    '#value' => $msg,  
  );
  */
  
 
  $form['kwresearch_stats_report_options'] = array(      
    '#type' => 'fieldset',      
    '#title' => t('Stats report options'),      
    '#collapsible' => TRUE,      
    '#collapsed' => FALSE,    
  );
  require_once './' . drupal_get_path('module', 'kwresearch') . "/includes/stats_report.inc";
  $options = kwresearch_get_report_data_options(); 
  $def_values = kwresearch_get_report_data_options_defaults();
//dsm(variable_get('kwresearch_popularity_report_columns', $def_values));
  $form['kwresearch_stats_report_options']['kwresearch_stats_report_data_options'] = array(    
    '#type' => 'checkboxes',    
    '#title' => t('Report data'),   
    '#options' => $options,
    '#default_value' => variable_get('kwresearch_stats_report_data_options', $def_values),     
    //'#description' => t('')     
  ); 
  
  $options = kwresearch_get_report_links_options(); 
  $def_values = kwresearch_get_report_links_options_defaults();
  $form['kwresearch_stats_report_options']['kwresearch_stats_report_links_options'] = array(    
    '#type' => 'checkboxes',    
    '#title' => t('Report links'),   
    '#options' => $options,
    '#default_value' => variable_get('kwresearch_stats_report_links_options', $def_values),     
    //'#description' => t('')     
  ); 
  $form['kwresearch_stats_report_options']['kwresearch_stats_report_item_limit'] = array(    
    '#type' => 'textfield',    
    '#title' => t('Item limit'),   
    '#size' => 5,
    '#default_value' => variable_get('kwresearch_stats_report_item_limit', KWARESEARCH_STATS_REPORT_ITEM_LIMIT_DEFAULT),     
    '#description' => t('The maximum number of items to show in the report')     
  ); 

  $form['kwresearch_site_report_options'] = array(      
    '#type' => 'fieldset',      
    '#title' => t('Site report options'),      
    '#collapsible' => TRUE,      
    '#collapsed' => FALSE,    
  );
  require_once './' . drupal_get_path('module', 'kwresearch') . "/includes/site_report.inc";
  $options = kwresearch_get_report_data_options('site'); 
  $def_values = kwresearch_get_site_report_data_options_defaults();
//dsm(variable_get('kwresearch_popularity_report_columns', $def_values));
  $form['kwresearch_site_report_options']['kwresearch_site_report_data_options'] = array(    
    '#type' => 'checkboxes',    
    '#title' => t('Report data'),   
    '#options' => $options,
    '#default_value' => variable_get('kwresearch_site_report_data_options', $def_values),     
    //'#description' => t('')     
  );   

  $form['vocab'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy settings'),
    '#description' => t('Use the below drop downs to select any vocabularies for tagging nodes with keywords. The Keyword Reserach stores tags in its own tables with meta data and suport for non-nodes. If you want to sync this data with a vocabulary, use the "sync" selection. If you want to store terms in vocabulary independent of Keyword Research\'s data, use the "tag" selection.'),
  );
  $vocabs = taxonomy_get_vocabularies();
  $options = array(0 => t('None'));
  $options2 = array();
  if (is_array($vocabs) && (count($vocabs) > 0)) {
	  foreach ($vocabs AS $vid => $v) {
	    if ($v->tags) {
	      $options[$vid] = $v->name;
	      $options2[$vid] = $v->name;
	    }
	  }
	  $form['vocab']['kwresearch_keyword_sync_vocabulary'] = array(    
	    '#type' => 'select',    
	    '#title' => t('Page keyword sync vocabulary'),   
	    '#options' => $options,
	    '#default_value' => variable_get('kwresearch_keyword_sync_vocabulary', 0),     
	    '#description' => t('Select a vocabulary to sync with Keyword Research\'s internal page keywords.')     
	  );  
	  
	  $form['vocab']['kwresearch_keyword_tag_vocabulary'] = array(    
	    '#type' => 'select',    
	    '#title' => t('Page keyword tag vocabulary'),   
	    '#options' => $options,
	    '#default_value' => variable_get('kwresearch_keyword_tag_vocabulary', 0),     
	    '#description' => t('Select a vocabulary for storing keywords independed of Keyword Research\'s page keywords.')     
	  );
	
	  $form['vocab']['kwresearch_report_vocabulary'] = array(    
	    '#type' => 'checkboxes',    
	    '#title' => t('Report vocabulary'),   
	    '#options' => $options2,
	    '#default_value' => variable_get('kwresearch_report_vocabulary', 0),     
	    '#description' => t('Select any vocabulary you would like to attach the keyword reports.')     
	  );
  }
  else {
    $form['vocab']['no'] = array(
	    '#type' => 'item',
	    '#title' => t('No vocabularies exist'),
	    '#description' => t('To sync keyword research you need to !create_a_vocabulary first.',
	      array(
	        '!create_a_vocabulary' => l(t('create a vocabulary'), 'admin/content/taxonomy/add/vocabulary'),
	      )
	    ),
	  );  	
  }





  return system_settings_form($form);
}

/**
 * Generates site keywords edit form page
 * @param int $kid - keyword id if editing keyword data
 */
function kwresearch_keywords_list_edit($kid = 'add') {
  if ($kid > 0) {
    $sql = '
      SELECT * 
      FROM {kwresearch_keyword}
      WHERE kid = %d
    ';
    $keyword = db_fetch_object(db_query($sql, $kid));
    drupal_set_title(t('Edit site keyword'));
  }
  if (!$keyword->keyword) {
    drupal_set_title(t('Add site keyword'));
  }  
  return drupal_get_form('kwresearch_site_keywords_edit_form', $keyword);
}

/**
 * Returns site keyword edit form array
 * 
 * @param $form_state
 * @param keyword obj $keyword
 */
function kwresearch_site_keywords_edit_form($form_state, $keyword) {
  if ($keyword->kid) {
    $form['kid'] = array(
      '#type' => 'value',
      '#value' => $keyword->kid
    );
    $form['keyword'] = array(
      '#type' => 'item',
      '#title' => t('Keyword'), 
      '#value' => $keyword->keyword,
    );    
  }
  else {
    $form['kid'] = array(
      '#type' => 'value',
      '#value' => 0,
    );
    $form['keyword'] = array(
      '#type' => 'textfield',
      '#title' => t('Keyword'), 
      '#required' => TRUE,
      '#default_value' => $_GET['keyword'],
    );  
  }
  
  $options = kwresearch_get_priority_options();
  $form['priority'] = array(
    '#type' => 'select',
    '#title' => t('Priority'), 
    '#options' => $options,
    '#default_value' => ($keyword->priority) ? $keyword->priority : 50,
    '#description' => t('Use to set how important it is for the site to rank well for this keyword. If unsure, just set to "Standard".'),
  );  

  $form['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Value'),
    '#field_prefix' => t('$'),
    '#default_value' => ($keyword->value >= 0) ? number_format($keyword->value, 2) : '',
    '#description' => t('Use to place a financial value for each visitor who comes to the site using this keyword.'),
  ); 
  
  if ($keyword->stats_update) { 
    $form['stats'] = array(      
      '#type' => 'fieldset',      
      '#title' => t('Statistics'),      
      '#collapsible' => TRUE,      
      '#collapsed' => FALSE,    
    );  
    $form['stats']['stats_update'] = array(
      '#type' => 'item',
      '#title' => t('Last updated'), 
      '#value' => format_date($keyword->stats_update),
    );  
    $form['stats']['sdaily_volume'] = array(
      '#type' => 'item',
      '#title' => t('Est. daily search volume'), 
      '#value' => number_format($keyword->daily_volume),
    );  
  }
    
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  
  $form['save_update'] = array(
    '#type' => 'submit',
    '#value' => t('Save & update stats'),
  );
  
  return $form;
}

/**
 * Validates site keyword edit form
 * 
 * @param $form
 * @param $form_state
 */
function kwresearch_site_keywords_edit_form_validate($form, &$form_state) {
  //if (!is_numeric($form_state['values']['value']) || $form_state['values']['value'] < 0) {
  //  form_set_error('value', t('Please set a numeric value for the value field greater than 0.'));
  //}
}

/**
 * Processes site keyword edit form
 *   saves settings to database
 * 
 * @param $form
 * @param $form_state
 */
function kwresearch_site_keywords_edit_form_submit($form, &$form_state) {
  $keyword_obj = new stdClass();
  $kid = ($form_state['values']['kid']) ? $form_state['values']['kid']:NULL;
  $keyword = strtolower($form_state['values']['keyword']);
  $keyword_obj->priority = $form_state['values']['priority'];
  $keyword_obj->value = (float)$form_state['values']['value'];
  $update_stats = ($form_state['values']['op'] == t('Save & update stats'));
  
  kwresearch_save_site_keyword($keyword, $kid, $keyword_obj, $update_stats);
  
  if (!$kid) {
    drupal_set_message(t('Site keyword <em>@keyword</em> has been added.', 
      array(
        '@keyword' => $keyword
      )),
      'status'
    );
  }
  else {
    drupal_set_message(t('Site keyword <em>@keyword</em> has been updated.', 
      array(
        '@keyword' => $keyword
      )),
      'status'
    );
  }
}

/**
 * Generates site keywords import page
 * 
 * @param int $kid
 */
function kwresearch_site_keywords_import($kid = 'add') {
  drupal_set_title(t('Import site keyword'));
  return drupal_get_form('kwresearch_site_keywords_import_form', $keyword);
}

/**
 * Returns site keywords import form array
 * 
 * @param $form_state
 * @param str $keyword
 */
function kwresearch_site_keywords_import_form($form_state, $keyword) {
  
  $options = kwresearch_get_priority_options();
  $form['import_data'] = array(
    '#type' => 'textarea',
    '#title' => t('Import data'), 
    '#description' => t('Paste in a list of keywords using the format "keyword,priority,value". Each keyword should be a new line. Priority and value values are optional.'),
  );  

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Add keywords to site list'),
  );
  
  return $form;
}

/**
 * Validates site keywords import form
 * 
 * @param $form
 * @param $form_state
 */
function kwresearch_site_keywords_import_form_validate($form, &$form_state) {
  if (!$form_state['values']['import_data']) {
    form_set_error('import_data', t('Please input data to import.'));
  }
}

/**
 * Processes site keywords import form
 * 
 * @param $form
 * @param $form_state
 */
function kwresearch_site_keywords_import_form_submit($form, &$form_state) {
  GLOBAL $user;
  $lines = explode("\n", $form_state['values']['import_data']);
  $imported = 0;
  foreach ($lines AS $line) {
    list($keyword, $priority, $value) = explode(",", $line);
    $keyword = drupal_strtolower(trim($keyword));
    if (!$keyword) {
      continue;
    }
    $keyword_obj = new stdClass();
    
    $set = '';
    $fields = '';
    $values = '';
    $format = 0;
    if ($priority) {
      $keyword_obj->priority = (int) $priority;
    }
    // set site kw priority to TBD if no value is given
    else {
      $keyword_obj->priority = 0;
    }
    if ($value) {
      $keyword_obj->value = (float) $value;
    }
    kwresearch_save_site_keyword($keyword, NULL, $keyword_obj);
    $imported++;
  }
  drupal_set_message(t('@imported keywords were imported.',
    array(
      '@imported' => $imported,
    ))
  );
}

/**
 * Generages page keyword edit form
 * 
 * @param int|str $pid - page id; the nid (int) or path (str) of the page
 * @param int $kid - keyword id
 */
function kwresearch_page_keywords_edit($pid, $kid = 'add') {
  if ($kid > 0) {
    $sql = '
      SELECT * 
      FROM {kwresearch_keyword}
      WHERE kid = %d
    ';
    $keyword = db_fetch_object(db_query($sql, $kid));
    drupal_set_title(t('Edit page keyword'));
  }
  if (!$keyword->keyword) {
    drupal_set_title(t('Add page keyword'));
  }  
  $output .= drupal_get_form('kwresearch_page_keywords_edit_form', $pid, $keyword);
  return $output;
}

/**
 * Returns page keyword edit form
 * 
 * @param arr $form_state
 * @param int|str $pid - page id; the nid (int) or path (str) of the page
 * @param str $keyword
 */

function kwresearch_page_keywords_edit_form($form_state, $pid, $keyword) {
  $url = url(($pid > 0)?'node/' . $pid:$pid);
  if (!$url) {
    drupal_set_message(t('Invalid page entered.'));
    return;
  }
  
  $page_keyword = kwresearch_load_page_keyword($pid, $keyword->kid);

  $form['page'] = array(
      '#title' => t('Page'),
      '#type' => 'item',
      '#value' =>  l($url, $url),
  );
  $form['pid'] = array(
    '#type' => 'value',
    '#value' => $pid
  );
  if ($keyword->kid) {
    $form['kid'] = array(
      '#type' => 'value',
      '#value' => $keyword->kid
    );
    $form['keyword'] = array(
      '#type' => 'item',
      '#title' => t('Keyword'), 
      '#value' => $keyword->keyword,
    );    
  }
  else {
    $form['kid'] = array(
      '#type' => 'value',
      '#value' => 0,
    );
    $form['keyword'] = array(
      '#type' => 'textfield',
      '#title' => t('Keyword'), 
      '#required' => TRUE,
    );  
  }
  
  $options = kwresearch_get_priority_options();
  $form['page_priority'] = array(
    '#type' => 'select',
    '#title' => t('Priority'), 
    '#options' => $options,
    '#default_value' => $page_keyword->priority,
    '#description' => t('Use to set how important it is for the site to rank well for this keyword. If unsure, just set to "Standard".'),
  ); 
    
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  
  $form['save_update'] = array(
    '#type' => 'submit',
    '#value' => t('Save & update stats'),
  );
  
  return $form;
}

/**
 * Validates the page keyword edit form
 * 
 * @param $form
 * @param $form_state
 */
function kwresearch_page_keywords_edit_form_validate($form, &$form_state) {
  if ($form_state['values']['value'] && (!is_numeric($form_state['values']['value']) || $form_state['values']['value'] < 0)) {
    form_set_error('value', t('Please set a numeric value for the value field greater than 0.'));
  }
}

/**
 * Processes the page keyword edit form
 * 
 * @param $form
 * @param $form_state
 */
function kwresearch_page_keywords_edit_form_submit($form, &$form_state) {
  $kid = $form_state['values']['kid'];
  $keyword = strtolower($form_state['values']['keyword']);
  $pid = $form_state['values']['pid'];
  $page_keyword_obj = new stdClass();
  $page_keyword_obj->priority = $form_state['values']['page_priority'];

  kwresearch_save_page_keyword($pid, $keyword, $kid, $page_keyword_obj);
  
  drupal_set_message(t('Site keyword <em>@keyword</em> has been updated.', 
    array(
      '@keyword' => $keyword
    )),
    'status'
  );
}

/**
 * Generates table of pages assoicated with a keyword
 * 
 * @param int $kid
 */
function kwresearch_keyword_pages_page($kid) {
  drupal_add_css(drupal_get_path('module', 'kwresearch') . '/kwresearch.css'); 
  
  drupal_set_title('Keyword pages');
  
  $keyword_obj = kwresearch_load_site_keyword($kid);
  $output = t('Keyword: @keyword',
    array(
      '@keyword' => $keyword_obj->keyword
    )      
  );
  
  $rows = array();

//dsm($filter);
  $header = array(
    array('data' => t('Page'), 'field' => 'k.page'),
    array('data' => t('Nid'), 'field' => 'pk.nid'),
    array('data' => t('Page priority'), 'field' => 'pk.priority', 'sort' => 'desc'),
    array('data' => t('Operations')),
  );  

  $sql = '
    SELECT pk.* 
    FROM {kwresearch_page_keyword} pk
    WHERE kid = %d
  ';
  $tablesort = tablesort_sql($header);
  $sql = $sql . $tablesort;
  $result = pager_query($sql, 100, 0, NULL, $kid);

  $priorities = kwresearch_get_priority_options();
  while ($r = db_fetch_object($result)) {
    if (!$r->path && $r->nid) {
      $r->path = 'node/' . $r->nid;
    }
    $rows[] = array('data' =>
      array(
        // Cells
        l(drupal_get_path_alias($r->path), $r->path),
        l($r->nid, 'node/' . $r->nid),
        $priorities[$r->priority],
        l(t('keywords'), 'admin/content/kwresearch/page_keywords/' . $r->nid, array('query' => 'destination=admin/content/kwresearch/keywords_pages')),
      ),
      // Attributes for tr
      'class' => "kwresearch"
    );
  }

  if (!$rows) {
    $rows[] = array(array('data' => t('No pages are associated with this keyword.'), 'colspan' => count($header)));
  }  

  $output .= theme('table', $header, $rows, array('id' => 'kwresearch-site-keywords'));
  $output .= theme('pager', NULL, 100, 0);

  return $output;  
}

