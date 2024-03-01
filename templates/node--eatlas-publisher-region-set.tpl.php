<?php
$eatlas_spatial_publisher_path = drupal_get_path('module', 'eatlas_spatial_publisher');

include_once($eatlas_spatial_publisher_path . '/eatlas_spatial_publisher_template_parser.inc');
drupal_add_js($eatlas_spatial_publisher_path . '/js/EAtlasSpatialPublisherMap.js');

$image_url = NULL;
if (property_exists($node, 'field_preview') &&
        isset($node->field_preview[LANGUAGE_NONE]) &&
        isset($node->field_preview[LANGUAGE_NONE][0]) &&
        isset($node->field_preview[LANGUAGE_NONE][0]['fid'])) {
    $image_file = file_load($node->field_preview[LANGUAGE_NONE][0]['fid']);
    if ($image_file) {
      // @ToDo remove 'amps' reference. The style value should be dynamic
        $image_url = image_style_url('amps_header_image', $image_file->uri);
        $imageTitleValues = field_get_items('file', $image_file, 'media_title');
        $imageTitle = '';
        if (isset($imageTitleValues[0]['safe_value'])) {
          $imageTitle = $imageTitleValues[0]['safe_value'];
        }
    }
}

$body_field = field_view_field('node', $node, 'body', 'default');
$body = render($body_field);

$rendered_blocks = '';
$content_blocks = block_get_blocks_by_region('content');
foreach ($content_blocks as $key => $value) {
    if ($key !== 'system_main' && $key[0] !== '#') {
        $rendered_blocks .= render($value);
    }
}

// Determine the content of the map_panel
// NOTE: We want the content to be in the panel on page load,
//   to help search engine indexing the page content without
//   having to execute complex JavaScript.
//   It might also help with accessibility
//   (browser without JavaScript capabilities).
$initial_page_content = '<div class="panel-header">' .
    '<h1 class="title">' . $node->title . '</h1>' .
    '</div>' .
    ($image_url && $imageTitle ?
        '<div class="flexslider-wrapper">' .
            '<div class="flexslider-image"><img src="' . $image_url . '" alt="' . $imageTitle . '" title="' . $imageTitle . '"></div>' .
        '</div>' : '') .
    ($body ? $body : '') .
    // Add Drupal page blocks
    $rendered_blocks;

$initial_page_title = eatlas_spatial_publisher_get_page_title($node);

$feature_id = isset($_GET['featureId']) ? filter_xss($_GET['featureId']) : NULL;
$feature_node = NULL;

$template_title = $initial_page_title;
$template_content = $initial_page_content;
if ($feature_id) {
    $site_tid = $node->field_eatlas_publisher_tid[LANGUAGE_NONE][0]['value'];
    $feature_node = eatlas_spatial_publisher_regions_set_get_feature_node($site_tid, $feature_id);

    $template_title = _eatlas_spatial_publisher_regions_set_set_feature_template_title($feature_node, $feature_id);
    $template_content = _eatlas_spatial_publisher_regions_set_get_feature_template($feature_node, $feature_id);
}

?>

<div id="node-<?php print $node->nid; ?>" class="eatlas_spatial_publisher_map">
    <div class="map_container">
        <div id="map-wrapper" class="map" tabindex="0"></div>

        <div class="map_panel_button">&lt;</div>
        <div class="map_panel">
            <div class="loading"></div>
            <div class="content-wrapper" id="content-top">
                <div class="content">
                    <?php print $template_content; ?>
                </div>
                <div class="goto-top">
                    <a class="goto-top-button" href="#content-top"><img src="/<?php print $eatlas_spatial_publisher_path; ?>/img/top-arrow.svg" style="display: block; margin: 0 auto;">TOP</a>
                </div>
            </div>
        </div>
    </div>
</div>

