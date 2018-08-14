<?php

// The region file define function which needs to be called in here,
// such as "eatlas_spatial_publisher_regions_save".
include_once('eatlas_spatial_publisher.region.admin.inc');
include_once('eatlas_spatial_publisher_template_parser.inc');

/**
 * Definition of the "Content type" used for "Region set"
 *   eatlas_publisher_region_set
 */

/**
 * Implements hook_node_info().
 * Define a new content type.
 *   https://api.drupal.org/api/drupal/modules%21node%21node.api.php/function/hook_node_info/7.x
 */
function eatlas_spatial_publisher_node_info() {
	return array(
		EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE => array(
			'name' => t('eAtlas Region set'),
			'base' => 'node_content', // Needed to be displayed within "Structure > Content types"
			'description' => t('A Spatial Publisher Region set'),
			'locked' => FALSE
		)
	);
}

/**
 * Implements hook_node_type_insert().
 *   https://www.drupal.org/docs/7/api/field-api/field-types-and-settings
 * Adding fields to the new content type
 * NOTE: This is done when the content type is created (when the module is installed)
 *   To run this again, the module needs to be uninstalled and re-installed.
 *   Uninstalling the module delete the content type and all nodes created with it.
 */
function eatlas_spatial_publisher_node_type_insert($content_type) {
	if ($content_type->type == EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE) {

		// Preview image
		$preview_image_config = array(
			'field_name' => 'field_preview', // Max 32 characters!
			'type' => 'file',
			'label' => t('Preview image'),
			'description' => t('This image is used as part of the article teaser on the site front page, search results and in article lists. The preview image is also used as a banner image for the article, which you can disable with the "Show banner" check box if necessary.'),
			'settings' => array(
				'file_extensions' => 'jpg jpeg gif png ico'
			),
			'required' => FALSE
		);
		if (module_exists('media')) {
			$preview_image_config['widget'] = array(
				'type' => 'media_generic',
				'module' => 'media',
				'settings' => array(
					'allowed_types' => array(
						'image' => 'image'
					),
					'allowed_schemes' => array(
						'public' => 'public'
					),
					'progress_indicator' => 'throbber'
				)
			);
		}
		_eatlas_spatial_publisher_create_node_field($preview_image_config, EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE);

		// Add body field
		node_add_body_field($content_type, t('Introduction article'));

		// Add other fields
		_eatlas_spatial_publisher_create_node_field(array(
			'field_name' => 'field_eatlas_publisher_geojson', // Max 32 characters!
			'type' => 'file',
			'label' => t('GeoJSON file'),
			'description' => t('GeoJSON file containing the polygons of the regions.'),
			'settings' => array(
				'file_extensions' => 'geojson',
				'file_directory'  => 'spatial_publisher_uploads'
			),
			'required' => FALSE
		), EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE);

		_eatlas_spatial_publisher_create_node_field(array(
			'field_name' => 'field_eatlas_publisher_id_col', // Max 32 characters!
			'type' => 'text',
			'label' => t('ID column name'),
			'description' => t('Name of the column which contains the ID of the region.'),
			'required' => TRUE
		), EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE);

		_eatlas_spatial_publisher_create_node_field(array(
			'field_name' => 'field_eatlas_publisher_pid_col', // Max 32 characters!
			'type' => 'text',
			'label' => t('Parent ID column name'),
			'description' => t('Name of the column which contains the ID of parent of the region, if any. NOTE: This field is currently ignored.'),
			'required' => FALSE
		), EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE);

		_eatlas_spatial_publisher_create_node_field(array(
			'field_name' => 'field_eatlas_publisher_csv', // Max 32 characters!
			'type' => 'file',
			'label' => t('CSV metadata file'),
			'description' => t('CSV file containing extra metadata for polygons defined in the GeoJSON file.'),
			'settings' => array(
				'file_extensions' => 'csv',
				'file_directory'  => 'spatial_publisher_uploads'
			),
			'required' => FALSE
		), EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE);

		_eatlas_spatial_publisher_create_node_field(array(
			'field_name' => 'field_eatlas_publisher_template', // Max 32 characters!
			'type' => 'text_long',
			'widget' => array('type' => 'text_textarea'),
			'label' => t('Template'),
			'description' => t('Template used to display the metadata.'),
			'required' => FALSE
		), EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE);

		_eatlas_spatial_publisher_create_node_field(array(
			'field_name' => 'field_eatlas_publisher_baselayer', // Max 32 characters!
			'type' => 'text_long',
			'widget' => array('type' => 'text_textarea'),
			'label' => t('Map Base layer'),
			'description' => t('An OpenLayer layer object to use as a Base layer. If none is specified, OSM layer will be used.'),
			'required' => FALSE
		), EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE);

		_eatlas_spatial_publisher_create_node_field(array(
			'field_name' => 'field_eatlas_publisher_style', // Max 32 characters!
			'type' => 'text_long',
			'widget' => array('type' => 'text_textarea'),
			'label' => t('Style'),
			'description' => t('The style used to draw the polygons.'),
			'required' => FALSE
		), EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE);

		_eatlas_spatial_publisher_create_node_field(array(
			'field_name' => 'field_eatlas_publisher_mo_style', // Max 32 characters!
			'type' => 'text_long',
			'widget' => array('type' => 'text_textarea'),
			'label' => t('Mouse over style'),
			'description' => t('The style used when the mouse move over a polygon. If none is specified, the lines will appear 3x thicker.'),
			'required' => FALSE
		), EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE);

		_eatlas_spatial_publisher_create_node_field(array(
			'field_name' => 'field_eatlas_publisher_hl_style', // Max 32 characters!
			'type' => 'text_long',
			'widget' => array('type' => 'text_textarea'),
			'label' => t('Selected style'),
			'description' => t('The style used to highlight the selected polygon. If none is specified, the lines will appear 3x thicker.'),
			'required' => FALSE
		), EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE);

		_eatlas_spatial_publisher_create_node_field(array(
			'field_name' => 'field_eatlas_publisher_tid', // Max 32 characters!
			'type' => 'list_text',
			'label' => t('Site'),
			'description' => t('The site associated with this region set.'),
			'settings' => array(
				'allowed_values_function' => '_eatlas_spatial_publisher_get_site_options'
			),
			'required' => TRUE
		), EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE);

		_eatlas_spatial_publisher_create_node_field(array(
			'field_name' => 'field_eatlas_publisher_parent', // Max 32 characters!
			'type' => 'number_integer',
			'label' => t('Parent'),
			'description' => t('Parent region set node ID. Leave blank to create a new Map.'),
			'required' => FALSE
		), EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE);

		node_type_save($content_type);

		// Disable comments
		variable_set('comment_' . EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE, 0);
	}
}

