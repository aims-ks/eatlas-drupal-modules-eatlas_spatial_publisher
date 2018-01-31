<?php

// Define which vocabulary should be used to defined available Spatial Publisher sites
define('EATLAS_SPATIAL_PUBLISHER_VOCABULARY', 'eatlas_spatial_publisher');
define('EATLAS_SPATIAL_PUBLISHER_DB_SITE_TABLE', 'eatlas_spatial_publisher_site');
define('EATLAS_SPATIAL_PUBLISHER_DB_REGION_TABLE', 'eatlas_spatial_publisher_region');
define('EATLAS_SPATIAL_PUBLISHER_DB_MENU_TABLE', 'eatlas_spatial_publisher_menu');
define('EATLAS_SPATIAL_PUBLISHER_DB_EXTRA_CSV_TABLE', 'eatlas_spatial_publisher_extra_csv');

// NOTE: Changing the name of the content type also require to change the name of the template file: page--eatlas-publisher-region-set.tpl.php
define('EATLAS_SPATIAL_PUBLISHER_CONTENT_TYPE', 'eatlas_publisher_region_set');

define('EATLAS_SPATIAL_PUBLISHER_BLOCK_PREFIX', 'eatlas_publisher_map_'); // Max 32 char

// Cache constants
define('EATLAS_SPATIAL_PUBLISHER_FILE_CACHE_PREFIX', 'eatlas_publisher_fid_');

?>
