<?php

/**
 * Add a supplementary CSV files
 */
function _eatlas_spatial_publisher_extra_csv_form($form, &$form_state, stdClass $site_taxonomy_term, $variable = NULL) {
	$site_tid = $site_taxonomy_term->tid;

	$extra_csv = eatlas_spatial_publisher_extra_csv_load($site_tid, $variable);

	// Check for confirmation forms.
	if (isset($form_state['confirm_delete'])) {
		return array_merge($form, _eatlas_spatial_publisher_extra_csv_form_confirm_delete($form, $form_state, $site_tid, $variable));
	}

	// Form fields
	$form['variable'] = array(
		'#type' => 'textfield',
		'#title' => t('Variable name'),
		'#description' => t('The name of the variable that will be send to the template.'),
		'#default_value' => $variable ? $variable : '',
		// The variable name can name be changed (it's part of the PK)
		'#disabled' => !!$variable, // The "!!" is to make a Boolean variable
		'#required' => TRUE
	);

	if (module_exists('media')) {
		// See: https://www.drupal.org/node/2288487
		$form['csv'] = array(
			'#type'          => 'media',
			'#title'         => 'CSV file',
			'#description'   => 'The CSV file to attach to the variable.',
			'#default_value' => ($extra_csv && property_exists($extra_csv, 'csv')) ? array('fid' => $extra_csv->csv) : NULL, //The fid
			'#media_options' => array(
				'global' => array(
					'file_directory'  => 'eatlas_spatial_publisher_extra_csv',
					'file_extensions' => 'csv',
					'uri_scheme'      => 'public',
					'types'           => array('default'), // Type "Other" is call "default" internally
					'schemes'         => array(
						'public' => 'public',
					)
				)
			)
		);
	} else {
		$form['csv'] = array(
			'#type' => 'managed_file',
			'#title' => t('CSV file'),
			'#description' => t('The CSV file to attach to the variable.'),
			'#upload_location' => 'public://eatlas_spatial_publisher_extra_csv/',
			'#upload_validators' => array(
				'file_validate_extensions' => array('csv')
			),
			'#default_value' => ($extra_csv && property_exists($extra_csv, 'csv')) ? $extra_csv->csv : NULL
		);
	}

	// Form actions (buttons)
	$form['actions'] = array('#type' => 'actions');
	$form['actions']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save')
	);

	$form['tid'] = array(
		'#type' => 'hidden',
		'#value' => $site_tid
	);

	$form['actions']['delete'] = array(
		'#type' => 'submit',
		'#value' => t('Delete'),
		'#access' => _eatlas_spatial_publisher_site_edit_access($site_taxonomy_term),
		'#weight' => 10,
	);

	return $form;

}

/**
 * Drupal magic: <form ID>_validate
 * Validate add / edit / delete site form
 */
function _eatlas_spatial_publisher_extra_csv_form_validate($form, &$form_state) {
	$min_variable_name_length = 3;

	// Validate variable
	$variable = NULL;
	if (isset($form_state['values']['variable'])) {
		$variable = trim($form_state['values']['variable']);
	}
	if (!$variable) {
		form_set_error('variable', t('Variable name is required.'));
	} else if (strlen($variable) < $min_variable_name_length) {
		form_set_error('variable', t('Variable name too short. It needs to be at least ' . $min_variable_name_length . ' characters long.'));
	} else if (!filter_var($variable, FILTER_VALIDATE_REGEXP, array("options" => array("regexp" => "/^[a-zA-Z_][a-zA-Z0-9_]*$/")))) {
		form_set_error('variable', t('Invalid variable name. Allowed characters are letters, numbers and underscore "_". The variable name can NOT start with a number.'));
	}

	// Validate site tid
	//   This should not happen since the field is not modifiable by the user (hidden)
	if (!isset($form_state['values']['tid']) || !$form_state['values']['tid']) {
		form_set_error('tid', t('Invalid form state. Missing taxonomy term ID.'));
	}
}

/**
 * Drupal magic: <form ID>_submit
 * Submit add / edit / delete site form
 */
