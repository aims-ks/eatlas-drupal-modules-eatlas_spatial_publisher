<?php

// API function
// Insert / Update / Delete regions associated with a region set node.
function eatlas_spatial_publisher_regions_save(stdClass $node) {
	$original_node = property_exists($node, 'original') ? $node->original : NULL;

	$geojson_file_id = _eatlas_spatial_publisher_region_get_fid($node, 'field_eatlas_publisher_geojson');
	$csv_file_id = _eatlas_spatial_publisher_region_get_fid($node, 'field_eatlas_publisher_csv');

	// Determine if the files have changed
	$dirty = empty($original_node);
	if (!empty($original_node)) {
		$original_geojson_file_id = _eatlas_spatial_publisher_region_get_fid($original_node, 'field_eatlas_publisher_geojson');
		$original_csv_file_id = _eatlas_spatial_publisher_region_get_fid($original_node, 'field_eatlas_publisher_csv');;

		if ($geojson_file_id !== $original_geojson_file_id) {
			cache_clear_all(EATLAS_SPATIAL_PUBLISHER_FILE_CACHE_PREFIX . $original_geojson_file_id, 'cache');
			$dirty = TRUE;
		}
		if ($csv_file_id !== $original_csv_file_id) {
			cache_clear_all(EATLAS_SPATIAL_PUBLISHER_FILE_CACHE_PREFIX . $original_csv_file_id, 'cache');
			eatlas_spatial_publisher_fix_csv_file($csv_file_id);
			$dirty = TRUE;
		}
	}

	if ($dirty) {
		// GeoJSON
		$geojson_content = NULL;
		if ($geojson_file_id !== NULL) {
			// Load the Drupal file object associated with the file ID
			$geojson_file = file_load($geojson_file_id);

			// Parse the file content
			$geojson_content = _eatlas_spatial_publisher_region_parse_geojson($geojson_file);
		}

		if (empty($geojson_content)) {
			// Delete all the region terms
			$region_terms = eatlas_spatial_publisher_region_load_terms($node->nid);
			if (!empty($region_terms)) {
				foreach ($region_terms as $region_term) {
					eatlas_spatial_publisher_delete_taxonomy_term($region_term->tid);
				}
			}
		} else {
			// CSV
			$csv_content = NULL;
			if ($csv_file_id !== NULL) {
				// Load the Drupal file object associated with the file ID
				$csv_file = file_load($csv_file_id);

				// Parse the file content
				$csv_content = _eatlas_spatial_publisher_region_parse_csv($node, $csv_file);
			}

			// Merge the CSV metadata to the GeoJSON features
			$geojson = _eatlas_spatial_publisher_region_merge_geojson_csv($node, $geojson_content, $csv_content);

			_eatlas_spatial_publisher_create_regions($node->nid, $geojson);
		}
	}
}

// Internal function
function _eatlas_spatial_publisher_region_get_fid($node, $field_name) {
	if (property_exists($node, $field_name) &&
			!empty($node->{$field_name}) &&
			!empty($node->{$field_name}[LANGUAGE_NONE]) &&
			!empty($node->{$field_name}[LANGUAGE_NONE][0]) &&
			!empty($node->{$field_name}[LANGUAGE_NONE][0]['fid'])) {
		return $node->{$field_name}[LANGUAGE_NONE][0]['fid'];
	}

	return NULL;
}

function _eatlas_spatial_publisher_region_get_value($node, $field_name) {
	if (property_exists($node, $field_name) &&
			!empty($node->{$field_name}) &&
			!empty($node->{$field_name}[LANGUAGE_NONE]) &&
			!empty($node->{$field_name}[LANGUAGE_NONE][0]) &&
			!empty($node->{$field_name}[LANGUAGE_NONE][0]['value'])) {
		return $node->{$field_name}[LANGUAGE_NONE][0]['value'];
	}

	return NULL;
}

// Internal function
function _eatlas_spatial_publisher_region_parse_csv(stdClass $node, $csv_file) {
	if (!$csv_file) {
		return array();
	}

	$csv_uri = $csv_file->uri;
	if (!$csv_uri) {
		return array();
	}

	$content_str = NULL;
	$csv_fid = $csv_file->fid;
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

	// Parse CSV header (first row)
	$id_column_number = NULL;
	$row = fgetcsv($handle);
	$columns = array();

	// $id_column: CSV column name for the id. Saved in "field_eatlas_publisher_id_col" field
	$id_column = _eatlas_spatial_publisher_region_get_value($node, 'field_eatlas_publisher_id_col');
	if ($id_column) {
		foreach ($row as $i => $cell) {
			$columns[$i] = trim($cell);
			if ($columns[$i] === $id_column) {
				$id_column_number = $i;
			}
		}

		if ($id_column_number !== NULL) {
			// Parse file content
			while ($row = fgetcsv($handle)) {
				$record = array();
				$id = NULL;
				foreach ($row as $i => $cell) {
					$cell_value = trim($cell);
					if ($i === $id_column_number) {
						$id = $cell_value;
					} elseif ($columns[$i]) {
						$record[$columns[$i]] = $cell_value;
					}
				}
				if ($id !== NULL) {
					$content[$id] = $record;
				}
			}
		}
	}

	// Close the file handle
	fclose($handle);

	return $content;
}

