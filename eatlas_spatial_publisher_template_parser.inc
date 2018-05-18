<?php
// get module constants, e.g. for max images in slider
include_once('eatlas_spatial_publisher.constants.inc');

/**
 * Define the class which is sent to the template.
 * The template will be able to get values using calls like:
 *   $c->get("Network")
 */
class EAtlas_spatial_publisher_template_context {
	private $site_tid;
	private $node;
	private $feature;
	private $term;
	private $extra_csv;

	function __construct($node, $feature) {
		$this->node = $node;
		$this->extra_csv = array();
		if ($this->node && property_exists($this->node, 'field_eatlas_publisher_tid') && isset($this->node->field_eatlas_publisher_tid[LANGUAGE_NONE][0]['value'])) {
			$this->site_tid = $this->node->field_eatlas_publisher_tid[LANGUAGE_NONE][0]['value'];
			$extra_csvs = eatlas_spatial_publisher_extra_csv_load_all($this->site_tid);
			foreach ($extra_csvs as $extra_csv) {
				$this->extra_csv[$extra_csv->variable] = eatlas_spatial_publisher_extra_csv_parse_csv($extra_csv->csv);
			}
		}
		$this->feature = $feature;
		if ($node && $feature) {
			$this->term = eatlas_spatial_publisher_region_get_feature_term($node->nid, $feature['id']);
		}
	}

	function MACRO_Image($image_id, $style='m_preview_strip', $link=NULL, $includeAttribution=FALSE) {
		$output = '';
		$image_file = NULL;
		if ($image_id !== NULL) {
			$image_file = file_load($image_id);
		}

		// Use "frame" module, use includeAttribution
		if ($image_file) {
			if (module_exists('eatlas_media_frame_filter')) {
				$output = eatlas_media_frame_decorate(
					array('#file' => $image_file),
					array(
						'showMetadata' => false,
						'styleName' => $style,
						'media_style' => 'none'
					),
					$link ? url($link) : NULL,
					NULL,
					FALSE
				);
			} else {
				if ($link) {
					$output = l(
						theme('image_style', array('path' => $image_file->uri, 'style_name' => $style)),
						$link,
						array('html' => TRUE)
					);
				} else {
					$output = theme('image_style', array('path' => $image_file->uri, 'style_name' => $style));
				}
			}
		}

		return $output;
	}

	function MACRO_ImageURL($image_id, $style='m_preview_strip') {
		$output = '';
		$image_file = NULL;
		if ($image_id !== NULL) {
			$image_file = file_load($image_id);
		}

		// Use "frame" module, use includeAttribution
		if ($image_file) {
			$output = image_style_url($style, $image_file->uri);
		}

		return $output;
	}

	function MACRO_RegionsSiteMap($showOnlyCurrentChildren = FALSE) {
		return eAtlas_spatial_publisher_regions_site_map($this->site_tid, $this->feature['id'], $showOnlyCurrentChildren);
	}

	function MACRO_GetRegionId() {
		return empty($this->term) ? NULL : $this->term->tid;
	}