function _eatlas_spatial_publisher_extra_csv_form_submit($form, &$form_state) {
	// The following test is not necessary since the validation already test them, but it cost nothing to ensure they are set.
	if (isset($form_state['values']['tid']) && isset($form_state['values']['variable'])) {
		$site_tid = $form_state['values']['tid'];
		$form_state['redirect'] = 'admin/config/eatlas/eatlas_spatial_publisher/' . $site_tid;

		if ($form_state['triggering_element']['#value'] == t('Delete')) {
			// Execute the term deletion.
			if ($form_state['values']['delete'] === TRUE) {
				_eatlas_spatial_publisher_extra_csv_form_confirm_delete_submit($form, $form_state);
			} else {
				// Rebuild the form to confirm form deletion.
				$form_state['rebuild'] = TRUE;
				$form_state['confirm_delete'] = TRUE;
			}
		} else {
			$variable = trim($form_state['values']['variable']);

			$extra_csv = eatlas_spatial_publisher_extra_csv_load($site_tid, $variable);
			$old_csv_fid = NULL;
			if ($extra_csv) {
				if (property_exists($extra_csv, 'csv')) {
					$old_csv_fid = $extra_csv->csv;
				}
			} else {
				$extra_csv = new stdClass();
				$extra_csv->tid = $site_tid;
				$extra_csv->variable = $variable;
			}

			$csv_fid = NULL;
			if (isset($form_state['values']['fid'])) {
				// The media module has "fid" hardcoded in...
				$csv_fid = $form_state['values']['fid'];
			} else if (isset($form_state['values']['csv'])) {
				$csv_fid = $form_state['values']['csv'];
			}
			if ($old_csv_fid && $csv_fid !== $old_csv_fid) {
				cache_clear_all(EATLAS_SPATIAL_PUBLISHER_FILE_CACHE_PREFIX . $old_csv_fid, 'cache');
				eatlas_spatial_publisher_fix_csv_file($csv_fid);
			}
			$extra_csv->csv = $csv_fid;

			eatlas_spatial_publisher_extra_csv_save($extra_csv);
		}
	}
}

/**
 * Drupal magic: <form ID>_confirm_delete
 * Confirm deletion of a CSV
 */
function _eatlas_spatial_publisher_extra_csv_form_confirm_delete($form, &$form_state, $site_tid, $variable) {
	// Always provide entity id in the same form key as in the entity edit form.
	$form['tid'] = array('#type' => 'value', '#value' => $site_tid);
	$form['variable'] = array('#type' => 'value', '#value' => $variable);
	$form['delete'] = array('#type' => 'value', '#value' => TRUE);

	return confirm_form(
		$form,
		t('Are you sure you want to delete the supplementary CSV file %variable?',
			array('%variable' => $variable)
		),
		'admin/config/eatlas/eatlas_spatial_publisher/' . $site_tid,
		t('Deleting this supplementary CSV file entry will remove the variable %variable ' .
			'from the template context and will prevent the template from accessing ' .
			'the content of the CSV file. ' .
			'This action cannot be undone.',
			array('%variable' => $variable)),
		t('Delete'),
		t('Cancel')
	);
}

/**
 * Submission of the confirm deletion of a site
 * Called from: _eatlas_spatial_publisher_site_form_submit
 */
function _eatlas_spatial_publisher_extra_csv_form_confirm_delete_submit($form, &$form_state) {
	$site_tid = $form_state['values']['tid'];
	$variable = trim($form_state['values']['variable']);
	if ($site_tid && $variable) {
		eatlas_spatial_publisher_extra_csv_delete($site_tid, $variable);
	}
}


// Internal function
function eatlas_spatial_publisher_extra_csv_parse_csv($csv_fid) {
	if (!$csv_fid) {
		return array();
	}

	$csv_file = file_load($csv_fid);
	if (!$csv_file) {
		return array();
	}

	$csv_uri = $csv_file->uri;
	if (!$csv_uri) {
		return array();
	}

	$content_str = NULL;
	if ($cache = cache_get(EATLAS_SPATIAL_PUBLISHER_FILE_CACHE_PREFIX . $csv_fid)) {
		$content_str = $cache->data;
	} else {
		$content_str = file_get_contents($csv_uri);
		cache_set(EATLAS_SPATIAL_PUBLISHER_FILE_CACHE_PREFIX . $csv_fid, $content_str);
	}

	if (!$content_str) {
		return array();
	}

	$content = array();

	// Open a String file handle (maximum 50 MB)
	$maxsize = 50 * 1024 * 1024;
	$handle = fopen("php://temp/maxmemory:$maxsize", 'r+');
	fputs($handle, $content_str);
	rewind($handle);

	$row = fgetcsv($handle);

	// Parse CSV header (first row)
	$columns = array();
	foreach ($row as $i => $cell) {
		$columns[$i] = trim($cell);
	}

	// Parse file content
	while ($row = fgetcsv($handle)) {
		$record = array();
		foreach ($row as $i => $cell) {
			$cell_value = trim($cell);
			$record[$columns[$i]] = $cell_value;
		}
		$content[] = $record;
	}

	// Close the file handle
	fclose($handle);

	return $content;
}