function _eatlas_spatial_publisher_regions_set_get_geojson(stdClass $node) {
	// We are returning JSON, so tell the browser.
	drupal_add_http_header('Content-Type', 'application/json');

	$geojson_str = '';
	if ($cache = cache_get(EATLAS_SPATIAL_PUBLISHER_NODE_CACHE_PREFIX . $node->nid)) {
		$geojson_str = $cache->data;
	} else {
		// TODO Recursive

		$geojson_content = _eatlas_spatial_publisher_regions_set_get_geojson_content($node);

		$id_column_name = eatlas_commons_get_entity_value('node', $node, 'field_eatlas_publisher_id_col');
		$parent_id_column_name = eatlas_commons_get_entity_value('node', $node, 'field_eatlas_publisher_pid_col');
		$label_column_name = eatlas_commons_get_entity_value('node', $node, 'field_eatlas_publisher_label_col');

		// Build the feature map
		$feature_map = array();
		if ($geojson_content && $geojson_content['features']) {

			usort($geojson_content['features'], '_eatlas_spatial_publisher_regions_set_sort_features');

			foreach($geojson_content['features'] as &$feature) {
				if (isset($feature['properties'][$id_column_name])) {
					$feature['properties']['_childNodes'] = array();
					$feature['title'] = isset($feature['properties'][$label_column_name]) ? $feature['properties'][$label_column_name] : t('Untitled');
					$feature['nid'] = $node->nid;
					$feature['properties']['parent_id'] = isset($feature['properties'][$parent_id_column_name]) ? $feature['properties'][$parent_id_column_name] : NULL;
					$feature_map[$feature['properties'][$id_column_name]] = &$feature;
				}
			}
		}

		// Add children, if any
		$child_nodes = eatlas_spatial_publisher_region_set_load_children($node);
		if ($child_nodes) {
			foreach($child_nodes as $child_node) {
				$child_features_arr = _eatlas_spatial_publisher_regions_set_get_geojson_content($child_node);
				if ($child_features_arr && $child_features_arr['features']) {

					//$child_id_column_name = eatlas_commons_get_entity_value('node', $child_node, 'field_eatlas_publisher_id_col');
					$child_parent_id_column_name = eatlas_commons_get_entity_value('node', $child_node, 'field_eatlas_publisher_pid_col');
					$child_label_column_name = eatlas_commons_get_entity_value('node', $child_node, 'field_eatlas_publisher_label_col');

					foreach($child_features_arr['features'] as $child_feature) {
						if (isset($child_feature['properties'][$child_parent_id_column_name])) {
							$parent = &$feature_map[$child_feature['properties'][$child_parent_id_column_name]];
							if ($parent) {
								$child_feature['title'] = isset($child_feature['properties'][$child_label_column_name]) ? $child_feature['properties'][$child_label_column_name] : t('Untitled');
								$child_feature['nid'] = $child_node->nid;
								$child_feature['properties']['parent_id'] = $parent['id'];
								$parent['properties']['_childNodes'][] = $child_feature;
							}
						}
					}
				}
			}
		}

		if (!empty($geojson_content)) {
			// Merge the CSV metadata to the GeoJSON features
			$geojson_str = drupal_json_encode($geojson_content);
		}

		// Cache the feature tree
		cache_set(EATLAS_SPATIAL_PUBLISHER_NODE_CACHE_PREFIX . $node->nid, $geojson_str);
	}

	if ($geojson_str) {
		print $geojson_str;
	}
}

