# Template #

The template is defined in *eAtlas Region set* content type. It is used to construct an HTML page for each feature
of the GeoJSON file, using the information found in the GeoJSON and CSV files.

The template use the PHP language. It also define a context object found in the variable ```$c```, which contains
many methods that can be used to get relevant information for the selected feature.

Context methods:

* ```$c->MACRO_RelatedNodes($topic)```

    ```$topic```: *String*. The display name of a topic.
        [List of topics](https://eatlas.org.au/admin/structure/taxonomy/topics)

    **Returns**: *String*. HTML snipet of all the rendered nodes associated with the specified topic.

    Example:
    ```
    <div id="research-nodes">
      <?=$c->MACRO_RelatedNodes("AMPS - Recent Research") ?>
    </div>
    ```

* ```$c->MACRO_RelatedImages($type, $style)```

* ```$c->MACRO_Image($image_id, $style, $link, $includeAttribution)```

* ```$c->MACRO_ImageURL($image_id, $style)```

* ```$c->MACRO_ImageTitle($image_id)```

* ```$c->MACRO_RegionsSiteMap($showOnlyCurrentChildren)```

* ```$c->MACRO_GetRegionId()```

* ```$c->MACRO_RenderNode($node, $index, $total)```

* ```$c->isNull($property_name)```

* ```$c->get($property_name)```

* ```$c->formatNumber($number)```

* ```$c->getCSV($variable)```

    Get a supplementary CSV file, as a 2D array.

* ```$c->getCSVRows($variable, $key_column, $value)```

    Get a set of rows from a supplementary CSV file, filtered by value in a given column.

* ```$c->getCSVRow($variable, $key_column, $value)```

    Get the first row from a supplementary CSV file, that matches the given value in a given column.

* ```$c->getCSVRowsMultiValueCell($variable, $key_column, $value, $delimiter)```

    Get a set of rows from a supplementary CSV file, filtered by value in a given column,
    for a column that may contains multiple value per cell.

* ```$c->getCSVCells($variable, $key_column, $value, $column_name)```

    Get a set of cells from a supplementary CSV file, filtered by value in a given column,

* ```$c->getCSVCell($variable, $key_column, $value, $column_name, $default_value)```

    Get the first cell from a supplementary CSV file, that matches the given value in a given column,

* ```$c->getReferenceURL($raw_url)```

    Create a valid URL from a mixed value string value.
    This method was created to handle messy data used in the Marine Park atlas project.

    ```$raw_url```: *String*. A DOI id (starting with *doi:*) or a valid URL.

    **Returns**: *String*. A DOI URL when the given URL starts with *doi:* (case insensitive).
        Otherwise, returns the string received in parameter. 

* ```$c->getFeature($feature_id)```

    Find the feature from the GeoJSON file that matches the given ID.

    ```$feature_id```: *String*. The ID of the feature, as defined in the GeoJSON file.