/************************
 * Load / Save / Delete *
 ************************/

function eatlas_spatial_publisher_extra_csv_load_all($site_tid) {
	if (!$site_tid) {
		return array();
	}

	// DB Select using the Drupal objects
	// https://api.drupal.org/api/drupal/includes!database!database.inc/function/db_select/7
	$query = db_select(EATLAS_SPATIAL_PUBLISHER_DB_EXTRA_CSV_TABLE, 'csv')
		->fields('csv')
		->condition('csv.tid', $site_tid, '=');
	$result = $query->execute();

	$extra_csvs = array();
	while ($data = $result->fetch()) {
		$extra_csvs[] = $data;
	}

	return $extra_csvs;
}

function eatlas_spatial_publisher_extra_csv_load($site_tid, $variable) {
	if (!$site_tid || !$variable) {
		return NULL;
	}

	// DB Select using the Drupal objects
	// https://api.drupal.org/api/drupal/includes!database!database.inc/function/db_select/7
	$query = db_select(EATLAS_SPATIAL_PUBLISHER_DB_EXTRA_CSV_TABLE, 'csv')
		->fields('csv')
		->condition('csv.tid', $site_tid, '=')
		->condition('csv.variable', $variable, '=');
	$result = $query->execute();
	$data = $result->fetch();

	if (!$data) {
		return NULL;
	}

	return $data;
}

function eatlas_spatial_publisher_extra_csv_save($extra_csv) {
	// Transform the extra_csv object as an Array to be sent to the DB.
	$extra_csv_fields = (array)$extra_csv;

	$site_tid = $extra_csv_fields['tid'];
	$variable = $extra_csv_fields['variable'];

	if ($site_tid && $variable) {
		$exists = db_query('SELECT 1 FROM {'.EATLAS_SPATIAL_PUBLISHER_DB_EXTRA_CSV_TABLE.'} WHERE tid = :tid AND variable = :variable', array(':tid' => $site_tid, ':variable' => $variable))->fetchField();
		if ($exists === FALSE) {
			// Insert
			// https://api.drupal.org/api/drupal/includes%21database%21query.inc/function/InsertQuery%3A%3Aexecute/7
			db_insert(EATLAS_SPATIAL_PUBLISHER_DB_EXTRA_CSV_TABLE)->fields($extra_csv_fields)->execute();
		} else {
			// TID and variable are used in the "WHERE" clause, not in the values.
			unset($extra_csv_fields['tid']);
			unset($extra_csv_fields['variable']);

			// Update
			db_update(EATLAS_SPATIAL_PUBLISHER_DB_EXTRA_CSV_TABLE)->fields($extra_csv_fields)->condition('tid', $site_tid)->condition('variable', $variable)->execute();
		}
	}
}

function eatlas_spatial_publisher_extra_csv_delete($site_tid, $variable) {
	$extra_csv = eatlas_spatial_publisher_extra_csv_load($site_tid, $variable);
	if ($extra_csv && property_exists($extra_csv, 'csv')) {
		$csv_fid = $extra_csv->csv;
		cache_clear_all(EATLAS_SPATIAL_PUBLISHER_FILE_CACHE_PREFIX . $csv_fid, 'cache');
	}

	// https://api.drupal.org/api/drupal/includes%21database%21database.inc/function/db_delete/7.x
	db_delete(EATLAS_SPATIAL_PUBLISHER_DB_EXTRA_CSV_TABLE)->condition('tid', $site_tid)->condition('variable', $variable)->execute();
}

?>