function _eatlas_spatial_publisher_regions_set_sort_features($feature_a, $feature_b) {
	$feature_a_ignore = FALSE;
	if (isset($feature_a['properties']['menu_ignore']) && $feature_a['properties']['menu_ignore']) {
		$feature_a_ignore = TRUE;
	}

	$feature_b_ignore = FALSE;
	if (isset($feature_b['properties']['menu_ignore']) && $feature_b['properties']['menu_ignore']) {
		$feature_b_ignore = TRUE;
	}

	// If they are both ignore, or both NOT ignore, the order doesn't matter
	if ($feature_a_ignore === $feature_b_ignore) {
		return -1;
	}

	return $feature_a_ignore ? -1 : 1;
}

function _eatlas_spatial_publisher_regions_set_get_geojson_content(stdClass $node) {
	$geojson_file_id = _eatlas_spatial_publisher_region_get_fid($node, 'field_eatlas_publisher_geojson');
	$csv_file_id = _eatlas_spatial_publisher_region_get_fid($node, 'field_eatlas_publisher_csv');

	// GeoJSON
	$geojson_content = array();
	if ($geojson_file_id !== NULL) {
		// Load the Drupal file object associated with the file ID
		$geojson_file = file_load($geojson_file_id);

		// Parse the file content
		$geojson_content = _eatlas_spatial_publisher_region_parse_geojson($geojson_file);
	}

	if (!empty($geojson_content)) {
		// CSV
		$csv_content = NULL;
		if ($csv_file_id !== NULL) {
			// Load the Drupal file object associated with the file ID
			$csv_file = file_load($csv_file_id);

			// Parse the file content
			$csv_content = _eatlas_spatial_publisher_region_parse_csv($node, $csv_file);
		}

		// Merge the CSV metadata to the GeoJSON features
		return _eatlas_spatial_publisher_region_merge_geojson_csv($node, $geojson_content, $csv_content);
	}

	return NULL;
}

