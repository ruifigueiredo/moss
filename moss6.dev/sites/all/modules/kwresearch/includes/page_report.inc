<?php

/**
 * @file
 * Admin include file. 
 */

/**
 * Generates table of keywords associated with a pages
 * 
 * @param int|str $pid - page id; the nid (int) or path (str) of the page
 */
function kwresearch_page_keywords_page($pid, $mode = NULL) {
  drupal_add_css(drupal_get_path('module', 'kwresearch') . '/kwresearch.css'); 
  kwresearch_set_admin_theme(); 
  drupal_set_title('Page keywords');
  if ($mode == 'node') {
    $pid = $pid->nid;
  }
  if ($pid > 0) {
    $node = node_load($pid);
    $path = 'node/' . $pid;
    $where = 'nid = %d';
  }
  else {
    $path = $pid;
    $where = 'path = "%s"';
  }
  $output = t('Page: !page',
    array(
      '!page' => l($path, $path)
    )      
  );
  
  $rows = array();

//dsm($filter);
  $header = array(
    array('data' => t('Keyword'), 'field' => 'k.keyword'),
    array('data' => t('Page priority'), 'field' => 'pk.priority', 'sort' => 'desc'),
    array('data' => t('Set by'), 'field' => 'up.name'),
    array('data' => t('Site priority'), 'field' => 'k.priority'),    
    array('data' => t('Value'), 'field' => 'k.value'),
    array('data' => t('Set by'), 'field' => 'u.name'),
    array('data' => t('Total Daily'), 'field' => 'k.daily_volume'),
    array('data' => t('Operations')),
  );  

  $sql = '
    SELECT pk.priority AS page_priority, pk.uid AS page_uid, k.*, u.name AS username, up.name AS page_username
    FROM {kwresearch_page_keyword} pk
    JOIN {kwresearch_keyword} k ON pk.kid = k.kid
    LEFT JOIN {users} u ON u.uid = k.uid
    LEFT JOIN {users} up ON up.uid = pk.uid
    WHERE ' . $where . '
  ';
  $tablesort = tablesort_sql($header);
  $sql = $sql . $tablesort;
  $result = pager_query($sql, 100, 0, NULL, $pid);

  $priorities = kwresearch_get_priority_options();
  $dest = 'admin/content/kwresearch/page_keywords/' . $pid;
  if ($mode == 'node') {
    $dest = 'node/' . $pid . '/kwresearch';
  }
  while ($r = db_fetch_object($result)) {

    $rows[] = array('data' =>
      array(
        // Cells
        check_plain($r->keyword),
        $priorities[$r->page_priority],
        ($r->page_uid > 0) ? l($r->page_username, 'user/' . $r->page_uid) : '-',
        ($r->priority > 0) ? $priorities[$r->priority] : '-',
        ($r->value >= 0) ?  t('$') . number_format($r->value, 2) : '-',
        ($r->uid > 0) ? l($r->username, 'user/' . $r->uid) : '-',        
        ($r->daily_volume >= 0) ? number_format($r->daily_volume) : '-',
        l(t('edit'), 'admin/content/kwresearch/page_keywords_edit/' . $pid . '/' . $r->kid, array('query' => 'destination=' . $dest)),
      ),
      // Attributes for tr
      'class' => "kwresearch"
    );
  }

  if (!$rows) {
    $rows[] = array(array('data' => t('No keywords associated with this page.'), 'colspan' => count($header)));
  }  

  $output .= theme('table', $header, $rows, array('id' => 'kwresearch-site-keywords'));
  $output .= theme('pager', NULL, 100, 0);
  
  $output .= l(t('Add page keyword'), 'admin/content/kwresearch/page_keywords_edit/' . $pid, array('query' => 'destination=' . $dest));

  return $output;  
}