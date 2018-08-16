# Content type ```eatlas_publisher_region_set``` #

The content type ```eatlas_publisher_region_set``` is used to describe the levels of the feature tree,
displayed on the navigation map.
Each navigation level have its own ```eatlas_publisher_region_set``` node, which contains a GeoJSON file, a CSV file
and many other fields.

The content type is created by the ```eatlas_spatial_publisher``` module<br/>
    file: ```eatlas_spatial_publisher.region_set.admin.inc```<br/>
    function: ```eatlas_spatial_publisher_node_type_insert```<br/>


## Content type fields ##

Overview of the fields for the ```eatlas_publisher_region_set``` content type. For information about how to use them,
see the [Configuration](configuration.md) page.

| LABEL                 | MACHINE NAME                     | FIELD TYPE            | WIDGET                    |
|-----------------------|----------------------------------|-----------------------|---------------------------|
| Title                 | title                            | Node module element   |                           |
| Preview image         | field_preview                    | File                  | Media file selector       |
| Introduction article  | body                             | Long text and summary | Text area with a summary  |
| GeoJSON file          | field_eatlas_publisher_geojson   | File                  | File                      |
| ID column name        | field_eatlas_publisher_id_col    | Text                  | Text field                |
| Parent ID column name | field_eatlas_publisher_pid_col   | Text                  | Text field                |
| Label column name     | field_eatlas_publisher_label_col | Text                  | Text field                |
| CSV metadata file     | field_eatlas_publisher_csv       | File                  | File                      |
| Template              | field_eatlas_publisher_template  | Long text             | Text area (multiple rows) |
| Map Base layer        | field_eatlas_publisher_baselayer | Long text             | Text area (multiple rows) |
| Style                 | field_eatlas_publisher_style     | Long text             | Text area (multiple rows) |
| Mouse over style      | field_eatlas_publisher_mo_style  | Long text             | Text area (multiple rows) |
| Selected style        | field_eatlas_publisher_hl_style  | Long text             | Text area (multiple rows) |
| Site                  | field_eatlas_publisher_tid       | List (text)           | Select list               |
| Parent                | field_eatlas_publisher_parent    | Integer               | Text field                |

### Field: Title ###

Field added by the node module

### Field: Preview image ###

All content type have a preview image field. It's used in search results, etc.

For the purpose of this module, the preview image is used to display a header image when no feature is selected.

**NOTE**: This content type isn't indexed; it won't appear in search results.

* **Label**: Preview image
* **Machine name**: field_preview
* **Field type**: File
* **Widget**: Media file selector
* **Help text**: This image is used as part of the article teaser on the site front page, search results and in article lists. The preview image is also used as a banner image for the article, which you can disable with the "Show banner" check box if necessary.
* **Allowed file extensions for uploaded files**: jpg, jpeg, gif, png, ico
* **Allowed URI schemes**: [X] public:// (Public files)
* **Number of values**: 1

### Field: Introduction article ###

* **Label**: Introduction article
* **Machine name**: body
* **Field type**: Long text and summary
* **Widget**: Text area with a summary
* **Help text**:
* **Rows**: 20
* **Number of values**: 1

### Field: GeoJSON file ###

* **Label**: GeoJSON file
* **Machine name**: field_eatlas_publisher_geojson
* **Field type**: File
* **Widget**: File
* **Help text**: GeoJSON file containing the polygons of the regions.
* **Allowed file extensions**: geojson
* **File directory**: spatial_publisher_uploads
* **Number of values**: 1

### Field: ID column name ###

* **Label**: ID column name
* **Machine name**: field_eatlas_publisher_id_col
* **Field type**: Text
* **Widget**: Text field
* **Help text**: Name of the column which contains the ID of the region.
* **Size of textfield**: 60
* **Text processing**: Plain text
* **Number of values**: 1
* **Maximum length**: 255

### Field: Parent ID column name ###

* **Label**: Parent ID column name
* **Machine name**: field_eatlas_publisher_pid_col
* **Field type**: Text
* **Widget**: Text field
* **Help text**: Name of the column which contains the ID of parent of the region, if any.
* **Size of textfield**: 60
* **Text processing**: Plain text
* **Number of values**: 1
* **Maximum length**: 255

### Field: Label column name ###

* **Label**: Label column name
* **Machine name**: field_eatlas_publisher_label_col
* **Field type**: Text
* **Widget**: Text field
* **Help text**: Name of the column which contains the label to be displayed.
* **Size of textfield**: 60
* **Text processing**: Plain text
* **Number of values**: 1
* **Maximum length**: 255

### Field: CSV metadata file ###

* **Label**: CSV metadata file
* **Machine name**: field_eatlas_publisher_csv
* **Field type**: File
* **Widget**: File
* **Help text**: CSV file containing extra metadata for polygons defined in the GeoJSON file.
* **Allowed file extensions**: csv
* **File directory**: spatial_publisher_uploads
* **Number of values**: 1

### Field: Template ###

* **Label**: Template
* **Machine name**: field_eatlas_publisher_template
* **Field type**: Long text
* **Widget**: Text area (multiple rows)
* **Help text**: Template used to display the metadata.
* **Rows**: 5
* **Text processing**: Plain text
* **Number of values**: 1

### Field: Map Base layer ###

* **Label**: Map Base layer
* **Machine name**: field_eatlas_publisher_baselayer
* **Field type**: Long text
* **Widget**: Text area (multiple rows)
* **Help text**: An OpenLayer layer object to use as a Base layer. If none is specified, OSM layer will be used.
* **Rows**: 5
* **Text processing**: Plain text
* **Number of values**: 1

### Field: Style ###

* **Label**: Style
* **Machine name**: field_eatlas_publisher_style
* **Field type**: Long text
* **Widget**: Text area (multiple rows)
* **Help text**: The style used to draw the polygons.
* **Rows**: 5
* **Text processing**: Plain text
* **Number of values**: 1

### Field: Mouse over style ###

* **Label**: Mouse over style
* **Machine name**: field_eatlas_publisher_mo_style
* **Field type**: Long text
* **Widget**: Text area (multiple rows)
* **Help text**: The style used when the mouse move over a polygon. If none is specified, the lines will appear 3x thicker.
* **Rows**: 5
* **Text processing**: Plain text
* **Number of values**: 1

### Field: Selected style ###

* **Label**: Selected style
* **Machine name**: field_eatlas_publisher_hl_style
* **Field type**: Long text
* **Widget**: Text area (multiple rows)
* **Help text**: The style used to highlight the selected polygon. If none is specified, the lines will appear 3x thicker.
* **Rows**: 5
* **Text processing**: Plain text
* **Number of values**: 1

### Field: Site ###

* **Label**: Site
* **Machine name**: field_eatlas_publisher_tid
* **Field type**: List (text)
* **Widget**: Select list
* **Help text**: The site associated with this region set.
* **Allowed values list**: The value of this field is being determined by the *_eatlas_spatial_publisher_get_site_options*
  function and may not be changed.
* **Number of values**: 1

### Field: Parent ###

* **Label**: Parent
* **Machine name**: field_eatlas_publisher_parent
* **Field type**: Integer
* **Widget**: Text field
* **Help text**: Parent region set node ID. Leave blank to create a new Map.
* **Number of values**: 1
