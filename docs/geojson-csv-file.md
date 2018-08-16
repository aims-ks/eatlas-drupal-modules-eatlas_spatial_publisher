# GeoJSON and CSV file #

In theory, the system only need a GeoJSON file. In practice, modifying a GeoJSON file is not strait-forward,
so it's better to combine the GeoJSON file with a more convenient file to maintain.

The solution is to use CSV to attach extra information (properties) to the GeoJSON features.

![GeoJSON and CSV relation](img/geojson-csv.png)



TODO

Explain how GeoJSON and CSV get combined to create a unified GeoJSON

Columns in settings (ID, Parent ID and Label)

GeoJSON ID match CSV ID (same column name)

API columns (ignore_menu)

How to use column in template

WARNING: Do not use "_" at the start of a column name. Those are reserved by the system.
