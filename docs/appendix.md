# Appendix #

```
eAtlas Spatial Publisher
|   Type: Drupal module (this one) + Taxonomy vocabulary (eatlas_spatial_publisher)
|
\-- Spatial Publisher site
    |   Type: 1st level taxonomy term + DB table (eatlas_spatial_publisher_site)
    |   Example: "eatlas", "ts", "nwa", "ampsa"
    |
    \-- Spatial Publisher region set
        |   Type: Node (eatlas_spatial_publisher_region_set)
        |   Data: 2nd level taxonomy term
        |   Example: "world" (eAtlas), "shoals" (NWA), "network" (AMPSA)
        |   NOTE: The Node has a parent field which refer to nodes of the same content type, which are filed under the same Spatial Publisher site
        |
        \-- Spatial Publisher region
                Type: DB table (eatlas_spatial_publisher_region)
                Data: 3nd level taxonomy term

Example:

Australian Marine Parks Science Atlas (AMPSA)

  STRUCTURE
    ampsa                      (Spatial Publisher site)
      network                  (Spatial Publisher region set)
        zone                   (Spatial Publisher region set)

  DATA (from GeoJSON + CSV)
    Coral Sea                  (Spatial Publisher region - network)
      AU_DotE_Comm-2014.165    (Spatial Publisher region - zone)
      AU_DotE_Comm-2014.170    (Spatial Publisher region - zone)
      ...
    Temperate East             (Spatial Publisher region - network)
      AU_DotE_Comm-2014.125    (Spatial Publisher region - zone)
      AU_DotE_Comm-2014.126    (Spatial Publisher region - zone)
      ...

eAtlas

  STRUCTURE
    eatlas                     (Spatial Publisher site)
      country                  (Spatial Publisher region set)
        state                  (Spatial Publisher region set)
          city                 (Spatial Publisher region set)

  DATA (from GeoJSON + CSV)
    australia                  (Spatial Publisher region - country)
      qld                      (Spatial Publisher region - state)
        cairns                 (Spatial Publisher region - city)
        townsville             (Spatial Publisher region - city)
      northern-territories     (Spatial Publisher region - state)
        darwin                 (Spatial Publisher region - city)
      west-australia           (Spatial Publisher region - state)
        perth                  (Spatial Publisher region - city)
```