	function MACRO_RelatedNodes($topic=NULL) {
		$output = '';

		if (!empty($this->term)) {
			// Add related nodes

			// taxonomy_select_nodes
			//   "Return nodes attached to a term across all field instances."
			//   This include "field_regions" field.
			//   https://api.drupal.org/api/drupal/modules!taxonomy!taxonomy.module/function/taxonomy_select_nodes/7.x
			$related_node_ids = taxonomy_select_nodes($this->term->tid, FALSE);
			if (!empty($related_node_ids)) {
				$related_nodes = node_load_multiple($related_node_ids);
				if (!empty($related_nodes)) {
					// Order nodes
					//   http://php.net/manual/en/function.usort.php
					usort($related_nodes, function($a, $b) {
						// Natural order comparison
						//   http://php.net/manual/en/function.strnatcmp.php
						return strnatcmp($a->title, $b->title);
					});

					$selected_nodes = array();
					foreach($related_nodes as $related_node) {
						// Get "Prepress" field (aka 'field_draft')
						$prepress = FALSE;
						if (property_exists($related_node, 'field_draft') &&
								!empty($related_node->field_draft) &&
								!empty($related_node->field_draft[LANGUAGE_NONE]) &&
								!empty($related_node->field_draft[LANGUAGE_NONE][0]) &&
								!empty($related_node->field_draft[LANGUAGE_NONE][0]['value'])) {
							$prepress = !!$related_node->field_draft[LANGUAGE_NONE][0]['value'];
						}

						if (!$prepress) {
							// Find the "filter" topic ID
							$topic_tid = NULL;
							if (!empty($topic)) {
								$topic_terms = taxonomy_get_term_by_name($topic, 'topics');
								if (!empty($topic_terms)) {
									$topic_term = reset($topic_terms); // Get first element from the array
									if (!empty($topic_term)) {
										$topic_tid = $topic_term->tid;
									}
								}
								if (!$topic_tid) {
									return NULL;
								}
							}

							// Check if the node is associated with the topic ID
							$node_contains_topic = FALSE;
							if (property_exists($related_node, 'field_topics') &&
									!empty($related_node->field_topics) &&
									!empty($related_node->field_topics[LANGUAGE_NONE])) {
								foreach ($related_node->field_topics[LANGUAGE_NONE] as $related_node_field_topics_value) {
									if (!empty($related_node_field_topics_value['tid']) &&
											$related_node_field_topics_value['tid'] === $topic_tid) {
										$node_contains_topic = TRUE;
									}
								}
							}

							// Filter out the nodes that are not associated with the topic
							if ($node_contains_topic) {
								$selected_nodes[] = $related_node;
							}
						}
					}

					$nb_nodes = count($selected_nodes);
					if ($nb_nodes > 0) {
						for ($i = 0; $i < $nb_nodes; $i++) {
							$output .= $this->MACRO_RenderNode($selected_nodes[$i], $i+1, $nb_nodes);
						}
					}
				}
			}
		}

		return $output;
	}

	/**
	 * Render a node with its title, preview image and teaser.
	 * This is an internal function
	 * but it could be useful in the template so I created a
	 * "MACRO" function with it.
	 */
	function MACRO_RenderNode($node, $index = 0, $total = 0) {
		$output = '';

		if ($node) {
			$node_title = $node->title;
			$node_url = NULL;
			$node_target = NULL;

			// URL & target
			if ($node->type === 'external_content') {
				$node_target = '_blank';
				if (property_exists($node, 'field_ea_rss_link') &&
						!empty($node->field_ea_rss_link) &&
						!empty($node->field_ea_rss_link[LANGUAGE_NONE]) &&
						!empty($node->field_ea_rss_link[LANGUAGE_NONE][0]) &&
						!empty($node->field_ea_rss_link[LANGUAGE_NONE][0]['value'])) {
					$node_url = $node->field_ea_rss_link[LANGUAGE_NONE][0]['value'];
				}
			} else {
				$node_url = url('node/'. $node->nid, array('query' => array('rsid' => $this->node->nid, 'featureId' => $this->feature['id'])));
			}

			// Preview image
			$image_id = NULL;
			if (property_exists($node, 'field_preview') &&
					!empty($node->field_preview) &&
					!empty($node->field_preview[LANGUAGE_NONE]) &&
					!empty($node->field_preview[LANGUAGE_NONE][0]) &&
					!empty($node->field_preview[LANGUAGE_NONE][0]['fid'])) {
				$image_id = $node->field_preview[LANGUAGE_NONE][0]['fid'];
			}
			$node_preview = $this->MACRO_Image($image_id, 'thumbnail');

			// Body
			$node_field_view = field_view_field('node', $node, 'body', array(
				'label' => 'hidden',
				'type' => 'text_summary_or_trimmed',
				'settings' => array('trim_length' => 200)
			));
			$node_summary = render($node_field_view);

			// Template
			if ($node_url) {
				$css_class = 'node-block';
				if ($index && $total) {
					if ($index === 1) {
						$css_class .= ' first';
					}
					if ($index === $total) {
						$css_class .= ' last';
					}
				}
				$output .=
					'<div class="' . $css_class . '" onclick="window.location=\'' . $node_url . '\';">' .
						($node_preview ? '<div class="node-preview-image"><a href="' . $node_url . '">' . $node_preview . '</a></div>' : '') .
						'<h3 class="node-title"><a href="' . $node_url . '"' . ($node_target ? ' target="' . $node_target . '"' : '') . '>' .
							$node_title .
						'</a></h3>' .
						'<div class="node-body">' . $node_summary . '</div>' .
					'</div>';
			}
		}

		return $output;
	}

