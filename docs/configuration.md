# Configuration #

Configure the module with ```https://eatlas.org.au/admin/config/eatlas/eatlas_spatial_publisher```

Click the *Add a Spatial Publisher site* link to create a new navigation map.

* **Name**: Name of the navigation map. That name is used internally for references
and for the root element of the navigation menu.
It will also be use for the name of the vocabulary root term that will be used for referencing nodes.
The name can be change later.

* **Region menus**: The module can create navigation menu items. Simply select to which menu the item needs to be added.
The menu item will have the weight index 0. You can control where it appear in the menu by changing the index
of the other elements of the menu.

* **Max zoom level**: Restrict the navigation map to a maximum zoom level.

* **North, East, South, West**: Define the initial bounding box of the navigation map.
OpenLayers will decide the most appropriate "center" and "zoom level" for the specified bounding box.
If no bounding box is specified (or one value is missing), the module will use the combine bounding box
of all the features of the provided GeoJSON file (see next step).

Once the navigation map is created, click the edit button next to it.


## Region sets ##

Click on *Add a region set* to associate a GeoJSON with the navigation map.
This will open the *Create eAtlas Region set* page, which will create an instance of the content type *Region set*.<br/>
See [Content type ```eatlas_publisher_region_set```](content-type-region-set.md) for more info.

* **Title**: Node title, display on the map side panel when no feature is selected.

* **Preview image**: Image displayed in the header of the map side panel when no feature is selected.

* **Introduction article**: Page content displayed in the map side panel when no feature is selected.

* **GeoJSON file**: File containing the polygon to display on the map.<br/>
    See [GeoJSON and CSV file](geojson-csv-file.md).

* **ID column name**: Name of the GeoJSON property or CSV column to use as an ID for the features.
    The ID is used in the URL, for the creation of taxonomy terms to refer nodes, etc.
    It is in your best interest to set this field correctly on creation of the node.
    Modifying this field will generate a lot of issues:
    * New taxonomy terms will be created, to reflect the value of the new column specified
    * Nodes associated with the old taxonomy terms will not get migrated to the new taxonomy term.
        The migration needs to be done manually.
    * Old taxonomy terms will remain in the system, with the label suffix *TO DELETE*.

* **Parent ID column name**: Name of the GeoJSON property or CSV column containing the ID of the parent feature.
    This is used for navigation map which has multiple level of navigation.
    The first level do not have parent. The column needs to stay empty.
    The second navigation level is created using another region set node.
    The value found on the column set here needs to match the values found in the
    *ID column name* of the parent node (described at the end of this list).

* **Label column name**: Name of the GeoJSON property or CSV column containing the value to use as a label
    when referring to the feature. This is used for the generation of the navigation menu.

* **CSV metadata file**: CSV file, used to add supplementary information to the features.
    Modifying the GeoJSON can be complicated. You need appropriate tools and it generally
    takes longer than modifying a CSV file.
    The purpose of this CSV file is to make it easier to maintain certain properties of the features.<br/>
    See [GeoJSON and CSV file](geojson-csv-file.md).

* **Template**: PHP template used to show information about the selected feature.<br/>
    See [Template](template.md)

* **Map Base layer**: JavaScript code used to instanciate an OpenLayers *ol.layer*.<br/>
    Example:
    ```
    new ol.layer.Tile({
      source: new ol.source.TileWMS({
        attributions: 'Basemap: <a href="..." target="_blank">Bright earth</a>',
        url: 'https://maps.eatlas.org.au/maps/gwc/service/wms',
        crossOrigin: 'anonymous',
        params: {
          'LAYERS': 'ea-be:World_Bright-Earth_MPA-basemap',
          'FORMAT': 'image/jpeg',
          'TRANSPARENT': false,
          'VERSION': '1.1.1',
          'TILED': true
        },
        hidpi: false,
        serverType: 'geoserver'
      })
    })
    ```

* **Style**: JavaScript code used to create a single or an array of OpenLayers *ol.style*.
    The style is used to display the GeoJSON feature when it's not selected.<br/>
    Properties from the GeoJSON and CSV file can be used here:

    * ```feature.get(PROPERTY_NAME)```<br/>
        PROPERTY_NAME: String. Name of the GeoJSON property or CSV column.<br/>
        Returns: String. The value for the specified property or CSV column, for the selected feature.

    The function *getAlphaColour* can also be used to change the opacity of an hexadecimal colour:

    * ```getAlphaColour(HEXADECIMAL_COLOR, OPACITY)```<br/>
        HEXADECIMAL_COLOR: String. Hexadecimal colour. Example: *#FF3399*<br/>
        OPACITY: Decimal. 1 for full opacity, 0 for transparent.<br/>
        Returns: Array of decimal. Representation of the colour as an array of decimal value, between 0 and 1;
            [Red, Green, Blue, Alpha]

    Example:
    ```
    new ol.style.Style({
      'stroke': new ol.style.Stroke({
        'color': feature.get('colour'),
        'width': 1,
      }),
      'fill': new ol.style.Fill({
        'color': getAlphaColour(feature.get('colour'), 0.5)
      })
    })
    ```

* **Mouse over style**: JavaScript code used to create a single or an array of OpenLayers *ol.style*.
    The style is used to display the GeoJSON feature when the mouse move over it.<br/>
    See *Style* above for more information.

* **Selected style**: JavaScript code used to create a single or an array of OpenLayers *ol.style*.
    The style is used to display the GeoJSON feature that is selected.<br/>
    See *Style* above for more information.

* **Site**: The navigation map associate with this *Region set* node.
    The appropriate value should already be selected.
    You should not have to modify this value.

* **Parent**: The ID of the parent node. This field is required for the second level of the navigation map, and up.<br/>
    *NOTE*: This field should display a dropdown of node, but that functionality has not been implemented yet.


## Supplementary CSV files ##

TODO