// Internal function
function _eatlas_spatial_publisher_region_parse_geojson($geojson_file) {
	$geojson_uri = $geojson_file->uri;
	$geojson_fid = $geojson_file->fid;
	$geojson_str = NULL;
	if ($cache = cache_get(EATLAS_SPATIAL_PUBLISHER_FILE_CACHE_PREFIX . $geojson_fid)) {
		$geojson_str = $cache->data;
	} else {
		$geojson_str = eatlas_commons_fix_encoding(file_get_contents($geojson_uri));
		cache_set(EATLAS_SPATIAL_PUBLISHER_FILE_CACHE_PREFIX . $geojson_fid, $geojson_str);
	}

	return drupal_json_decode($geojson_str);
}



// Internal function
function _eatlas_spatial_publisher_region_merge_geojson_csv(stdClass $node, $geojson, $csv) {
	// Get a pointer to features (by reference)
	$features = &$geojson['features'];

	// $id_column: GeoJSON attribute for the id. Saved in "field_eatlas_publisher_id_col" field
	$id_column = _eatlas_spatial_publisher_region_get_value($node, 'field_eatlas_publisher_id_col');

	// Get a pointer to each feature (by reference)
	foreach ($features as &$feature) {
		if (isset($feature['properties'][$id_column])) {
			$feature['id'] = $feature['properties'][$id_column];
		}
		if (isset($feature['id'])) {
			$id = $feature['id'];
			if (isset($csv[$id])) {
				$metadata = $csv[$id];
				if (!isset($feature['properties'])) {
					$feature['properties'] = array();
				}
				foreach ($metadata as $key => $value) {
					$feature['properties'][$key] = $value;
				}
			}
		}
	}
	// Clear the reference used in the foreach (to avoid problems)
	unset($feature);

	return $geojson;
}

// Internal function
function _eatlas_spatial_publisher_create_regions($region_set_nid, $geojson) {
	$parent_tid = NULL;
	$region_set = node_load($region_set_nid);
	if (property_exists($region_set, 'field_eatlas_publisher_tid') &&
			!empty($region_set->field_eatlas_publisher_tid) &&
			!empty($region_set->field_eatlas_publisher_tid[LANGUAGE_NONE]) &&
			!empty($region_set->field_eatlas_publisher_tid[LANGUAGE_NONE][0]) &&
			!empty($region_set->field_eatlas_publisher_tid[LANGUAGE_NONE][0]['value'])) {

		$parent_tid = $region_set->field_eatlas_publisher_tid[LANGUAGE_NONE][0]['value'];
	}

	if ($parent_tid !== NULL) {
		// 1. Create a list of "region" terms from the GeoJSON
		$new_terms = array();
		foreach ($geojson['features'] as $feature) {
			if (isset($feature['id'])) {
				$name = $feature['id'];
				/*
				if (isset($feature['properties'])) {
					$properties = $feature['properties'];
					if (isset($properties['Label'])) {
						$name = $properties['Label'];
					}
				}
				*/

				$new_terms[$feature['id']] = array(
					'feature_id' => $feature['id'],
					'name' => $name,
					'region_set_nid' => $region_set_nid,
					'parent' => $parent_tid
				);
			}
		}

		// 2. Load existing region for the current $region_set_nid
		$current_terms = array();
		foreach (eatlas_spatial_publisher_region_load_terms($region_set_nid) as $current_term) {
			$feature_id = NULL; 
			if (property_exists($current_term, 'field_eatlas_publisher_featureid') &&
					!empty($current_term->field_eatlas_publisher_featureid) &&
					!empty($current_term->field_eatlas_publisher_featureid[LANGUAGE_NONE]) &&
					!empty($current_term->field_eatlas_publisher_featureid[LANGUAGE_NONE][0]) &&
					!empty($current_term->field_eatlas_publisher_featureid[LANGUAGE_NONE][0]['value'])) {
				$feature_id = $current_term->field_eatlas_publisher_featureid[LANGUAGE_NONE][0]['value'];
			}

			if (empty($feature_id)) {
				// The term is not associated with a feature. It should be deleted.
				eatlas_spatial_publisher_delete_taxonomy_term($current_term->tid);
			} else {
				$current_terms[$feature_id] = $current_term;
			}
		}

		// 3. Update / Create / Delete
		// NOTE: Delete needs to be executed first.
		//   If a parent needs to be deleted, it's children will also be deleted.
		//   Those children may have feature ID which are use in a parent that will be created.
		//   If the Create / Update is executed first, the children which needs to be deleted
		//   will be updated instead of having new children created.

		// Delete terms that are do not exists in the new list
		foreach ($current_terms as $term_feature_id => $current_term) {
			if (!isset($new_terms[$term_feature_id])) {
				eatlas_spatial_publisher_delete_taxonomy_term($current_term->tid);
			}
		}

		// Update terms that exists in both list
		// Create terms that are do not exists in the current list
		foreach ($new_terms as $term_feature_id => $new_term) {
			$current_term = isset($current_terms[$term_feature_id]) ? $current_terms[$term_feature_id] : NULL;
			_eatlas_spatial_publisher_save_region_term($new_term, $current_term);
		}
	}
}

