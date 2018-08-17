# Appendix #

## Template example ##

```
<?php
$coolScienceNodes = $c->MACRO_RelatedNodes("AMPS - Recent Research");
$otherNodes = $c->MACRO_RelatedNodes(NULL);
$imageSlider = $c->MACRO_RelatedImages('image_slider', 'amps_image_slider');
$sc_papers_rows = $c->getCSVRowsMultiValueCell('references', 'rTag', $c->get("MPA_ID"));
?>

<div class="panel-header">
  <div class="breadcrumbs">
    <a href="/amps">Australia</a> &gt;
    <?php if($c->getFeature($c->get("Network_ID"))): ?>
      <a href="/amps?featureId=<?=$c->get("Network_ID") ?>"><?=$c->get("Network") ?></a> &gt;
    <?php endif; ?>
    <?=$c->get("MPA_NAME") ?>
  </div>

  <h2 class="title"><?=$c->get("MPA_NAME") ?> <?=$c->get("park_type") ?></h2>

  <div style="background-color: <?=$c->get("showcase_colour") ?>; height: 2em; margin: 2em 0;"></div>

  <div class="template-menu">
    <nav>
      <ul>
        <?php if ($coolScienceNodes): ?>
          <li><a href="#research">Research</a></li>
        <?php endif; ?>
        <?php if ($otherNodes): ?>
          <li><a href="#other">Other content</a></li>
        <?php endif; ?>
        <?php if ($imageSlider): ?>
          <li><a href="/region_gallery/<?=$c->MACRO_GetRegionId() ?>">Image gallery</a></li>
        <?php endif; ?>
        <?php if ($sc_papers_rows): ?>
          <li><a href="#sc-papers">Scientific papers</a></li>
        <?php endif; ?>
      </ul>
    </nav>
  </div>
</div>

<?php if ($imageSlider): ?>
  <div class="preview">
    <?=$imageSlider ?>
    <div style="text-align: right;"><a href="/region_gallery/<?=$c->MACRO_GetRegionId() ?>">Image gallery</a></div>
  </div>
<?php endif; ?>

<div class="pagewidth">
  <div class="stats">
    <div>
      <?php if ($c->get('menu_ignore') !== "TRUE" && !$c->isNull("Network")): ?>
        <div>
          <span class="title">Network:</span>
          <span class="value"><?=$c->get("Network") ?></span>
        </div>
      <?php endif; ?>
      <?php if (!$c->isNull("Park_Area")): ?>
        <div>
          <span class="title">Marine park area:</span>
          <span class="value"><?=$c->formatNumber($c->get("Park_Area")) ?></span>
          <span class="unit">kilometres<sup>2</sup></span>
        </div>
      <?php endif; ?>
    </div>
  </div>

  <?php if (!$c->isNull("Description")): ?>
    <p><?=$c->get("Description") ?></p>
  <?php endif; ?>

  <?php if ($coolScienceNodes): ?>
    <div class="group">
      <h2 id="research">Research</h2>
      <div class="description-block">These short articles highlight the findings from ongoing scientific research within the <?=$c->get("MPA_NAME") ?> Marine Park, including exciting new discoveries and ideas for future projects.</div>
      <div id="research-nodes">
        <?=$coolScienceNodes ?>
      </div>
      <div class="button-row">
        <button class="collapsible-minheight-button" target="research-nodes" expandLabel="More" collapseLabel="Less">More</button>
      </div>
    </div>
  <?php endif; ?>

  <?php if ($otherNodes): ?>
    <div class="group">
      <h2 id="other">Other content</h2>
      <div id="other-nodes">
        <?=$otherNodes ?>
      </div>
      <div class="button-row">
        <button class="collapsible-minheight-button" target="other-nodes" expandLabel="More" collapseLabel="Less">More</button>
      </div>
    </div>
  <?php endif; ?>

  <!--
  Harvard referencing
      https://en.wikipedia.org/wiki/Parenthetical_referencing
  Format:
      Smith, John (2008). Name of Book. Name of Publisher.
  -->
  <?php
  function marineparks_check_plain($str) {
      if (!$str) {
          return $str;
      }
      $safe_str = check_plain($str);
      if (!$safe_str) {
          return '<b>INVALID ENCODING</b>';
      }
      return $safe_str;
  }
  ?>
  <?php if ($sc_papers_rows): ?>
    <div class="group">
      <h2 id="sc-papers">Scientific papers</h2>
      <div class="description-block">The following publications contain information relevant to the <?=$c->get("Network") ?> Network. Click on the links to access to the publications.</div>
      <ul id="sc-paper-list">
        <?php foreach($sc_papers_rows as $sc_papers_row) {
          $url = $c->getReferenceURL($sc_papers_row['DOI']);

          $author = $sc_papers_row['Author'];
          if (!$author) $author = 'Unknown author';

          $year = $sc_papers_row['Year'];
          if (!$year) $year = 'N/A';

          $title = $sc_papers_row['Title'];
          if (!$title) $title = 'Untitled';

          $publisher = NULL; //$sc_papers_row['Publisher..IP.owner'];
          ?>
          <li>
            <?=marineparks_check_plain($author) ?> (<?=marineparks_check_plain($year) ?>).

            <?php if ($url) : ?>
              <a href="<?=$url ?>" target="_blank">
            <?php endif; ?>

            <i><?=marineparks_check_plain($title) ?></i>.

            <?php if ($url) : ?>
              </a>
            <?php endif; ?>

            <?php if ($publisher) : ?>
              <?=marineparks_check_plain($publisher) ?>.
            <?php endif; ?>
          </li>
        <?php } ?>
      </ul>
      <div class="button-row">
        <button class="collapsible-minheight-button" target="sc-paper-list" expandLabel="More" collapseLabel="Less">More</button>
      </div>
    </div>
  <?php endif; ?>
</div>
```

