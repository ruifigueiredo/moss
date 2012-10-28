<?php
/**
 * @file * Admin include file. 
*/

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
function alchemy_tagging_suggest_admin_settings() {
 
  $options = alchemy_tagging_suggest_get_types();
  $form['alchemy_tagging_suggest_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Term types'), 
    '#options' => $options,
    '#default_value' => variable_get('alchemy_tagging_suggest_types', alchemy_tagging_suggest_get_types(TRUE)),
    '#description' => t('Alchemy provides an array of data it can extract from content. Use the above checkboxes to select which ones you want to use to generate tagging suggestions.'),
  );
  
  $form['alchemy_tagging_suggest_relevance_min'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimal relevancy'),
    '#default_value' => variable_get('alchemy_tagging_suggest_relevance_min', 50),
    '#description' => t('Use this to specify the minimum relevance a phrase must have in order to be included. Set between 0 - 100.'),
    '#field_suffix' => t('%'),
    '#size' => 3,
  );
  
  $form['alchemy_tagging_suggest_usecache'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use cached data if available.'), 
    '#default_value' => variable_get('alchemy_tagging_suggest_usecache', FALSE),
    '#description' => t('The responses from Alchemy are cached. If you want to enable fetching data from cache rather than fetching from the Alchemy API, check the above box. This setting is useful when Tagging is used with Content Analysis or Autotagging modules are both enabled since since those modules will refresh the cache.'),
    '#prefix' => '<div id="alchemy_tagging_suggest_cache">',
    '#suffix' => '</div>',  
  );
  
  return system_settings_form($form);
}