<script>
    (function ($) {
        $(document).ready(function() {
            // Trigger 'content-change' event, to initialise event listeners.
            $('.eatlas_spatial_publisher_map .map_panel').trigger('content-change');

            // Create a EAtlasSpatialPublisherMap instance and initialise it.
            // NOTE: The EAtlasSpatialPublisherMap class is defined in "js/EAtlasSpatialPublisherMap.js"
            var currentState = null;

            var eAtlasSpatialPublisherMap = new EAtlasSpatialPublisherMap({
                target: 'map-wrapper',
                baseLayer: <?php print eatlas_spatial_publisher_get_baselayer($node); ?>,
                layers: <?php print eatlas_spatial_publisher_get_layers_js_array($node); ?>,
                bbox: <?php print eatlas_spatial_publisher_get_initial_zoom_bbox($node); ?>,
                maxZoom: <?php print eatlas_spatial_publisher_get_max_zoom($node); ?>,
                panelCss: {
                    'width': '45%',
                    'min-width': '400px'
                },
                onFeatureClick: featureClicked
            });

            eAtlasSpatialPublisherMap.init();

            // URL Addressability
            $(window).bind('popstate', function(e) {
                var state = e.originalEvent.state;
                if (!state) {
                    state = getURLParameters();
                }
                if (!stateEquals(currentState, state)) {
                    currentState = state;
                    loadState(state);
                }
            });
            currentState = getURLParameters();
            if (currentState.featureId) {
                eAtlasSpatialPublisherMap.selectFeatureId(currentState.featureId);
                eAtlasSpatialPublisherMap.zoomToFeatureId(currentState.featureId);
            }

            function changeState(state) {
                var urlPath = '';
                if (state) {
                    for (var key in state) {
                        if (state.hasOwnProperty(key)) {
                            urlPath += (urlPath ? '&' : '?') +
                                    encodeURIComponent(key) + '=' +
                                    encodeURIComponent(state[key]);
                        }
                    }
                }

                loadState(state);
                // NOTE: The pushState method doesn't work with an empty string.
                window.history.pushState(state, "", urlPath ? urlPath : "?");
            }

            function loadState(state) {
                if (state && state.featureId) {
                    eAtlasSpatialPublisherMap.getFeatureLayer(state.featureId, function(featureId) {
                        return function(layer) {
                            eAtlasSpatialPublisherMap.loadPanelTitle(
                                '/eatlas_spatial_publisher/' + layer.config.id + '/featuretitle/' + state.featureId
                            );
                            eAtlasSpatialPublisherMap.loadPanelContent(
                                '/eatlas_spatial_publisher/' + layer.config.id + '/feature/' + state.featureId
                            );
                        };
                    }(state.featureId));
                    eAtlasSpatialPublisherMap.selectFeatureId(state.featureId);
                    eAtlasSpatialPublisherMap.zoomToFeatureId(state.featureId);
                } else {
                    setDefaultPanelContent();
                    eAtlasSpatialPublisherMap.resetZoom();
                }
            }

            function stateEquals(stateA, stateB) {
                function safeState(state) {
                    if (!state || !state.hasOwnProperty('featureId') || !state.featureId) {
                        return {
                            'featureId': ''
                        };
                    }
                    return state;
                }

                var safeStateA = safeState(stateA);
                var safeStateB = safeState(stateB);

                return safeStateA.featureId === safeStateB.featureId;
            }


            function featureClicked(layer, feature) {
                var newState = null;
                var newTitleUrl = null;
                var newContentUrl = null;

                if (layer && feature) {
                    var nodeId = layer.config.id;
                    var featureId = feature.getId();

                    // If there is only one children, zoom to that child
                    var children = feature.get('_childNodes');
                    if (children != null && children.length === 1) {
                        while (children.properties != null && children.properties._childNodes != null && children.properties._childNodes.length === 1) {
                            children = children.properties._childNodes;
                        }
                        var uniqueChild = children[0];
                        nodeId = uniqueChild.nid;
                        featureId = uniqueChild.id;
                    }

                    newState = {
                        'featureId': featureId
                    };

                    newTitleUrl = '/eatlas_spatial_publisher/' + nodeId + '/featuretitle/' + featureId;
                    newContentUrl = '/eatlas_spatial_publisher/' + nodeId + '/feature/' + featureId;
                }

                if (!stateEquals(currentState, newState)) {
                    currentState = newState;
                    changeState(newState);
                    if (newContentUrl && newTitleUrl) {
                        eAtlasSpatialPublisherMap.loadPanelTitle(newTitleUrl);
                        eAtlasSpatialPublisherMap.loadPanelContent(newContentUrl);
                    } else {
                        setDefaultPanelContent();
                    }
                }
            }

            function setDefaultPanelContent() {
                var panelTitle = <?php print json_encode($initial_page_title); ?>;
                var panelContent = <?php print json_encode($initial_page_content); ?>;
                eAtlasSpatialPublisherMap.setPanelTitle(panelTitle);
                eAtlasSpatialPublisherMap.setPanelContent(panelContent);

                // Add the "cog" to edit blocks (if any)
                // NOTE: The "cog" is added by JavaScript. Since this panel
                //   is drawn afterwards, the script needs to be ran again.
                if (Drupal.behaviors.contextualLinks) {
                    Drupal.behaviors.contextualLinks.attach(document);
                }
            }

            function getURLParameters() {
                // NOTE: This can easily be done using the JavaScript URL object,
                //   but Internet Explorer doesn't support it (it's always him...)

                // window.location => Page URL
                //   .search       => URL parameters part
                //   .substring(1) => Remove the "?" at the beginning
                var parameters = {};
                var query = window.location.search.substring(1);
                if (query) {
                    var key, value, pos, entries = query.split("&");
                    for (var i=0; i<entries.length; i++) {
                        // Separate key from value
                        //   Key and value are decoded using "decodeURIComponent"
                        // NOTE: It's safer to use "indexOf", in case the value contains an unencodded "="
                        pos = entries[i].indexOf("=");
                        if (pos < 0) {
                            key = decodeURIComponent(entries[i]);
                            value = "";
                        } else {
                            key = decodeURIComponent(entries[i].substring(0, pos));
                            value = decodeURIComponent(entries[i].substring(pos+1));
                        }

                        // Prevent key from clashing with internal Array attributes
                        // Examples: "toString", "valueOf", "isPrototypeOf", "hasOwnProperty", "get", "set", etc.
                        while (typeof parameters[key] !== "undefined" && !parameters.hasOwnProperty(key)) {
                            key = "_" + key;
                        }

                        // If first entry with this key (enter as string)
                        if (typeof parameters[key] === "undefined") {
                            parameters[key] = value;

                        // If second entry with this key (enter as array with first value)
                        } else if (typeof parameters[key] === "string") {
                            var arr = [parameters[key], value];
                            parameters[key] = arr;

                        // If third or later entry with this key (append value to array)
                        } else if (typeof parameters[key] === "array") {
                            parameters[key].push(value);
                        }
                    }
                }
                return parameters;
            }
        });
    }(jQuery));

</script>
