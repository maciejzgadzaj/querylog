<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for the querylog module.
 */

/**
 * querylog module settings form.
 */
function querylog_settings_form() {
  $form = array();
  // General options
  $form['general'] = array(
    '#type'           => 'fieldset',
    '#title'          => t('General settings'),
  );
  $form['general']['dev_query'] = array(
    '#type'           => 'checkbox',
    '#title'          => t('Collect query info'),
    '#description'    => t("Enable Drupal's query info collecting. If disabled, no query log functionality will work."),
    '#default_value'  => variable_get('dev_query', 0),
  );
  $form['general']['querylog_log_queries'] = array(
    '#type'           => 'checkbox',
    '#title'          => t('Save queries to database'),
    '#description'    => t('Save all executed queries to database.'),
    '#default_value'  => variable_get('querylog_log_queries', 0),
  );
  $form['general']['querylog_max_entries'] = array(
    '#type'           => 'select',
    '#title'          => t('Discard log entries above the following limit'),
    '#options'        => drupal_map_assoc(array(100, 1000, 10000, 100000, 1000000)),
    '#description'    => t('The maximum number of rows to keep in the database log. Older entries will be automatically discarded.'),
    '#default_value'  => variable_get('querylog_max_entries', '10000'),
  );
  $form['general']['querylog_print_queries'] = array(
    '#type'           => 'checkbox',
    '#title'          => t('Print queries to screen'),
    '#description'    => t('Print all executed queries to screen. (Only available for user roles with "display queries" <a href="@permissions">permission</a> set.)', array(
      '@permissions'    => url('admin/user/permissions', array('fragment' => 'module-querylog'))
    )),
    '#default_value'  => variable_get('querylog_print_queries', 0),
  );
  // Function filtering
  $form['function_filter'] = array(
    '#type'           => 'fieldset',
    '#title'          => t('Function specific filtering settings'),
  );
  $form['function_filter']['querylog_filter_function_mode'] = array(
    '#type'           => 'radios',
    '#title'          => t('Save queries from specific functions'),
    '#options'        => array(
      t('Save from every function except the listed functions.'),
      t('Save from only the listed functions.')
    ),
    '#default_value'  => variable_get('querylog_filter_function_mode', 0),
  );
  $form['function_filter']['querylog_filter_function_list'] = array(
    '#type'           => 'textarea',
    '#title'          => t('Functions'),
    '#default_value'  => variable_get('querylog_filter_function_list', ''),
    '#description'    => t("Enter one function name per line. The '*' character is a wildcard. Example functions are %cache for all cache queries and %sess for all session queries.", array(
      '%cache'          => 'cache*',
      '%sess'           => 'sess*',
    )),
  );
  // Page filtering
  $form['page_filter'] = array(
    '#type'           => 'fieldset',
    '#title'          => t('Page specific filtering settings'),
  );
  $form['page_filter']['querylog_filter_page_mode'] = array(
    '#type'           => 'radios',
    '#title'          => t('Save queries from specific functions'),
    '#options'        => array(
      t('Save from every page except the listed pages.'),
      t('Save from only the listed pages.')
    ),
    '#default_value'  => variable_get('querylog_filter_page_mode', 0),
  );
  $description = '<p>' . t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array(
    '%blog'           => 'blog',
    '%blog-wildcard'  => 'blog/*',
    '%front'          => '<front>',
  )) . '</p>';
  $description .= '<p>' . t('It as advised that path %querylog_path stays excluded, otherwise browsing through <a href="@recent_entries">Recent Database Entries</a> list could get pretty difficult, as by default it is sorted by date in descending order, and each page load would add new set of queries to the top of the list.', array(
    '%querylog_path'  => 'admin/reports/querylog*',
    '@recent_entries' => url('admin/reports/querylog'),
  )) . '</p>';
  $form['page_filter']['querylog_filter_page_list'] = array(
    '#type'           => 'textarea',
    '#title'          => t('Pages'),
    '#default_value'  => variable_get('querylog_filter_page_list', ''),
    '#description'    => $description,
  );
  return system_settings_form($form);
}