function eatlas_spatial_publisher_regions_set_get_feature_tree($site_tid) {
	$feature_map = array();

	$root_nodes = eatlas_spatial_publisher_region_set_load_roots($site_tid);

	if ($root_nodes) {
		foreach($root_nodes as $root_node) {
			// TODO Recursive
			// TODO Use _eatlas_spatial_publisher_regions_set_get_geojson($root_node)

			$features = _eatlas_spatial_publisher_regions_set_get_geojson_content($root_node);
			if ($features && $features['features']) {

				$id_column_name = eatlas_commons_get_entity_value('node', $root_node, 'field_eatlas_publisher_id_col');
				//$parent_id_column_name = eatlas_commons_get_entity_value('node', $root_node, 'field_eatlas_publisher_pid_col');
				$label_column_name = eatlas_commons_get_entity_value('node', $root_node, 'field_eatlas_publisher_label_col');

				foreach($features['features'] as $feature) {
					if (isset($feature['properties'][$id_column_name])) {
						$feature['children'] = array();
						$feature['title'] = isset($feature['properties'][$label_column_name]) ? $feature['properties'][$label_column_name] : t('Untitled');
						$feature['node'] = $root_node;
						$feature_map[$feature['properties'][$id_column_name]] = $feature;
					}
				}
			}

			$child_nodes = eatlas_spatial_publisher_region_set_load_children($root_node);
			if ($child_nodes) {
				foreach($child_nodes as $child_node) {
					$child_features_arr = _eatlas_spatial_publisher_regions_set_get_geojson_content($child_node);
					if ($child_features_arr && $child_features_arr['features']) {

						//$child_id_column_name = eatlas_commons_get_entity_value('node', $child_node, 'field_eatlas_publisher_id_col');
						$child_parent_id_column_name = eatlas_commons_get_entity_value('node', $child_node, 'field_eatlas_publisher_pid_col');
						$child_label_column_name = eatlas_commons_get_entity_value('node', $child_node, 'field_eatlas_publisher_label_col');

						foreach($child_features_arr['features'] as $child_feature) {
							if (isset($child_feature['properties'][$child_parent_id_column_name])) {
								$parent = &$feature_map[$child_feature['properties'][$child_parent_id_column_name]];
								// If the parent was not found, create an artificial parent to hold the child
								if (!$parent) {
									$parent = array(
										'id' => $child_feature['properties'][$child_parent_id_column_name],
										'virtual' => TRUE,
										'children' => array()
									);
									$feature_map[$child_feature['properties'][$child_parent_id_column_name]] = $parent;
								}
								$child_feature['properties']['parent_id'] = $parent['id'];
								$child_feature['title'] = isset($child_feature['properties'][$child_label_column_name]) ? $child_feature['properties'][$child_label_column_name] : t('Untitled');
								$child_feature['node'] = $child_node;
								$parent['children'][] = $child_feature;
							}
						}
					}
				}
			}
		}
	}

	return $feature_map;
}

// Find the node associated with a feature ID
function eatlas_spatial_publisher_regions_set_get_feature_node($site_tid, $feature_id) {
	if ($site_tid) {
		$feature_tree = eatlas_spatial_publisher_regions_set_get_feature_tree($site_tid);
		$feature_node = _eatlas_spatial_publisher_regions_set_find_feature_node($feature_tree, $feature_id);
		return $feature_node;
	}
	return NULL;
}

// Find the node associated with a feature ID, from the feature tree (as returned by eatlas_spatial_publisher_regions_set_get_feature_tree)
function _eatlas_spatial_publisher_regions_set_find_feature_node($feature_tree, $feature_id) {
	if ($feature_tree && count($feature_tree) > 0) {
		foreach ($feature_tree as $current_feature) {
			$current_feature_id = isset($current_feature['id']) ? $current_feature['id'] : NULL;
			if ($current_feature_id === $feature_id) {
				return $current_feature['node'];
			}
			if (isset($current_feature['children']) && count($current_feature['children']) > 0) {
				$branch_found_node = _eatlas_spatial_publisher_regions_set_find_feature_node($current_feature['children'], $feature_id);
				if ($branch_found_node !== NULL) {
					return $branch_found_node;
				}
			}
		}
	}
	return NULL;
}

function _eatlas_spatial_publisher_regions_set_get_feature_template($node, $feature_id) {
	// http://aims.ereefs.localhost/eatlas_spatial_publisher/4003/feature/qld

	$feature = eatlas_spatial_publisher_get_feature_from_node($node, $feature_id);
	// Put the content of the template in a "section" to prevent unbalanced code from messing the page
	// NOTE: It's unlikely that the template contains "section" tags.
	//   When the browser sees the end of the section tag,
	//   it closes everything that hasn't been closed in the section.
	print '<section>' . "\n";

	print '<!-- Apply same style as the body field of an article (adjust line-height) -->' . "\n";
	print '<div class="field field-name-body field-type-text-with-summary">' . "\n";

	if ($node && $feature) {
		// Output the feature in the source, to be used as a reference while making the template
		// TODO Give access to this in the backend, when making the template!
		//print "<!--\nFEATURE\n" . print_r($feature, TRUE) . "\n-->";

		eAtlas_spatial_publisher_eval_template($node, $feature);
	} else {
		print '<div>Feature not found.</div>' . "\n";
	}

	print '</div> <!-- End of field-type-text-with-summary -->' . "\n";

	print '</section>' . "\n";
}

