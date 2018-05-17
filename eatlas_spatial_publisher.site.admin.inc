<?php

/**
 * List all sites, in a table
 */
function _eatlas_spatial_publisher_overview() {
	$output = '<h3>' . t('eAtlas Spatial Publisher overview') . '</h3>';
	$header = array(t('Title'), t('Operations'));

	$site_terms = eatlas_spatial_publisher_site_load_terms();

	if ($site_terms === NULL) {
		$output .= theme('table', array(
				'header' => $header,
				'rows' => array(),
				'empty' => t('Vocabulary %vocabulary is missing!', array('%vocabulary' => EATLAS_SPATIAL_PUBLISHER_VOCABULARY))
		));
	} else {
		// Load all the sites
		$rows = array();
		foreach ($site_terms as $site_term) {
			$rows[] = array(
				$site_term->name,
				l(t('edit'), "admin/config/eatlas/eatlas_spatial_publisher/$site_term->tid")
			);
		}

		$output .= theme('table', array(
				'header' => $header,
				'rows' => $rows,
				'empty' => t('No Spatial Publisher site available.')
		));
	}

	return $output;
}


/**
 * Display a personalised title for "Edit Spatial Publisher site" pages
 */
function _eatlas_spatial_publisher_site_title(stdClass $taxonomy_term) {
	return t('Edit @spatial_publisher Spatial Publisher site', array(
		'@spatial_publisher' => $taxonomy_term->name
	));
}

/**
 * Add / edit / delete site form
 */
function _eatlas_spatial_publisher_site_form($form, &$form_state, stdClass $taxonomy_term = NULL) {
	$publisher_vocabulary = taxonomy_vocabulary_machine_name_load(EATLAS_SPATIAL_PUBLISHER_VOCABULARY);
	if (!$publisher_vocabulary) {
		return;
	}

	$tid = $taxonomy_term === NULL ? NULL : $taxonomy_term->tid;

	$values = eatlas_spatial_publisher_site_load($tid);

	// https://api.drupal.org/api/drupal/developer%21topics%21forms_api_reference.html/7
	$form = array();

	// Check for confirmation forms.
	if (isset($form_state['confirm_delete'])) {
		return array_merge($form, _eatlas_spatial_publisher_site_form_confirm_delete($form, $form_state, $tid));
	}

	if ($taxonomy_term !== NULL) {

		// Add the table of region set
		$region_sets = eatlas_spatial_publisher_region_set_load_all($tid);
		$rs_rows = array();
		foreach ($region_sets as $region_set) {
			$rs_rows[] = array(
				$region_set->title,
				l(t('edit'), "node/$region_set->nid/edit")
			);
		}
		$region_sets_table = '<h3>' . t('%publisher region sets', array('%publisher' => $taxonomy_term->name)) . '</h3>' .
			theme('table', array(
				'header' => array(t('Title'), t('Operations')),
				'rows' => $rs_rows,
				'empty' => t('No region set available.')
			));


		// Add the table of supplementary CSV files
		$extra_csv_files = eatlas_spatial_publisher_extra_csv_load_all($tid);
		$csv_rows = array();
		foreach ($extra_csv_files as $extra_csv_file) {
			$csv_rows[] = array(
				$extra_csv_file->variable,
				l(t('edit'), "admin/config/eatlas/eatlas_spatial_publisher/$tid/extra_csv/$extra_csv_file->variable/edit") // TODO wrong URL
			);
		}
		$extra_csv_files_table = '<h3>' . t('%publisher supplementary CSV files', array('%publisher' => $taxonomy_term->name)) . '</h3>' .
			theme('table', array(
				'header' => array(t('Variable'), t('Operations')),
				'rows' => $csv_rows,
				'empty' => t('No CSV file available.')
			));


		$form['#prefix'] = $region_sets_table . $extra_csv_files_table;
	}

	// Taxonomy field
	$form['name'] = array(
		'#type' => 'textfield',
		'#title' => t('Name'),
		'#description' => t('Taxonomy term name.'),
		'#default_value' => ($taxonomy_term && property_exists($taxonomy_term, 'name')) ? $taxonomy_term->name : ''
	);

	// Menus in which we want to splice the region menu
	$menus_options = menu_get_menus();
	array_unshift($menus_options, '-- None --');
	if (!empty($menus_options)) {
		$form['region_menus'] = array(
			'#title' => t('Region menus'),
			'#description' => t('Menus that require a list of regions. ' .
					'The list will be incorporated into the selected menus at index 0.') .
					'<br>' .
					t('<strong>NOTE</strong>: To decide where the region sub-menu will appear in the menu, ' .
					'you must manually change the menu item\'s weight by clicking ' .
					'the <em>Show row weights</em> link from the menu\'s <em>List links</em> tab ' .
					'and choose <em>Weights</em> above or bellow 0.'),
			'#type' => 'select',
			'#multiple' => TRUE,
			'#size' => 10, // Number of "lines"
			'#default_value' => property_exists($values, 'region_menus') ? $values->region_menus : array(),
			'#options' => $menus_options
		);
	}

	// Custom fields (saved in "eatlas_spatial_publisher_site" table)
	/*
	$form['theme'] = array(
		'#type' => 'textfield',
		'#title' => t('Theme'),
		'#description' => t('Sub-website theme.'),
		'#default_value' => property_exists($values, 'theme') ? $values->theme : ''
	);
	*/

	// Initial zoom bounding box
	$form['bbox_north'] = array(
		'#type' => 'textfield',
		'#attributes' => array(
			// NOTE: The space before 'type' is required (?)
			' type' => 'number'
		),
		'#title' => t('North'),
		'#description' => t('Initial zoom bounding box. If one of the coordinate is missing, the bounding box of the GeoJSON will be used.'),
		'#default_value' => property_exists($values, 'bbox_north') ? $values->bbox_north : ''
	);
	$form['bbox_east'] = array(
		'#type' => 'textfield',
		'#attributes' => array(
			// NOTE: The space before 'type' is required (?)
			' type' => 'number'
		),
		'#title' => t('East'),
		'#description' => t('Initial zoom bounding box. If one of the coordinate is missing, the bounding box of the GeoJSON will be used.'),
		'#default_value' => property_exists($values, 'bbox_east') ? $values->bbox_east : ''
	);
	$form['bbox_south'] = array(
		'#type' => 'textfield',
		'#attributes' => array(
			// NOTE: The space before 'type' is required (?)
			' type' => 'number'
		),
		'#title' => t('South'),
		'#description' => t('Initial zoom bounding box. If one of the coordinate is missing, the bounding box of the GeoJSON will be used.'),
		'#default_value' => property_exists($values, 'bbox_south') ? $values->bbox_south : ''
	);
	$form['bbox_west'] = array(
		'#type' => 'textfield',
		'#attributes' => array(
			// NOTE: The space before 'type' is required (?)
			' type' => 'number'
		),
		'#title' => t('West'),
		'#description' => t('Initial zoom bounding box. If one of the coordinate is missing, the bounding box of the GeoJSON will be used.'),
		'#default_value' => property_exists($values, 'bbox_west') ? $values->bbox_west : ''
	);


	$form['actions'] = array('#type' => 'actions');
	$form['actions']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save')
	);

	$form['tid'] = array(
		'#type' => 'hidden',
		'#value' => $tid
	);

	$form['actions']['delete'] = array(
		'#type' => 'submit',
		'#value' => t('Delete'),
		'#access' => _eatlas_spatial_publisher_sites_admin_access(),
		'#weight' => 10,
	);

	return $form;
}

