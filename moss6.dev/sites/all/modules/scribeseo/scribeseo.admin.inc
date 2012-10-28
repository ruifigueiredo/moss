<?php
// $Id: scribeseo.admin.inc,v 1.1.2.8 2010/09/26 00:43:12 tomdude48 Exp $

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
function scribeseo_admin_settings() {
  //Define the titles
  if (variable_get('scribeseo_apikey', '')) {
    $userAccountAccess = scribeseo_getUserInfo();
    $userAccountData = $userAccountAccess->results['GetAccountStatusResult'];
    if (!$userAccountData) {
      drupal_set_message(t('Unable to communicate with Scribe server.'), 'warning');
    } 
    elseif ($userAccountData['Exception']['Type']) {
      drupal_set_message($userAccountData['Exception']['Message'] . "(type " . $userAccountData['Exception']['Type'] . ")", 'warning');
    } 
    else {
      $accountStatus = $userAccountData['AccountStatus'];
      variable_set('scribeseo_credits', array('remaining' => $accountStatus['CreditsRemaining'], 'time' => time()));
    }
  }
  
//dsm($accountStatus);
  
  $form['scribeseo_apikey'] = array(
    '#type' => 'textfield',
    '#title' => t('Scribe API Key'), 
    '#description' => t('You will need a Scribe SEO API Key to enable analysis. <a href="' . SCRIBESEO_LINK_GETKEY . '" target="_blank">Get your API key here</a>.'),
    '#default_value' => variable_get('scribeseo_apikey', ''), 
  );
  
  $form['scribeseo_usessl'] = array(
    '#type' => 'select',
    '#title' => t('Security method'), 
    '#description' => t('Use of Enhanced Security (with SSL) can cause problems on some shared hosts. Only select this option if you are certain you need it. See the <a href="https://my.scribeseo.com">suppport site</a> for more information.'),
    '#options' => array('http' => t('Basic Non-SSL'), 'https' => t('Enhanced SSL')),
    '#default_value' => variable_get('scribeseo_usessl', ''), 
  );
  
  $form['scribeseo_usecache_default'] = array(
    '#type' => 'radios',
    '#title' => t('Default re-analysis preference'), 
    '#description' => t('The Scribe SEO API limits the number of submissions per month based on type of account. The Scribe SEO module caches results from previous submissions. The Content Analysis interface enables you to select to review the last analysis or submit to the Scribe API for a new analysis. Use the above options to set your default preference for nodes and URLs that have previously cached analysis.'),
    '#options' => array('1' => t('Show the cached version'), '0' => t('Submit to Scribe SEO for re-analysis')),
    '#default_value' => variable_get('scribeseo_usecache_default', '0'), 
  );
  
  $d = t('Scribe SEO requires a meta description for analysis.');
  $d .= t('The recommended method is to use the ');
  $d .= l(t('Meta Tags module'), 'http://drupal.org/project/nodewords', array('attributes' => array('target' => 'drupal_nodewords')));
  $d .= t(' to enter a description per node.');
  $d .= t('The above settings provide an alternative method if Scribe cannot retrieve a description from the Meta Tags module.');
  $form['scribeseo_altdescription'] = array(
    '#type' => 'radios',
    '#title' => t('Alternative description options'), 
    '#description' => $d,
    '#options' => array('' => t('Do not use an alternative.'), 'node_teaser' => t('Use Drupal\'s node teaser function.'), 'first_body_sentence' => t('Use the first sentence of the body.')),
    '#default_value' => variable_get('scribeseo_altdescription', 'node_teaser'), 
  );
  
  $d = t('Enables Scribe SEO data to display on the ');
  $d .= l(t('admin node form'), 'admin/content/node', array('attributes' => array('target' => 'admin_node_form')));
  $d .= '.';
  $form['scribeseo_node_admin_form_options'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Node admin form options'), 
    '#description' => $d,
    '#options' => array('score' => t('Display SEO score.'), 'primarykeyword' => t('Display primary keywords.')),
    '#default_value' => variable_get('scribeseo_node_admin_form_options', array('score')), 
  );
  

  
  if (!$accountStatus) {
    return system_settings_form($form);
  }
  $form['scribeseo_acctstatus_hdr'] = array(
    '#type' => 'markup',
    '#value' => '<h3>' . t('Account Status') . '</h3>', 
  );
  $form['scribeseo_acctstatus'] = array(
    '#type' => 'item',
    '#title' => t('Account Status'), 
    '#value' => $accountStatus['AccountStatus'],
  );
  $form['scribeseo_accttype'] = array(
    '#type' => 'item',
    '#title' => t('Account Type'), 
    '#value' => $accountStatus['AccountType'],
  );
  $form['scribeseo_evals'] = array(
    '#type' => 'item',
    '#title' => t('Evaluations'), 
    '#value' => $accountStatus['CreditsTotal'] . ' ' . t('evaluations per month (1 evaluation = 1 SEO analysis) '),
  );
  $form['scribeseo_evalsleft'] = array(
    '#type' => 'item',
    '#title' => t('Evaluations left'), 
    '#value' => $accountStatus['CreditsRemaining']. ' ' . t('evaluations as of '. date('F j, Y, g:i a')),
  );
  
  drupal_add_css(drupal_get_path('module', 'contentanalysis') . '/contentanalysis.css');
  $form['scribeseo_upgradelink'] = array(
    '#type' => 'markup',
    '#value' => '<div class="contentanalysis">' . l(t('Upgrade Account'), SCRIBESEO_LINK_UPGRADEACCOUNT, array('attributes' => array('target' => '_scribeseo', 'class' => 'button'))) . t(' Get more evaluation credits by upgrading your account.') . '</div><br>',
  );  

  return system_settings_form($form);
}