## Simple style example ##

Simple style, that draw feature's polygon using colours defined in the *GeoJSON / CSV file*.

See [OpenLayers Style doc](https://openlayers.org/en/latest/apidoc/module-ol_style_Style.html)
```
new ol.style.Style({
  'stroke': new ol.style.Stroke({
    'color': feature.get('showcase_colour'),
    'width': 1
  }),
  'fill': new ol.style.Fill({
    'color': getAlphaColour(feature.get('showcase_colour'), 0.5)
  })
})
```

## Complex style example ##

Complex style:

* First part: Define how to draw the label.
    * If resolution is > 40000 (zoomed out), do not draw any label.
    * If the feature's 'menu_ignore' property is set to 'TRUE', draw a small label
        in the center of the feature's polygon.
        The label is offset for the marine park 'Moreton Bay' only.
    * Otherwise, draw a large label in the center of the feature's polygon.
* Second part: Define how to draw the polygon.
    * Draw the feature's polygon using colours defined in the *GeoJSON / CSV file*.

See [OpenLayers Style doc](https://openlayers.org/en/latest/apidoc/module-ol_style_Style.html)
```
[
  resolution > 40000 ? new ol.style.Style() : ( feature.get('menu_ignore') === "TRUE" ?
      // Style for non Parks Australia parks. 
      // De-emphasise them
      new ol.style.Style({
          'text': new ol.style.Text({
          'text': feature.get('MPA_NAME'),
          'textAlign': 'center',
          'font': resolution > 10000 ? '8px Arial' : '10px Arial',
          'stroke': new ol.style.Stroke({'color': getAlphaColour('#ffffff', 0.8), width: 2})
        }),
        // Find the center of the multi-polygon
        'geometry': function(feature) {
          var extent = feature.getGeometry().getExtent();
          if (extent) {
            var mpa = feature.get('MPA_NAME');
            var offsetX = 0, offsetY = 0;
            // Adjust labels to not clash with basemap
            if (mpa == 'Moreton Bay') {
                // move away from Brisbane label. When zoomed in have no offset.
                offsetX = resolution * 5;
                offsetY = resolution * 15; 
            }
            centre = ol.extent.getCenter(extent);
            return new ol.geom.Point([centre[0]+offsetX,centre[1]+offsetY]);
          }
        }
      }) :
      new ol.style.Style({
        'text': new ol.style.Text({
          'text': feature.get('MPA_NAME'),
          'textAlign': 'center',
          'font': resolution > 20000 ? 'bold 13px Arial' : 'bold 16px Arial',
          'stroke': new ol.style.Stroke({'color': getAlphaColour('#ffffff', 0.8), width: 2})
        }),
        // Find the center of the multi-polygon
        'geometry': function(feature) {
          var extent = feature.getGeometry().getExtent();
          if (extent) {
            var offsetX = 0, offsetY = 0;
            centre = ol.extent.getCenter(extent);
            return new ol.geom.Point([centre[0]+offsetX,centre[1]+offsetY]);
          }
        }
      })
  ),
  new ol.style.Style({
    'stroke': new ol.style.Stroke({
      'color': feature.get('colour'),
      'width': 2,
    }),
    'fill': new ol.style.Fill({
      'color': getAlphaColour(feature.get('colour'), 0.5)
    })
  })
]
```
