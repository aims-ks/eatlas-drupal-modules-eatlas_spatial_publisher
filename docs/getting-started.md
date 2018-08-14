## Installation ##

1. Create the content type ```eatlas_publisher_region_set```.

    **NOTE**: The content type should be created by the module, but that part haven't been implemented yet.

2. Extract the module files in ```drupal/sites/all/modules/eatlas/eatlas_spatial_publisher```

3. Enable the module with ```https://eatlas.org.au/admin/modules```

### Entities created ###

The module automatically create the following entities if they do not already exists.

Vocabulary:
* ```eatlas_spatial_publisher```

Database tables:
* ```eatlas_spatial_publisher_site```
* ```eatlas_spatial_publisher_region```
* ```eatlas_spatial_publisher_menu```
* ```eatlas_spatial_publisher_extra_csv```

Content type:
* ```eatlas_publisher_region_set```

**NOTE**: The content type is not created by the module yet. It needs to be created manually prior to the
    installation of the module.

Cache:
* ```eatlas_publisher_fid_[FID]```
* ```eatlas_publisher_nid_[NID]```

**NOTE**: The cache is managed using Drupal cache API (```cache_set```, ```cache_get``` and ```cache_clear_all```).
    It's used to avoid re-loading GeoJSON / CSV files and to avoid re-generating the merged GeoJSON file.


## Configuration ##

Configure the module with ```https://eatlas.org.au/admin/config/eatlas/eatlas_spatial_publisher```

You will need to 


## Uninstall ##

1. Disable the module ```https://eatlas.org.au/admin/modules```

2. Uninstall the module ```https://eatlas.org.au/admin/modules/uninstall```

### Entities deleted ###

Database tables (deleted by Drupal, as part of the DB schema API):
* ```eatlas_spatial_publisher_site```
* ```eatlas_spatial_publisher_region```
* ```eatlas_spatial_publisher_menu```
* ```eatlas_spatial_publisher_extra_csv```

Content type (and all nodes created from that type):
* ```eatlas_publisher_region_set```

Cache:
* ```eatlas_publisher_fid_*```
* ```eatlas_publisher_nid_*```

### Entities NOT deleted ###

Vocabulary:
* ```eatlas_spatial_publisher```

Files added using the media module