/**
 * Internal helper function
 * List all the Spatial publisher sites
 * (first level of the Spatial Publisher vocabulary - taxonomy module)
 * This is done as a function so it can be run every time the drop down
 * is rendered, ensuring the list is always up to date.
 * NOTE: If the list was creating during the creation of the
 *   "field_eatlas_publisher_tid" field (see: eatlas_spatial_publisher_node_type_insert),
 *   the list would be static and would not update when sites are
 *   deleted or new sites are created.
 */
function _eatlas_spatial_publisher_get_site_options() {
	$site_terms = eatlas_spatial_publisher_site_load_terms();
	$sites = array();
	foreach ($site_terms as $site_term) {
		$sites[$site_term->tid] = $site_term->name;
	}
	return $sites;
}

/**
 * Internal helper function
 * Creating a field in Drupal is done in 2 operations.
 * 1. Creating the form field (field_create_field).
 * 2. Associating the form field with a content type (field_create_instance).
 * It's done that way to allow fields to be reused. In our case, we want the
 *   fields to be unique for our content type, so we can do the
 *   creation / association in one function.
 * NOTE: field_create_field and field_create_instance share some
 *   attributes and ignore the attributes that they don't know, so it make
 *   sense to merge the attributes in one configuration object (for simplicity).
 */
function _eatlas_spatial_publisher_create_node_field($field_config, $bundle_name) {
	$field_name = $field_config['field_name'];

	$field = field_info_field($field_name);
	// If the field do not exists
	if (!$field) {
		$field = field_create_field($field_config);
		watchdog('eatlas_spatial_publisher', 'Field "'.$field_name.'" created');
	} else {
		watchdog('eatlas_spatial_publisher', 'Field "'.$field_name.'" already exists');
	}

	$instance = field_info_instance('node', $field_name, $bundle_name);
	if (!$instance){
		$field_instance_config = $field_config;
		$field_instance_config['entity_type'] = 'node';
		$field_instance_config['bundle'] = $bundle_name;
		field_create_instance($field_instance_config);
		watchdog('eatlas_spatial_publisher', 'Field "'.$field_name.'" added to "'.$bundle_name.'".');
	} else {
		watchdog('eatlas_spatial_publisher', 'Field "'.$field_name.'" is already setup for "'.$bundle_name.'".');
	}
}

/**
 * API function
 * List all Region Sets associated with a site.
 */
function eatlas_spatial_publisher_region_set_load_all($site_tid) {
	if ($site_tid === NULL) {
		return array();
	}

	$query = new EntityFieldQuery();
	$result = $query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE)
		->fieldCondition('field_eatlas_publisher_tid', 'value', $site_tid, '=')
		->execute();

	if (isset($result['node']) && !empty($result['node'])) {
		return node_load_multiple(array_keys($result['node']));
	}

	return array();
}


function eatlas_spatial_publisher_region_set_load_roots($site_tid) {
	if ($site_tid === NULL) {
		return array();
	}

	// Load all region sets for the given site ID
	$query = new EntityFieldQuery();
	// NOTE: EntityFieldQuery can't select field_eatlas_publisher_parent IS NULL
	//   because it join tables using INNER JOIN instead of LEFT JOIN.
	//     https://www.drupal.org/node/1226622
	$result = $query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE)
		->fieldCondition('field_eatlas_publisher_tid', 'value', $site_tid, '=')
		->execute();

	if (isset($result['node']) && !empty($result['node'])) {
		$nodes = node_load_multiple(array_keys($result['node']));

		$root_nodes = array();
		foreach ($nodes as $node) {
			// Get the parent NID (Drupal API really lack support for doing this)
			$parent_nid = NULL;
			if (property_exists($node, 'field_eatlas_publisher_parent') &&
					!empty($node->field_eatlas_publisher_parent) &&
					!empty($node->field_eatlas_publisher_parent[LANGUAGE_NONE]) &&
					!empty($node->field_eatlas_publisher_parent[LANGUAGE_NONE][0]) &&
					!empty($node->field_eatlas_publisher_parent[LANGUAGE_NONE][0]['value'])) {
				$parent_nid = $node->field_eatlas_publisher_parent[LANGUAGE_NONE][0]['value'];
			}

			if ($parent_nid === NULL) {
				// If the node doesn't have a parent ID, the node is considered
				//   a root and is added to the list.
				$root_nodes[] = $node;
			}
		}

		return $root_nodes;
	}

	return array();
}