/**
 * Menu callback; displays a listing of saved database queries.
 */
function querylog_overview() {

  $filter = _querylog_build_filter_query();
  $output = drupal_get_form('_querylog_overview_filter_form');

  $header = array(
    array(
      'data'  => t('Function'),
      'field' => 'q.function'
    ),
    array(
      'data'  => t('Query'),
      'field' => 'q.query'
    ),
    array(
      'data'  => t('Execution'),
      'field' => 'q.time'
    ),
    array(
      'data'  => t('Date'),
      'field' => 'q.timestamp',
      'sort'  => 'desc'
    ),
    array(
      'data'  => t('User'),
      'field' => 'u.name'
    ),
  );
  $drupal_version = array_shift(explode('.', VERSION));

  $sql = "SELECT q.qid, q.function, q.query, q.time, q.uid, q.timestamp, u.name FROM {querylog} q INNER JOIN {users} u ON q.uid = u.uid";
  $tablesort = tablesort_sql($header);
  if (!empty($filter['where'])) {
    $result = pager_query($sql ." WHERE ". $filter['where'] . $tablesort, 50, 0, NULL, $filter['args']);
  }
  else {
    $result = pager_query($sql . $tablesort, 50);
  }

  while ($row = db_fetch_object($result)) {
    $rows[] = array('data' =>
      array(
        l($row->function, sprintf('http://api.drupal.org/api/function/%s/%d', $row->function, $drupal_version)),
        l(truncate_utf8($row->query, 56, TRUE, TRUE), 'admin/reports/querylog/'. $row->qid, array('html' => TRUE)),
        number_format($row->time * 1000, 2) . ' ms',
        format_date($row->timestamp, 'small'),
        theme('username', $row),
      ),
      // Attributes for tr
      'class' => "querylog-". preg_replace('/[^a-z]/i', '-', $row->function),
    );
  }

  if (!$rows) {
    $rows[] = array(array('data' => t('No queries available.'), 'colspan' => 4));
  }

  $output .= theme('table', $header, $rows, array('id' => 'admin-querylog'));
  $output .= theme('pager', NULL, 50, 0);

  return $output;
}

/**
 * Menu callback; displays details about of a specific database query.
 */
function querylog_event($qid) {
  $output = '';
  $result = db_query('SELECT q.qid, q.function, q.query, q.time, q.uid, q.timestamp, u.name, u.uid FROM {querylog} q INNER JOIN {users} u ON q.uid = u.uid WHERE q.qid = %d', $qid);
  if ($row = db_fetch_object($result)) {
    $rows = array(
      array(
        array('data' => t('Function'), 'header' => TRUE),
        $row->function,
      ),
      array(
        array('data' => t('Query'), 'header' => TRUE),
        _querylog_format_query($row->query),
      ),
      array(
        array('data' => t('Execution time'), 'header' => TRUE),
        $row->time * 1000 . ' ms',
      ),
      array(
        array('data' => t('Date'), 'header' => TRUE),
        format_date($row->timestamp, 'large'),
      ),
      array(
        array('data' => t('User'), 'header' => TRUE),
        theme('username', $row),
      ),
    );
    $attributes = array('class' => 'querylog-event');
    $output = theme('table', array(), $rows, $attributes);
  }
  return $output;
}

/**
 * Returns form for log filters.
 */
function _querylog_overview_filter_form() {
  $session = &$_SESSION['querylog_overview_filters'];
  $session = is_array($session) ? $session : array();
  $filters = _querylog_overview_filters();

  // Fieldsets
  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter query log'),
    '#theme' => 'querylog_overview_filters',
    '#collapsible' => TRUE,
    '#collapsed' => empty($session),
  );

  // Form elements
  foreach ($filters as $key => $filter) {
    $form['filters'][$filter['fieldset']][$key] = $filter['field'];
    // Add default values if previously posted values are in session
    if (!empty($session[$key])) {
      $form['filters'][$filter['fieldset']][$key]['#default_value'] = $session[$key];
    }
  }

  // Submit button
  $form['filters']['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
    '#prefix' => '<div style="clear:both"></div>',
  );

  // Add reset button if previously posted values are in session
  if (!empty($session)) {
    $form['filters']['buttons']['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Reset')
    );
  }

  return $form;

}