// API function
function eatlas_spatial_publisher_region_get_feature_term($nid, $feature_id) {
	foreach (eatlas_spatial_publisher_region_load_terms($nid) as $current_term) {
		$term_feature_id = NULL; 
		if (property_exists($current_term, 'field_eatlas_publisher_featureid') &&
				!empty($current_term->field_eatlas_publisher_featureid) &&
				!empty($current_term->field_eatlas_publisher_featureid[LANGUAGE_NONE]) &&
				!empty($current_term->field_eatlas_publisher_featureid[LANGUAGE_NONE][0]) &&
				!empty($current_term->field_eatlas_publisher_featureid[LANGUAGE_NONE][0]['value'])) {
			$term_feature_id = $current_term->field_eatlas_publisher_featureid[LANGUAGE_NONE][0]['value'];
		}

		if ($feature_id === $term_feature_id) {
			return $current_term;
		}
	}
	return null;
}

// API function
function eatlas_spatial_publisher_region_load_terms($region_set_nid) {
	if ($region_set_nid === NULL) {
		return array();
	}

	// Find the site term ID
	$region_set_node = node_load($region_set_nid);
	$site_tid = $region_set_node->field_eatlas_publisher_tid[LANGUAGE_NONE][0]['value'];

	// Extra protection (optional) - Do not even try to go further if the $site_tid
	//   is not a term of the $publisher_vocabulary.
	$publisher_vocabulary = taxonomy_vocabulary_machine_name_load(EATLAS_SPATIAL_PUBLISHER_VOCABULARY);
	$site_taxonomy_term = taxonomy_term_load($site_tid);
	if ($site_taxonomy_term->vid !== $publisher_vocabulary->vid) {
		return array();
	}

	// Load all region of the site (the list needs to be filtered)
	$all_terms = taxonomy_get_children($site_tid);

	// Only keep the terms that are linked with the given region set.
	$region_terms = array();
	foreach($all_terms as $term) {
		if ($term->field_eatlas_publisher_nid[LANGUAGE_NONE][0]['value'] === $region_set_nid) {
			$region_terms[] = $term;
		}
	}

	return $region_terms;
}

// Internal function
function _eatlas_spatial_publisher_save_region_term($values, stdClass $term = NULL) {
	if (isset($values['feature_id'])) {
		$dirty = FALSE;

		if (!$term) {
			// Create a new region taxonomy term
			$publisher_vocabulary = taxonomy_vocabulary_machine_name_load(EATLAS_SPATIAL_PUBLISHER_VOCABULARY);

			$term = (object)array(
				'vid' => $publisher_vocabulary->vid,
				'field_eatlas_publisher_featureid' => array(
					LANGUAGE_NONE => array(array(
						'value' => $values['feature_id']
					))
				)
			);

			$dirty = TRUE;
		}

		// Set taxonomy term values

		// Taxonomy term name
		if (!property_exists($term, 'name') || $term->name !== $values['name']) {
			$term->name = $values['name'];
			$dirty = TRUE;
		}

		// Parent term ID
		$dirty_parents = FALSE;
		if (!property_exists($term, 'tid')) {
			$dirty_parents = TRUE;
		} else {
			$parents = taxonomy_get_parents($term->tid);
			if (empty($parents) || count($parents) !== 1) {
				$dirty_parents = TRUE;
			} else {
				$parent_values = array_values($parents);
				$parent = $parent_values[0];
				if ($parent->tid !== $values['parent']) {
					$dirty_parents = TRUE;
				}
			}
		}
		if ($dirty_parents) {
			$term->parent = $values['parent'];
			$dirty = TRUE;
		}

		// Associated region set NID
		if (!property_exists($term, 'field_eatlas_publisher_nid') || $term->field_eatlas_publisher_nid[LANGUAGE_NONE][0]['value'] !== $values['region_set_nid']) {
			$term->field_eatlas_publisher_nid = array(
				LANGUAGE_NONE => array(array(
					'value' => $values['region_set_nid']
				))
			);
			$dirty = TRUE;
		}

		if ($dirty) {
			taxonomy_term_save($term);
		}
	}
}

?>
