<?php

include_once('eatlas_spatial_publisher.constants.inc');

/**
 * Implements: hook_block_info().
 *   https://api.drupal.org/api/drupal/modules%21block%21block.api.php/function/hook_block_info/7.x
 */
function eatlas_spatial_publisher_block_info() {
	$blocks = array();

	$sites = eatlas_spatial_publisher_site_load_terms();

	foreach ($sites as $site) {
		$region_sets = eatlas_spatial_publisher_region_set_load_roots($site->tid);

		foreach ($region_sets as $region_set) {
			$blocks[EATLAS_SPATIAL_PUBLISHER_BLOCK_PREFIX . $region_set->nid] = array(
				'info' => t('eAtlas Spacial Publisher map @site: @region_set', array(
						'@site' => $site->name,
						'@region_set' => $region_set->title)),
				'cache' => DRUPAL_NO_CACHE
			);
		}
	}

	return $blocks;
}

/**
 * Implements: hook_block_view().
 *   https://api.drupal.org/api/drupal/modules%21block%21block.api.php/function/hook_block_view/7.x
 */
function eatlas_spatial_publisher_block_view($delta = '') {
	$block = array();

	if (eatlas_commons_starts_with($delta, EATLAS_SPATIAL_PUBLISHER_BLOCK_PREFIX)) {
		$region_set_nid = substr($delta, strlen(EATLAS_SPATIAL_PUBLISHER_BLOCK_PREFIX));
		$region_set_node = node_load($region_set_nid);

		if (!empty($region_set_node)) {
			// Get the parent NID (Drupal API really lack support for doing this)
			$parent_nid = NULL;
			if (property_exists($region_set_node, 'field_eatlas_publisher_parent') &&
					!empty($region_set_node->field_eatlas_publisher_parent) &&
					!empty($region_set_node->field_eatlas_publisher_parent[LANGUAGE_NONE]) &&
					!empty($region_set_node->field_eatlas_publisher_parent[LANGUAGE_NONE][0]) &&
					!empty($region_set_node->field_eatlas_publisher_parent[LANGUAGE_NONE][0]['value'])) {
				$parent_nid = $region_set_node->field_eatlas_publisher_parent[LANGUAGE_NONE][0]['value'];
			}

			// Only display content for root node.
			// NOTE: The tree structure may changed. The association between a
			//   Block and a Page doesn't get updated when a block cease to exist.
			//   It this check is not done here, dead block will continue to get
			//   displayed after they become child of a bigger block.
			if ($parent_nid === NULL) {

				$block['content'] = array(
					'#theme' => 'node',
					'#view_mode' => 'full',
					'#node' => $region_set_node
				);
			}
		}
	}

	return $block;
}

?>