/**
 * Processes result from submitted log filters form.
 */
function _querylog_overview_filter_form_submit($form, &$form_state) {
  $op = $form_state['values']['op'];
  $filters = _querylog_overview_filters();
  switch ($op) {
    case t('Filter'):
      foreach ($filters as $name => $filter) {
        if (isset($form_state['values'][$name])) {
          $_SESSION['querylog_overview_filters'][$name] = $form_state['values'][$name];
        }
      }
      break;
    case t('Reset'):
      $_SESSION['querylog_overview_filters'] = array();
      break;
  }
  return 'admin/reports/dblog_viewer';
}

function _querylog_overview_filters() {
  $filters = array();
  $filters['function'] = array(
    'fieldset'  => 'content',
    'field' => array(
      '#type'   => 'textfield',
      '#title'  => t('Function contains'),
      '#size'   => 30,
    ),
    'where' => "function LIKE '%%%s%%'",
  );
  $filters['query'] = array(
    'fieldset'  => 'content',
    'field' => array(
      '#type'   => 'textfield',
      '#title'  => t('Query contains'),
      '#size'   => 30,
    ),
    'where' => "query LIKE '%%%s%%'",
  );
  return $filters;
}

/**
 * Builds query for log filters based on session.
 */
function _querylog_build_filter_query() {
  if (empty($_SESSION['querylog_overview_filters'])) {
    return;
  }

  $filters = _querylog_overview_filters();

  // Build query
  $where = $args = array();
  foreach ($_SESSION['querylog_overview_filters'] as $key => $filter) {
    $filter_where = array();
    // Selects
    if (is_array($filter)) {
      foreach ($filter as $value) {
        if ($value) {
          $filter_where[] = $filters[$key]['where'];
          $args[] = ($filters[$key]['value_modifier']) ? call_user_func($filters[$key]['value_modifier'], $value) : $value;
        }
      }
    }
    // Inputs
    elseif ($filter) {
      $filter_where[] = $filters[$key]['where'];
      $args[] = ($filters[$key]['value_modifier']) ? call_user_func($filters[$key]['value_modifier'], $filter) : $filter;
    }
    if (!empty($filter_where)) {
      $where[] = '('. implode(' OR ', $filter_where) .')';
    }
  }
  $where = !empty($where) ? implode(' AND ', $where) : '';

  return array(
    'where' => $where,
    'args' => $args,
  );
}

/**
 * Basic formatting for SQL queries.
 * Based on SQL Formatter class by Ziadin Givan:
 * http://www.phpkode.com/source/s/php-code-beautifier/SQLFormatter.inc
 */
function _querylog_format_query($sql, $indent = "\t") {
  // Remove new lines
  $sql = preg_replace('/[\r\n]/', '', $sql);
  // Remove trailing spaces
  $sql = preg_replace('/\s+/', ' ', $sql);
  // Put SQL keywords on new lines
  $sql = preg_replace('/\s*(FROM|WHERE|LIMIT|VALUES|ORDER|SET|OFFSET|GROUP BY)\s*/', "\n$1\n$indent", $sql);
  // Add new lines after SELECT and commas
  $sql = preg_replace('/\s*(SELECT|UPDATE|,)\s*/', "$1\n$indent", $sql);
  // Add new lines before AND
  $sql = preg_replace('/\s*(AND |INNER JOIN |LEFT JOIN |RIGHT JOIN |NATURAL JOIN )\s*/', "\n$indent$1", $sql);
  // Separate ', ' into multiple lines
  $sql = preg_replace('/\s*(\',) (\')\s*/', "$1\n$2", $sql);
  // Put a new query on a new line
  $sql = preg_replace('/; /', ";\n", $sql);
  return sprintf('<pre>%s</pre>', $sql);
}