function eatlas_spatial_publisher_region_set_load_children($region_set_node) {
	if ($region_set_node === NULL) {
		return array();
	}

	$site_tid = NULL;
	if (property_exists($region_set_node, 'field_eatlas_publisher_tid') &&
			!empty($region_set_node->field_eatlas_publisher_tid) &&
			!empty($region_set_node->field_eatlas_publisher_tid[LANGUAGE_NONE]) &&
			!empty($region_set_node->field_eatlas_publisher_tid[LANGUAGE_NONE][0]) &&
			!empty($region_set_node->field_eatlas_publisher_tid[LANGUAGE_NONE][0]['value'])) {
		$site_tid = $region_set_node->field_eatlas_publisher_tid[LANGUAGE_NONE][0]['value'];
	}


	// Load all region sets for the given site ID
	$query = new EntityFieldQuery();
	$result = $query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE)
		->fieldCondition('field_eatlas_publisher_tid', 'value', $site_tid, '=')
		->execute();

	if (isset($result['node']) && !empty($result['node'])) {
		$nodes = node_load_multiple(array_keys($result['node']));

		$child_nodes = array();
		foreach ($nodes as $node) {
			// Get the parent NID (Drupal API really lack support for doing this)
			$parent_nid = NULL;
			if (property_exists($node, 'field_eatlas_publisher_parent') &&
					!empty($node->field_eatlas_publisher_parent) &&
					!empty($node->field_eatlas_publisher_parent[LANGUAGE_NONE]) &&
					!empty($node->field_eatlas_publisher_parent[LANGUAGE_NONE][0]) &&
					!empty($node->field_eatlas_publisher_parent[LANGUAGE_NONE][0]['value'])) {
				$parent_nid = $node->field_eatlas_publisher_parent[LANGUAGE_NONE][0]['value'];
			}

			if ($parent_nid === $region_set_node->nid) {
				$child_nodes[] = $node;
			}
		}

		return $child_nodes;
	}

	return array();
}




/**
 * Redirect to the node add page for the Region set content type,
 *   with a site_tid URL parameter.
 * The form is altered (see: "eatlas_spatial_publisher_form_eatlas_publisher_region_set_node_form_alter")
 *   to use that parameter to set the site_tid dropdown value.
 */
function _eatlas_spatial_publisher_add_region_set(stdClass $site_taxonomy_term) {
	$site_tid = $site_taxonomy_term->tid;
	drupal_goto('node/add/eatlas-publisher-region-set', array('query' => array('site_tid' => $site_tid)));
}

/**
 * Implementations of hook_form_FORM_ID_alter.
 * This hook is used to dynamically modify a given form.
 *   It has less overhead then "hook_form_alter" since it's only called for
 *   a single form.
 */

/**
 * Implements hook_form_FORM_ID_alter for
 *   FORM_ID = eatlas_publisher_region_set_node_form
 * Tells Drupal to call a function to redirect to the
 *   eatlas_spatial_publisher configuration page after the add / edit form
 *   was successfully submitted.
 * NOTE: I tried every single suggestion of form redirection, none works.
 *   I suspect the node module set its own redirection after executing all
 *   the "submit" hooks, which overwrites my settings.
 *   I ended up having to use the "destination" URL parameter.
 *   It feels like a hack, but it works, and it doesn't get in the way of the
 *   validation process (some solutions redirect even when the validation fails).
 */
function eatlas_spatial_publisher_form_eatlas_publisher_region_set_node_form_alter(&$form, &$form_state, $form_id) {
	// Set the site ID using the URL parameter
	if (isset($_GET['site_tid']) && is_numeric($_GET['site_tid'])) {
		$site_tid = $_GET['site_tid'];

		if (isset($form['field_eatlas_publisher_tid'])) {
			$form['field_eatlas_publisher_tid'][LANGUAGE_NONE]['#default_value'] = $site_tid;
		}
	}

	$form['#submit'][] = '_eatlas_spatial_publisher_edit_form_submit';
}
/**
 * Implements hook_form_FORM_ID_alter for
 *   FORM_ID = node_delete_confirm
 * Tells Drupal to call a function to redirect to the
 *   eatlas_spatial_publisher configuration page after the delete form
 *   was successfully submitted.
 * It also modify the "Cancel" button's link to redirect
 *   to the same eatlas_spatial_publisher configuration page.
 */