/**
 * Drupal magic: <form ID>_validate
 * Validate add / edit / delete site form
 */
function _eatlas_spatial_publisher_site_form_validate($form, &$form_state) {}

/**
 * Drupal magic: <form ID>_submit
 * Submit add / edit / delete site form
 */
function _eatlas_spatial_publisher_site_form_submit($form, &$form_state) {
	$form_state['redirect'] = 'admin/config/eatlas/eatlas_spatial_publisher';

	if ($form_state['triggering_element']['#value'] == t('Delete')) {
		// Execute the term deletion.
		if ($form_state['values']['delete'] === TRUE) {
			return _eatlas_spatial_publisher_site_form_confirm_delete_submit($form, $form_state);
		}
		// Rebuild the form to confirm term deletion.
		$form_state['rebuild'] = TRUE;
		$form_state['confirm_delete'] = TRUE;
		return;
	}

	$publisher_site = NULL;
	if (isset($form_state['values']['tid'])) {
		$publisher_site = eatlas_spatial_publisher_site_load($form_state['values']['tid']);
		$publisher_site->tid = $form_state['values']['tid'];
	} else {
		$publisher_site = new stdClass();
	}

	$publisher_site->name = $form_state['values']['name'];
	$publisher_site->region_menus = $form_state['values']['region_menus'];
	//$publisher_site->theme = $form_state['values']['theme'];

	$publisher_site->bbox_north = _eatlas_spatial_publisher_site_get_form_state_value($form_state, 'bbox_north');
	$publisher_site->bbox_east  = _eatlas_spatial_publisher_site_get_form_state_value($form_state, 'bbox_east');
	$publisher_site->bbox_south = _eatlas_spatial_publisher_site_get_form_state_value($form_state, 'bbox_south');
	$publisher_site->bbox_west  = _eatlas_spatial_publisher_site_get_form_state_value($form_state, 'bbox_west');

	eatlas_spatial_publisher_site_save($publisher_site);
}

function _eatlas_spatial_publisher_site_get_form_state_value($form_state, $field_name) {
	$value = trim($form_state['values'][$field_name]);
	if ($value === '') {
		return NULL;
	}
	return $value;
}

/**
 * Drupal magic: <form ID>_confirm_delete
 * Confirm deletion of a site
 * Inspired from:
 *   taxonomy.admin.inc: taxonomy_term_confirm_delete
 */
function _eatlas_spatial_publisher_site_form_confirm_delete($form, &$form_state, $tid) {
	$taxonomy_term = taxonomy_term_load($tid);

	// Always provide entity id in the same form key as in the entity edit form.
	$form['tid'] = array('#type' => 'value', '#value' => $tid);
	$form['delete'] = array('#type' => 'value', '#value' => TRUE);

	return confirm_form(
		$form,
		t('Are you sure you want to delete the Spatial Publisher site %title?',
			array('%title' => $taxonomy_term->name)
		),
		'admin/config/eatlas/eatlas_spatial_publisher',
		t('Deleting a Spatial Publisher site will delete all its regions if there are any. ' .
			'The website will be detached from its theme and it\'s likely to become inaccessible. ' .
			'This action cannot be undone.'),
		t('Delete'),
		t('Cancel')
	);
}

/**
 * Submission of the confirm deletion of a site
 * Called from: _eatlas_spatial_publisher_site_form_submit
 */
function _eatlas_spatial_publisher_site_form_confirm_delete_submit($form, &$form_state) {
	// Delete the site taxonomy term (and its children)
	// NOTE: A listener on taxonomy term deletion will be trigger for deleting associated metadata.
	eatlas_spatial_publisher_delete_taxonomy_term($form_state['values']['tid']);
	$form_state['redirect'] = 'admin/config/eatlas/eatlas_spatial_publisher';
	return;
}

?>