	/**
	 * Add related images
	 * $type:
	 *   'gallerie': Images display like "diapositives".
	 *   'image_slider': Images shown using the "FlexSlider".
	 */
	function MACRO_RelatedImages($type='gallerie', $style='medium') {
		$output = '';

		if (!empty($this->term)) {
			// Select files
			// NOTE: There is no API function in taxonomy module to deal with "file" (there is no such thing as "taxonomy_select_files")
			//   There is no taxonomy index for "file" (can't use "taxonomy_index" table)
			//   EntityFieldQuery can't handle "file" fields because they are part of an other structure ("file_managed")
			//   The only solution is to use "db_select" which is very low level...
			// NOTE: The DB table "field_data_field_regions" contains all the information needed,
			//   but the query is based on "file_managed" table to avoid getting results from deleted entities
			//   (the Drupal DB seems to stay clean but there is no integrity constraints so we can't assume it is clean)
			$query = db_select('file_managed', 'fm');
			$query->join('field_data_field_regions', 'fr', 'fr.entity_type = \'file\' AND fr.bundle = \'image\' AND fm.fid = fr.entity_id');
			$query->groupBy('fm.fid');
			$query->fields('fm', array('fid'));
			$query->condition('fr.field_regions_tid', $this->term->tid, '=');
			if ($type === 'image_slider') {
				$query->join('field_data_field_region_highlight', 'frh', 'frh.entity_type = \'file\' AND frh.bundle = \'image\' AND fm.fid = frh.entity_id');
				$query->condition('frh.field_region_highlight_value', 1, '=');
			}
			$results = $query->execute()->fetchAll();

			if (!empty($results)) {
				// Collect all file ids in an array
				$fids = array();
				foreach($results as $result) {
					$fids[] = $result->fid;
				}
				// Load files
				$files = file_load_multiple($fids);

				if (!empty($files)) {
					if ($type === 'gallerie') {
						$output .= '<div class="related-images">';
						foreach ($files as $file) {
							$link = 'media/' . $file->fid;

							if (module_exists('eatlas_media_frame_filter')) {
								$photo = eatlas_media_frame_decorate(
									array('#file' => $file),
									array(
										'showMetadata' => false,
										'styleName' => $style,
										'media_style' => 'none'
									),
									url($link),
									NULL,
									FALSE
								);
							} else {
								$photo = l(
									theme('image_style', array('path' => $file->uri, 'style_name' => $style)),
									$link,
									array('html' => TRUE)
								);
							}

							$output .= '<div class="mediaframe_gallery wysiwyg-media-file noMetadata" style="width: 222px; height: 222px; margin: 3px">' . $photo . '</div>';
						}
						$output .= '</div>';

					} else if ($type === 'image_slider') {
						if (count($files) == 1) {
							$file = reset($files);
							$link = 'media/' . $file->fid;

							$output .=
								'<div class="flexslider-wrapper">' .
									'<a href="' . $link . '" class="flexslider-bgimage" style="background-image:url(\'' . $this->MACRO_ImageURL($file->fid, $style) . '\')"></a>' .
                					'<div class="media-attribution">' . theme('eatlas_media_gallery_fixes_attribution', array('prefix' => '', 'file' => $file)) . '</div>' .
								'</div>';
						} else {
							// Activate the image slider (when the page will be ready)
							$output .=
								'<script>' .
								'(function ($) {' .
									'$(document).ready(function() {' .
										'$(".flexslider").flexslider({' .
											'animation: "slide"' .
										'});' .
									'});' .
								'})(jQuery);' .
								'</script>';

							// Add markup
							$output .=
								'<div class="flexslider-wrapper">' .
									'<div class="flexslider">' .
										'<ul class="slides">';

							$counter = 0;
							foreach ($files as $file) {
								$link = 'media/' . $file->fid;
								$output .=
									'<li>' .
										'<a href="' . $link . '" class="flexslider-bgimage" style="background-image:url(\'' . $this->MACRO_ImageURL($file->fid, $style) . '\')"></a>' .
                    '<div class="media-attribution">' . theme('eatlas_media_gallery_fixes_attribution', array('', 'file' => $file)) . '</div>' .
									'</li>';

								$counter++;
                if ($counter >= EATLAS_SPATIAL_PUBLISHER_SLIDER_MAX_IMAGES) {
                  break;
                }
							}

							$output .=
										'</ul>' .
									'</div>' .
								'</div>';
						}
					}
				}
			}
		}

		return $output;
	}