function eatlas_spatial_publisher_form_node_delete_confirm_alter(&$form, &$form_state, $form_id) {
	// Fix the cancel button
	if (isset($form['#node'])) {
		$node = $form['#node'];
		if (property_exists($node, 'field_eatlas_publisher_tid')) {
			$site_tid = $node->field_eatlas_publisher_tid[LANGUAGE_NONE][0]['value'];
			$form['actions']['cancel']['#href'] = 'admin/config/eatlas/eatlas_spatial_publisher/' . $site_tid;
		}
	}
	$form['#submit'][] = '_eatlas_spatial_publisher_delete_form_submit';
}

/**
 * Change the page the Spatial Publisher content type edit form will
 *   redirect to after submitting the form (creation and edit).
 * NOTE: We want the form to redirect to the module configuration page
 *   instead of the node view page.
 */
function _eatlas_spatial_publisher_edit_form_submit($form, &$form_state) {
	if (isset($form_state['values']['field_eatlas_publisher_tid'])) {
		$site_tid = $form_state['values']['field_eatlas_publisher_tid'][LANGUAGE_NONE][0]['value'];
		// NOTE: $form_state['redirect'] can't be used here, the node module overwrites it.
		$_GET['destination'] = 'admin/config/eatlas/eatlas_spatial_publisher/' . $site_tid;
	}
}

/**
 * Change the page the Spatial Publisher content type delete form will
 *   redirect to after submitting the form (delete only).
 * NOTE: We want the form to redirect to the module configuration page
 *   instead of the site home page.
 */
function _eatlas_spatial_publisher_delete_form_submit($form, &$form_state) {
	if (isset($form['#node'])) {
		$node = $form['#node'];
		if (property_exists($node, 'field_eatlas_publisher_tid')) {
			$site_tid = $node->field_eatlas_publisher_tid[LANGUAGE_NONE][0]['value'];
			$form_state['redirect'] = 'admin/config/eatlas/eatlas_spatial_publisher/' . $site_tid;
		}
	}
}

/**
 * Implements hook_node_insert($node).
 *   Update regions when the GeoJSON / CSV file are updated.
 * See: https://api.drupal.org/api/drupal/modules!node!node.api.php/function/hook_node_insert/7.x
 */
function eatlas_spatial_publisher_node_insert($node) {
	if ($node->type === EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE) {
		eatlas_spatial_publisher_regions_save($node);
	}
}

/**
 * Implements hook_node_update($node).
 *   Update regions when the GeoJSON / CSV file are updated.
 * See: https://api.drupal.org/api/drupal/modules!node!node.api.php/function/hook_node_update/7.x
 */
function eatlas_spatial_publisher_node_update($node) {
	if ($node->type === EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE) {
		eatlas_spatial_publisher_regions_save($node);
	}
}

/**
 * Implements hook_node_delete($node).
 *   Delete associated region (taxonomy terms)
 * See: https://api.drupal.org/api/drupal/modules!node!node.api.php/function/hook_node_delete/7.x
 */
function eatlas_spatial_publisher_node_delete($node) {
	if ($node->type === EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE) {
		cache_clear_all(EATLAS_SPATIAL_PUBLISHER_NODE_CACHE_PREFIX . $node->nid, 'cache');

		$geojson_file_id = _eatlas_spatial_publisher_region_get_fid($node, 'field_eatlas_publisher_geojson');
		cache_clear_all(EATLAS_SPATIAL_PUBLISHER_FILE_CACHE_PREFIX . $geojson_file_id, 'cache');

		$csv_file_id = _eatlas_spatial_publisher_region_get_fid($node, 'field_eatlas_publisher_csv');
		cache_clear_all(EATLAS_SPATIAL_PUBLISHER_FILE_CACHE_PREFIX . $csv_file_id, 'cache');

		$region_terms = eatlas_spatial_publisher_region_load_terms($node->nid);

		// Delete all the region terms
		if (!empty($region_terms)) {
			foreach ($region_terms as $region_term) {
				eatlas_spatial_publisher_delete_taxonomy_term($region_term->tid);
			}
		}
	}
}

?>
