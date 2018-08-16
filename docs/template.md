# Template #

The template is defined in *eAtlas Region set* content type. It is used to construct an HTML page for each feature
of the GeoJSON file, using the information found in the GeoJSON and CSV files.

The template use the PHP language. It also define a context instance ```$c``` from the class
```EAtlas_spatial_publisher_template_context```, which contains
many methods that can be used to get relevant information on the selected feature.

## Context methods ##

* ```$c->MACRO_RelatedNodes($topic)```

    Get nodes associated with a given topic, and with the selected feature.

    * **Parameters**

        * ```$topic```: *String* (mandatory). The display name of a topic.
            [List of topics](https://eatlas.org.au/admin/structure/taxonomy/topics)

    * **Return value**

        *String*. HTML snipet of all the rendered nodes associated with the specified topic, and with the selected feature.

    * **Example**

        ```
        <div id="research-nodes">
            <?=$c->MACRO_RelatedNodes("AMPS - Recent Research") ?>
        </div>
        ```

* ```$c->MACRO_RenderNode($node, $index, $total)```

    Render a node preview with its title, preview image and teaser. This method is used by *MACRO_RelatedNodes*
    to render a list of nodes.

    * **Parameters**

        * ```$node```: *stdClass* (mandatory). The node to render. The node instance can be retrieved
            using the Drupal function [node_load()](https://api.drupal.org/api/drupal/modules!node!node.module/function/node_load/7.x).

        * ```$index```: *Integer* (optional). Index of the current node, when rendering multiple nodes.
            This is used to automatically add *first* and *last* CSS classes.

            Default: ```0```

        * ```$total```: *Integer* (optional). Total number of nodes to render, when rendering multiple nodes.
            This is used to automatically add *first* and *last* CSS classes.

            Default: ```0```

    * **Return value**

        *String*. HTML snipet of the rendered node.

    * **Example**

        ```
        <div class="about-eatlas">
            <?=$c->MACRO_RenderNode(node_load(116)) ?>
        </div>
        ```

* ```$c->MACRO_RelatedImages($type, $style)```

    Get images associated with the selected feature.

    * **Parameters**

        * ```$type```: *String* (optional). The rendering type. Accepted values:

            * 'gallery': Images display like "diapositives".
            * 'image_slider': Images shown using the "FlexSlider".

            Default: ```'gallery'```

        * ```$style```: *String* (optional). The image rendering style. Example:

            * 'thumbnail'
            * 'medium'
            * 'large'

            For complete liste, see [Image styles](https://eatlas.org.au/admin/config/media/image-styles)

            Default: ```'medium'```

    * **Return value**

        *String*. HTML snipet of all the rendered images associated with the selected feature.

    * **Example**

        ```
        <div class="preview">
            <?=$c->MACRO_RelatedImages('image_slider', 'amps_image_slider') ?>
        </div>
        ```

* ```$c->MACRO_Image($image_id, $style, $link, $includeAttribution)```

    Render an image.

    * **Parameters**

        * ```$image_id```: *Integer* (mandatory). The ID of the image to render.

        * ```$style```: *String* (optional). The image rendering style. Example:

            * 'thumbnail'
            * 'medium'
            * 'large'

            For complete liste, see [Image styles](https://eatlas.org.au/admin/config/media/image-styles)

            Default: ```'m_preview_strip'```

        * ```$link```: *String* (optional). The URL the user should be redirect to when clicking the image.

            Default: ```NULL```

        * ```$includeAttribution```: *Boolean* (optional). Add attributions on the image or not.
            This parameter is not supported yet.

            Default: ```FALSE```

    * **Return value**

        *String*. HTML snipet for the specified image.

    * **Example**

        ```
        <div class="logo">
            <?=$c->MACRO_Image(1927, 'medium', 'https://eatlas.org.au') ?>
        </div>
        ```

* ```$c->MACRO_ImageURL($image_id, $style)```

    Get the URL of an image.

    * **Parameters**

        * ```$image_id```: *Integer* (mandatory). The ID of the image.

        * ```$style```: *String* (optional). The image rendering style. Example:

            * 'thumbnail'
            * 'medium'
            * 'large'

            For complete liste, see [Image styles](https://eatlas.org.au/admin/config/media/image-styles)

            Default: ```'m_preview_strip'```

    * **Return value**

        *String*. The URL for the specified image, in the desired rendering style.

    * **Example**

        ```
        <div class="logo" style="background-image:url('<?=$c->MACRO_ImageURL(1927, 'medium') ?>')">
            LOGO
        </div>
        ```

* ```$c->MACRO_ImageTitle($image_id)```

    * **Parameters**

        * ```$image_id```: *Integer* (mandatory). The ID of the image.

    * **Return value**

        *String*. The title of the image, specified in Drupal.

    * **Example**

        ```
        <div class="title">
            <?=$c->MACRO_ImageTitle(1927) ?>
        </div>
        ```

* ```$c->MACRO_RegionsSiteMap($showOnlyCurrentChildren)```

    Generate a site map or a menu to produce a textual representation of the navigation map.

    * **Parameters**

        * ```$showOnlyCurrentChildren```: *Boolean* (optional). Use to only render sub-menu items
            for the current page. For example, this is used in the *Marine Parks Atlas* to get
            the list of marine parks for a given network.

            Default: ```FALSE```

    * **Return value**

        *String*. Rendered menu containing links for all features of the map,
            excluding the one which has the 'ignore_menu' option set to true.

    * **Example**

        ```
        <div class="site-map">
            <?=$c->MACRO_RegionsSiteMap() ?>
        </div>
        ```

* ```$c->MACRO_GetRegionId()```

    Get the ID of the selected feature. This is used to craft links.

    * **Return value**

        *String*. The ID of the selected feature.

    * **Example**

        ```
        <a href="/region_gallery/<?=$c->MACRO_GetRegionId() ?>">Image gallery</a>
        ```

* ```$c->get($property_name, $default_value)```

    Get the value of a given property, for the selected feature.

    * **Parameters**

        * ```$property_name```: *String* (mandatory). Name of the property to retrieve.
            Properties are defined by the GeoJSON and the CSV file.
            See [GeoJSON and CSV file](geojson-csv-file.md) for more info.

        * ```$default_value```: *String* (optional). The value to return when the property
            is null, as defined by *isNull*.

            Default: ```NULL```

    * **Return value**

        *String*. Return ```$default_value``` if the value is null, as defined by *isNull*.
            Otherwise, returns the value defined in the GeoJSON or CSV.
            Every value is returned as a *String*.

    * **Example**

        ```
        <div>Network: <?=$c->get("Network") ?></div>
        ```

* ```$c->isNull($property_name)```

    Test if the value for a given property is null for the selected feature.

    * **Parameters**

        * ```$property_name```: *String* (mandatory). Name of the property to test.
            Properties are defined by the GeoJSON and the CSV file.
            See [GeoJSON and CSV file](geojson-csv-file.md) for more info.

    * **Return value**

        *Boolean*. TRUE if the property is not set or its value is null, contains only whitespaces
            or is an empty string for the selected feature. FALSE otherwise.

    * **Example**

        ```
        <?php if (!$c->isNull("Network")): ?>
            <div>Network: <?=$c->get("Network") ?></div>
        <?php endif; ?>
        ```

* ```$c->formatNumber($number, $decimals)```

    Format a number using the builtin PHP function [number_format](http://php.net/manual/en/function.number-format.php).

    * **Parameters**

        * ```$number```: *String* (mandatory). A string representing a number. Also accept numeric value.

        * ```$decimals```: *Integer* (optional). Number of decimal points.

            Default: ```0```

    * **Return value**

        *String*. If the parameter ```$number``` is a number, return the formatted number.
            Otherwise, return the input parameter as-is.

    * **Example**

        ```
        <div class="total">$ <?=$c->formatNumber("4521.857", 2) ?></div>
        ```
        Outputs: ```<div class="total">$ 4,521.86</div>```

* ```$c->getCSV($variable)```

    Get a supplementary CSV file, as a 2D array.
        See [Configuration](configuration.md) for more info about the supplementary CSV files.

    * **Parameters**

        * ```$variable```: *String* (mandatory). The variable name associated with the CSV,
            as configured in the Spatial Publisher site.

    * **Return value**

        *Array of Associative arrays*. Returns a PHP structure representing the CSV file.
            Each element of the array represent a row from the CSV.
            The associative arrays are indexed using the column header defined in the
            first row of the CSV. See the example bellow for a visual representation.

    * **Example**

        ```
        <?php
        $species_csv = $c->getCSV('species');
        print '<pre>' . check_plain(print_r($species_csv, TRUE)) . '</pre>';
        ?>
        ```
        Outputs:
        ```
        Array
        (
            [0] => Array
                (
                    [species_ID] => Diomedea exulans antipodensis
                    [scientific_name] => Diomedea exulans antipodensis
                    [common_name] => Antipodean Albatross
                    [group] => Seabird_Sp
                    [layer] => cmr_cmr:AU_DOEE_bia_2016_AntipodeanAlbatross
                )

            [1] => Array
                (
                    [species_ID] => Morus serrator
                    [scientific_name] => Morus serrator
                    [common_name] => Australasian Gannet
                    [group] => Seabird_Sp
                    [layer] => cmr_cmr:AU_DOEE_bia_2016_AustralasianGannet
                )
            ...
        )
        ```

* ```$c->getCSVRows($variable, $key_column, $value)```

    Get a set of rows from a supplementary CSV file, filtered by value for a given column.

    * **Parameters**

        * ```$variable```: *String* (mandatory). The variable name associated with the CSV,
            as configured in the Spatial Publisher site.

        * ```$key_column```: *String* (mandatory). The name of the column to filter against,
            as defined in the first row of the CSV file.

        * ```$value```: *String* (mandatory). The value used to filtered the ```$key_column```.

    * **Return value**

        *Array of Associative arrays*. Returns a PHP structure representing the
            selected rows from the CSV file.
            Each element of the array represent a row from the CSV.
            The associative arrays are indexed using the column header defined in the
            first row of the CSV. See the example bellow for a visual representation.

    * **Example**

        ```
        <?php
        $dolphin_species = $c->getCSVRows('species', 'group', 'Dolphin_Sp');
        print '<pre>' . check_plain(print_r($dolphin_species, TRUE)) . '</pre>';
        ?>
        ```
        Outputs:
        ```
        Array
        (
            [0] => Array
                (
                    [species_ID] => Orcaella heinsohni
                    [scientific_name] => Orcaella heinsohni
                    [common_name] => Australian Snubfin Dolphin
                    [group] => Dolphin_Sp
                    [layer] => cmr_cmr:AU_DOEE_bia_2016_AustralianSnubfinDolphin
                )

            [1] => Array
                (
                    [species_ID] => Sousa chinensis
                    [scientific_name] => Sousa chinensis
                    [common_name] => Indo Pacific Humpback Dolphin
                    [group] => Dolphin_Sp
                    [layer] => cmr_cmr:AU_DOEE_bia_2016_IndoPacificHumpbackDolphin
                )
            ...
        )
        ```

* ```$c->getCSVRow($variable, $key_column, $value)```

    Get the first row from a supplementary CSV file, that matches the given value in a given column.

    * **Parameters**

        * ```$variable```: *String* (mandatory). The variable name associated with the CSV,
            as configured in the Spatial Publisher site.

        * ```$key_column```: *String* (mandatory). The name of the column to filter against,
            as defined in the first row of the CSV file.

        * ```$value```: *String* (mandatory). The value used to filtered the ```$key_column```.

    * **Return value**

        *Associative arrays*. Returns a PHP structure representing the
            selected row from the CSV file.
            The associative array is indexed using the column header defined in the
            first row of the CSV. See the example bellow for a visual representation.

    * **Example**

        ```
        <?php
        $orcaella_heinsohni_specie = $c->getCSVRow('species', 'species_ID', 'Orcaella heinsohni');
        print '<pre>' . check_plain(print_r($orcaella_heinsohni_specie, TRUE)) . '</pre>';
        ?>
        ```
        Outputs:
        ```
        Array
        (
            [species_ID] => Orcaella heinsohni
            [scientific_name] => Orcaella heinsohni
            [common_name] => Australian Snubfin Dolphin
            [group] => Dolphin_Sp
            [layer] => cmr_cmr:AU_DOEE_bia_2016_AustralianSnubfinDolphin
        )
        ```

* ```$c->getCSVRowsMultiValueCell($variable, $key_column, $value, $delimiter)```

    Get a set of rows from a supplementary CSV file, filtered by value in a given column,
    for a column that may contains multiple value per cell.

    * **Parameters**

        * ```$variable```: *String* (mandatory). The variable name associated with the CSV,
            as configured in the Spatial Publisher site.

        * ```$key_column```: *String* (mandatory). The name of the column to filter against,
            as defined in the first row of the CSV file.

        * ```$value```: *String* (mandatory). The value used to filtered the ```$key_column```.

        * ```$delimiter```: *String* (optional). The string used to split the value from the CSV cell,
            in order to get the list of values.

            Default: ```','```

    * **Return value**

        *Array of Associative arrays*. Returns a PHP structure representing the
            selected rows from the CSV file.
            Each element of the array represent a row from the CSV.
            The associative arrays are indexed using the column header defined in the
            first row of the CSV. See the example bellow for a visual representation.

    * **Example**

        ```
        <?php
        $sc_papers = $c->getCSVRowsMultiValueCell('references', 'rTag', 'AMP_TE_LH');
        print '<pre>' . check_plain(print_r($sc_papers, TRUE)) . '</pre>';
        ?>
        ```
        Outputs
        ```
        Array
        (
            [0] => Array
                (
                    [] => 27
                    [Number] => 1104
                    [Author] => Andreakis, N, Costello, P, Zanolla, M, Saunders, G. W and Mata, L.
                    [Title] => Endemic or introduced...
                    [Journal] => J Phycol
                    [Year] => 2016
                    [Volume] => 52
                    [Issue] => 1
                    [Pages] => 141-7
                    [DOI] => doi: 10.1111/jpy.12373
                    [Type] => Publication
                    [Network] => TE; SW
                    [Park] => Abrolhos; Eastern Recherche; Lord Howe; Norfolk; Solitary Islands
                    [nTag] => AMP_SW, AMP_TE
                    [rTag] => AMP_SW_ABR, AMP_SW_ER, AMP_TE_LH, AMP_TE_NOR, AMP_TE_SI
                )

            [1] => Array
                (
                    [] => 133
                    [Number] => 1184
                    [Author] => Boo, G. H, Nelson, W. A, Preuss, M, Kim, J. Y and Boo, S. M.
                    [Title] => Genetic segregation and...
                    [Journal] => Journal of Applied Phycology
                    [Year] => 2016
                    [Volume] => 28
                    [Issue] =>
                    [Pages] => 3
                    [DOI] => doi: 10.1007/s10811-015-0699-x
                    [Type] => Publication
                    [Network] => TE; SW
                    [Park] => Lord Howe; South-west Corner
                    [nTag] => AMP_SW, AMP_TE
                    [rTag] => AMP_SW_SWC, AMP_TE_LH
                )
            ...
        )
        ```

* ```$c->getCSVCells($variable, $key_column, $value, $column_name)```

    Get a set of cells from a supplementary CSV file, filtered by value in a given column.
    This method is very similar to *getCSVRows*, except it only return the values for a given column.

    * **Parameters**

        * ```$variable```: *String* (mandatory). The variable name associated with the CSV,
            as configured in the Spatial Publisher site.

        * ```$key_column```: *String* (mandatory). The name of the column to filter against,
            as defined in the first row of the CSV file.

        * ```$value```: *String* (mandatory). The value used to filtered the ```$key_column```.

        * ```$column_name```: *String* (mandatory). The name of the column to get the values from.

    * **Return value**

        *Array of Strings*: The values for the specified ```$column_name``` for the selected rows.

    * **Example**

        ```
        <?php
        $dolphin_names = $c->getCSVCells('species', 'group', 'Dolphin_Sp', 'common_name');
        print '<pre>' . check_plain(print_r($dolphin_names, TRUE)) . '</pre>';
        ?>
        ```
        Outputs:
        ```
        Array
        (
            [0] => Australian Snubfin Dolphin
            [1] => Indo Pacific Humpback Dolphin
            [2] => Indo Pacific Spotted Bottlenose Dolphin
        )
        ```

* ```$c->getCSVCell($variable, $key_column, $value, $column_name, $default_value)```

    Get the first cell from a supplementary CSV file, that matches the given value in a given column.

    * **Parameters**

        * ```$variable```: *String* (mandatory). The variable name associated with the CSV,
            as configured in the Spatial Publisher site.

        * ```$key_column```: *String* (mandatory). The name of the column to filter against,
            as defined in the first row of the CSV file.

        * ```$value```: *String* (mandatory). The value used to filtered the ```$key_column```.

        * ```$column_name```: *String* (mandatory). The name of the column to get the values from.

    * **Return value**

        *String*: The value for the specified ```$column_name``` for the first of selected rows.

    * **Example**

        ```
        <div class="name">
            <?=$c->getCSVCell('species', 'group', 'Dolphin_Sp', 'common_name') ?>
        </div>
        ```
        Outputs:
        ```
        <div class="name">
            Australian Snubfin Dolphin
        </div>
        ```

* ```$c->getReferenceURL($raw_url)```

    Create a valid URL from a mixed value string value.
    This method was created to handle messy data used in the Marine Park atlas project.

    * **Parameters**

        * ```$raw_url```: *String* (mandatory). A DOI id (starting with *doi:*) or a valid URL.

    * **Return value**

        *String*. A DOI URL when the given URL starts with *doi:* (case insensitive).
            Otherwise, returns the string received in parameter.

    * **Example**

        ```<a href="<?=$c->getReferenceURL("doi: 10.1111/jpy.12373") ?>">Link</a>```
        Outputs:
        ```<a href="http://dx.doi.org/10.1111/jpy.12373">Link</a>```

* ```$c->getFeature($feature_id)```

    Find the feature from the GeoJSON file that matches the given ID.

    * **Parameters**

        * ```$feature_id```: *String* (mandatory). The ID of the feature, as defined in the GeoJSON file.

    * **Return value**

        *Associative array*: Associative array representing the feature. See example bellow for more info.

    * **Example**

        ```
        <?php
        $cod_grounds_feature = $c->getFeature('AMP_TE_CG');
        print '<pre>' . check_plain(print_r($cod_grounds_feature, TRUE)) . '</pre>';
        ?>
        ```
        Outputs:
        ```
        Array
        (
            [type] => Feature
            [properties] => Array
                (
                    [Network] => Temperate East
                    [Network_ID] => AMP_TE
                    [MPA_NAME] => Cod Grounds
                    [MPA_ID] => AMP_TE_CG
                    [Area_km2] => 4.06915952691
                    [colour] => #57ADA0
                    [showcase_colour] => #00AD93
                    [showcase] => 1
                    [menu_ignore] =>
                    [Description] => Cod Grounds Marine Park is...
                    [Photo_ID] => 2744
                    [Reserve_Depth_Min_m] => 21
                    [Reserve_Depth_Max_m] => 46
                    [Reserve_Depth_Average_m] => -40.11
                    [Video_text] =>
                    [YouTube_Video_ID] =>
                    [Sealion_Sp] =>
                    [Dugon_Sp] =>
                    [Dolphin_Sp] =>
                    [Turtle_Sp] =>
                    [Rivershark_Sp] =>
                    [Seabird_Sp] => Procellaria parkinsoni, Ardenna carneipes, Ardenna tenuirostris, Ardenna pacifica
                    [Shark_Sp] => Carcharias taurus
                    [Whale_Sp] => Megaptera novaeangliae
                    [Biodiversity_text] => The Temperate East Marine Region has...
                    [Park_Area] => 4
                    [_parent_id] => AMP_TE
                )

            [geometry] => Array
                (
                    [type] => MultiPolygon
                    [coordinates] => Array
                        (
                            [0] => Array
                                (
                                    [0] => Array
                                        (
                                            [0] => Array
                                                (
                                                    [0] => 152.92083
                                                    [1] => -31.69028
                                                )

                                            [1] => Array
                                                (
                                                    [0] => 152.89972
                                                    [1] => -31.69028
                                                )

                                            [2] => Array
                                                (
                                                    [0] => 152.89972
                                                    [1] => -31.67194
                                                )

                                            [3] => Array
                                                (
                                                    [0] => 152.92083
                                                    [1] => -31.67194
                                                )

                                            [4] => Array
                                                (
                                                    [0] => 152.92083
                                                    [1] => -31.69028
                                                )

                                        )

                                )

                        )

                )

            [id] => AMP_TE_CG
            [title] => Cod Grounds
            [node] => stdClass Object
                (
                    [vid] => 27187
                    [uid] => 1
                    [title] => Marine parks
                    [nid] => 27184
                    [type] => eatlas_publisher_region_set
                    ...
                )

        )
        ```
