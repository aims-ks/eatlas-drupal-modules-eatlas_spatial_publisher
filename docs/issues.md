# Issues #

There is a list of issues that still need to be resolved.

* The field *Parent* of the content type *eatlas_spatial_publisher_region_set* should use a dropdown
    instead of been an open text field.
* The GeoJSON file and CSV file are not managed properly.
    * They are defined using a basic File field, which is not managed by media module.
    * There is not easy way to prevent / rollback errors when a corrupted GeoJSON / CSV file is uploaded.
* GeoJSON tree can only go 2 levels deep.
    * We need to implement recursive functions to build navigation maps with an arbitrary deep.
* Refactorisation / code cleanup
    * The file *eatlas_spatial_publisher.region_set.admin.inc* have 2 methods that compute the GeoJSON
        feature's tree. They should be combined into one.
