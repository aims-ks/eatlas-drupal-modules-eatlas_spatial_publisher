# Drupal module: eAtlas Spatial Publisher #

## Module documentation ##

This module is used to provide a geographical navigation using a clickable map.

1. [Overview](docs/overview.md)
2. [Getting started](docs/getting-started.md)
3. [Configuration](docs/configuration.md)
    1. [Content type ```eatlas_publisher_region_set```](docs/content-type-region-set.md)
    2. [GeoJSON and CSV file](docs/geojson-csv-file.md)
    3. [Template](docs/template.md)
5. Binding between map and configuration
6. Issues
    * File field not managed by media module
    * GeoJSON tree can only go 2 levels deep
    * eatlas_spatial_publisher.region_set.admin.inc have 2 methods that compute the feature tree. They should be combined.
    * region_set Parent needs to be a dropdown
7. [Appendix](docs/appendix.md)