	// NOTE Do not use isEmpty because "0" will return true!
	function isNull($property_name) {
		$properties = $this->feature['properties'];
		return isset($properties[$property_name]) ? $properties[$property_name] === NULL || trim($properties[$property_name]) === '' : TRUE;
	}

	function get($property_name, $default_value=NULL) {
		$properties = $this->feature['properties'];
		return $this->isNull($property_name) ? $default_value : $properties[$property_name];
	}

	function getCSV($variable) {
		if (!$variable) {
			return NULL;
		}
		return $this->extra_csv[$variable];
	}

	function getCSVRows($variable, $key_column, $value) {
		$rows = array();
		$csv_content = $this->getCSV($variable);
		if ($csv_content && $key_column) {
			foreach ($csv_content as $row) {
				if (isset($row[$key_column]) && trim($row[$key_column]) === trim($value)) {
					$rows[] = $row;
				}
			}
		}

		return $rows;
	}

	// Select rows which contains the specified value.
	// The cell may contains multiple value (separated by the delimiter value)
	function getCSVRowsMultiValueCell($variable, $key_column, $value, $delimiter = ',') {
		$rows = array();
		$csv_content = $this->getCSV($variable);
		if ($csv_content && $key_column) {
			foreach ($csv_content as $row) {
				if (isset($row[$key_column])) {
					$multiValue = trim($row[$key_column]);
					$multiValueArray = explode($delimiter, $multiValue);
					if ($multiValueArray && count($multiValueArray) > 0) {
						foreach ($multiValueArray as $singleValue) {
							if (trim($singleValue) === trim($value)) {
								$rows[] = $row;
							}
						}
					}
				}
			}
		}

		return $rows;
	}

	function getCSVCells($variable, $key_column, $value, $column_name) {
		$csv_rows = $this->getCSVRows($variable, $key_column, $value);
		$cell_values = array();
		if ($csv_rows && count($csv_rows) > 0) {
			foreach ($csv_rows as $csv_row) {
				if ($csv_row && isset($csv_row[$column_name])) {
					// The row may contain an empty cell for the required column
					$cell_value = trim($csv_row[$column_name]);
					if ($cell_value) {
						$cell_values[] = $cell_value;
					}
				}
			}
		}
		return $cell_values;
	}

	function getCSVRow($variable, $key_column, $value) {
		$csv_content = $this->getCSV($variable);
		if ($csv_content && $key_column) {
			foreach ($csv_content as $row) {
				if (isset($row[$key_column]) && trim($row[$key_column]) === trim($value)) {
					return $row;
				}
			}
		}

		return NULL;
	}

	function getCSVCell($variable, $key_column, $value, $column_name, $default_value = NULL) {
		$csv_row = $this->getCSVRow($variable, $key_column, $value);
		$cell_value = NULL;
		if ($csv_row && isset($csv_row[$column_name])) {
			$cell_value = trim($csv_row[$column_name]);
		}
		return $cell_value ? $cell_value : $default_value;
	}

	// If the string starts with "DOI:", returns the DOI URL.
	// Otherwise, return the input string.
	function getReferenceURL($raw_url) {
		if ($raw_url && strtolower(substr($raw_url, 0, 4)) === 'doi:') {
			$doi = trim(substr($raw_url, 4));
			if ($doi) {
				return 'http://dx.doi.org/' . $doi;
			}
		}

		return $raw_url;
	}

}

function eAtlas_spatial_publisher_regions_site_map($site_tid, $active_feature_id = NULL, $showOnlyCurrentChildren = FALSE) {
	$tree_data = eAtlas_spatial_publisher_regions_site_menu_tree_page_data('eAtlas_spatial_publisher_site_map_' . $site_tid, $site_tid, $active_feature_id);
	if ($showOnlyCurrentChildren) {
		$branch_id = 'eatlas_publisher_menu_item_' . $active_feature_id;
		// Get current branch
		$current_branch_data = eAtlas_spatial_publisher_search_sub_menu($tree_data, $branch_id);

		// Get current branch children
		if ($current_branch_data && isset($current_branch_data[$branch_id]) && isset($current_branch_data[$branch_id]['below'])) {
			$tree_data = $current_branch_data[$branch_id]['below'];
		} else {
			$tree_data = array();
		}
	}
	$tree_output = menu_tree_output($tree_data);
	return drupal_render($tree_output);
}

function eAtlas_spatial_publisher_search_sub_menu($menu, $mlid) {
	foreach ($menu as $submenu_mlib => $submenu) {
		if ($submenu_mlib === $mlid) {
			return array(
				$submenu_mlib => $submenu
			);
		}
		if (isset($submenu['below']) && count($submenu['below']) > 0) {
			$found_branch = eAtlas_spatial_publisher_search_sub_menu($submenu['below'], $mlid);
			if ($found_branch !== NULL) {
				return $found_branch;
			}
		}
	}
	return NULL;
}

/**
 * Inspired on menu_tree_data
 *   https://api.drupal.org/api/drupal/includes%21menu.inc/function/menu_tree_data/7.x
 */
function eAtlas_spatial_publisher_regions_site_menu_tree_page_data($menu_name, $site_tid, $active_feature_id=NULL, $parent_id=NULL) {
	$feature_tree = eatlas_spatial_publisher_regions_set_get_feature_tree($site_tid);
	return _eAtlas_spatial_publisher_regions_site_submenu_tree_page_data($menu_name, $feature_tree, $active_feature_id, $parent_id);
}

function _eAtlas_spatial_publisher_regions_site_submenu_tree_page_data($menu_name, $feature_branch, $active_feature_id=NULL, $parent_id=NULL) {
	$menu_links = array();

	if ($feature_branch) {
		foreach($feature_branch as $feature) {
			$mlid = 'eatlas_publisher_menu_item_' . $feature['id'];

			$child_menu = isset($feature['children']) ?
					_eAtlas_spatial_publisher_regions_site_submenu_tree_page_data($menu_name, $feature['children'], $active_feature_id, $mlid) :
					array();

			// The menu item can be ignored if specified in the properties
			$ignore = isset($feature['properties']['menu_ignore']) && $feature['properties']['menu_ignore'];

			if (!$ignore) {
				$menu_links[$mlid] = array(
					'link' => array(
						'menu_name' => $menu_name,
						'access' => TRUE,
						'hidden' => FALSE,
						'in_active_trail' => FALSE, // TODO
						'mlid' => $mlid,
						'plid' => $parent_id ? $parent_id : 0,
						'href' => 'amps',
						'localized_options' => array(
							'query' => array(
								'featureId' => $feature['id']
							)
						),
						'title' => $feature['title'],
						'has_children' => !empty($child_menu),
						'expanded' => !empty($child_menu)
					),
					'below' => $child_menu
				);
			}
		}
	}

	return $menu_links;
}




function eAtlas_spatial_publisher_eval_template(stdClass $node, $feature) {
	if (property_exists($node, 'field_eatlas_publisher_template') &&
			isset($node->field_eatlas_publisher_template[LANGUAGE_NONE]) &&
			isset($node->field_eatlas_publisher_template[LANGUAGE_NONE][0]) &&
			isset($node->field_eatlas_publisher_template[LANGUAGE_NONE][0]['value'])) {

		// The variables $node, $feature and $c will be available to the template
		// "The code will be executed in the scope of the code calling eval()."
		//   http://php.net/manual/en/function.eval.php
		$c = new EAtlas_spatial_publisher_template_context($node, $feature);

		// IMPORTANT:
		// The string MUST start with closing PHP tag
		//   and MUST end with opening PHP tag,
		//   because the eval function expect PHP code,
		//   not a HTML template containing PHP fragments.
		// Spaces before closing PHP tag and after opening PHP tag are important!
		//   Some version of PHP will die if those spaces are not present.
		eval(' ?>' . $node->field_eatlas_publisher_template[LANGUAGE_NONE][0]['value'] . '<?php ');
	}
}

?>
