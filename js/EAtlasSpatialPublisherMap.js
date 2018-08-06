/**
 * The idea here is to do some sort of layer switching depending on what the user is interested in (what has been clicked)
 * Example:
 *   http://watershedreports.wwf.ca/#canada/by/threat-overall/profile
 */

// TODO Config example
// TODO Write documentation: setPanelContent, loadPanelContent
// TODO Write documentation: loadStateCallback(state)
//   state can be:
//     Object created by EAtlasSpatialPublisherMap
//     Object containing the URL parameters
//     null (initial page)


function EAtlasSpatialPublisherMap(config) {
	this.config = config;

	this.map_container_el = jQuery('.eatlas_spatial_publisher_map');
	this.map_target = jQuery('#' + this.config.target);

	this.panel_button_el = this.map_container_el.find('.map_panel_button');
	this.panel_el = this.map_container_el.find('.map_panel');
	this.initial_panel_width = null;
	this.min_map_width = 30;

	this.map_el = this.map_container_el.find('.map');

	this.panelAnimationDuration = 500;
	this.zoomAnimationDuration = 500;

	this.geoJSONLayerMap = null;
	this.olMap = null;
	this.rootGeoJSONLayer = null;

	// What extent (bounding box) to show when there is nothing selected
	this.mapDefaultExtent = null;

	// {
	//   featureId: {
	//     "feature": feature,
	//     "layer": layer
	//   }
	// }
	this.featureMap = {};

	this.selectedFeatureId = null;
	this.mouseoverFeature = null;
	this.featureBuffer = 5; // in pixels (hitTolerance)

	// List of event listener to call when all layers are loaded
	// {
	//   eventListenerId: callback
	// }
	this.onLayersReadyCallbackMap = {};

	// Used to resize the map after a short delay.
	// This is used to fix the "mouse over" offset issue.
	this.delayedMapResize = null;
}

EAtlasSpatialPublisherMap.prototype.init = function() {
	if (this.config.panelCss) {
		this.panel_el.css(this.config.panelCss);
		if (this.config.panelCss.width) {
			this.initial_panel_width = this.config.panelCss.width;
		}
	}
	if (!this.initial_panel_width) {
		this.initial_panel_width = this.panel_el.css('width') || this.panel_el.width();
	}

	this.panel_el.css('position', 'absolute');
	this.showMap();

	// Auto-adjust map size when window is resized (when viewed as node)
	var navigationMap = jQuery('.navigation_map_node .eatlas_spatial_publisher_map');
	if (navigationMap.length) {
		var adjustMapHeight = function(that) {
			return function() {
				var pageHeight = jQuery(window).height();

				var navigationMapTop = that.map_container_el.position().top;
				var bottomPanelMinHeight = that.isPanelBellowMap() ? 150 : 0;

				// NOTE: The map needs to be smaller than the calculated value (-1px),
				//   otherwise it trigger the browser body scroller when the
				//   window is zoomed to 125% (Google Chrome)
				var mapHeight = Math.floor(pageHeight - navigationMapTop - bottomPanelMinHeight) - 1;
				if (mapHeight < 200) {
					mapHeight = 200;
				}

				that.map_target.height(mapHeight);
				that.map_target.trigger('resize');
			};
		}(this);

		jQuery(window).resize(function() {
			adjustMapHeight();
		});

		adjustMapHeight();
	}

	// Apply CSS
	jQuery(window).resize(function(that) {
		return function() {
			that.onWindowResize();
		};
	}(this));
	this.onWindowResize();

	// NOTE: The resize event is not triggered automatically on anything
	//   but the window object. It needs to be triggered manually.
	this.map_container_el.resize(function (that) {
		return function () {
			that.olMap.updateSize();

			// Also trigger the resize 200ms after the action,
			//   to fix the "mouse over" offset issue.
			if (that.delayedMapResize) {
				window.clearTimeout(that.delayedMapResize);
			}
			that.delayedMapResize = window.setTimeout(function() {
				that.olMap.updateSize();
				that.delayedMapResize = null;
			}, 200);
		};
	}(this));

	// Fix panel width and position
	if (this.panel_el) {
		this.panel_el.width(this.initial_panel_width);
		this.panel_el.css('right', 0);
		this.panel_el.show();
	}

	// Add button event listeners to show / hide the panel
	if (this.panel_button_el) {
		this.panel_button_el.css('right', this.initial_panel_width);
		this.panel_button_el.click(function(that) {
			return function() {
				if (that.panel_el.isExpanded) {
					that.showMap();
				} else {
					that.hideMap();
				}
			};
		}(this));
		if (!this.isPanelBellowMap()) {
			this.panel_button_el.show();
		}
	}


	/* Map */

	// Create the base layer and add it to the list of layers
	var baseLayer = null;
	if (this.config.baseLayer) {
		try {
			// "eval" the style and put the result in the styleConf variable.
			// We need to use "eval" here since there is no way
			//   to define stroke / fill styles using JSON in OpenLayers 3.
			//   The style elements are defined using
			//   instance of OpenLayers objects.
			eval("baseLayer = " + this.config.baseLayer);
		} catch (e) {
			console.log(e);
		}
	}
	// Fallback if baseLayer config is missing or not set properly
	if (!baseLayer) {
		// OpenStreetMap base layer
		baseLayer = new ol.layer.Tile({
			source: new ol.source.OSM()
		});
	}
	var layers = [baseLayer];


	this.geoJSONLayerMap = {};
	for (var i=0; i<this.config.layers.length; i++) {
		var layerConfig = this.config.layers[i];

		// Create a style function for the current layer
		function styleFunction(layerConfig, that) {
			// The default style is created from a closed scope function (or maybe it's just minimised...)
			//   The easiest way to get the default style function is to create an empty layer
			//   and request its style function.
			var defaultVectorStyleFunction = (new ol.layer.Vector({})).getStyle();

			return function(feature, resolution) {
				// Define variables / functions accessible in the scope of the style

				// Set the alpha channel to an hex-colour.
				// NOTE: Alpha-hex-colours (format: '#RRGGBBAA') are not well supported by old browsers.
				var getAlphaColour = function(hexColourStr, alpha) {
					var colourArray = ol.color.asArray(hexColourStr);
					// Clone the colour array
					colourArray = colourArray.slice();
					// Change the alpha of the colour
					colourArray[3] = alpha;
					return colourArray;
				};

				// Define variables / functions accessible in the scope of the style
				var baseStyle = null;
				if (layerConfig.style) {
					try {
						// "eval" the style and put the result in the styleConf variable.
						// We need to use "eval" here since there is no way
						//   to define stroke / fill styles using JSON in OpenLayers 3.
						//   The style elements are defined using
						//   instance of OpenLayers objects.
						eval("baseStyle = " + layerConfig.style);
					} catch (e) {
						console.log(e);
					}
				}

				if (!baseStyle) {
					baseStyle = defaultVectorStyleFunction(feature, resolution)[0].clone();
				}

				// Highlight the selected feature(s) using the selected feature style
				if (feature.getId() === that.selectedFeatureId || feature.get('Network_ID') === that.selectedFeatureId) {
					if (layerConfig.selected_style) {
						try {
							eval("baseStyle = " + layerConfig.selected_style);
						} catch (e) {
							console.log(e);
						}
					} else {
						var styleStroke, baseStrokeWidth, highlightStrokeWidth;
						if (Array.isArray(baseStyle)) {
							for (var j=0; j<baseStyle.length; j++) {
								styleStroke = baseStyle[j].getStroke();
								if (styleStroke) {
									baseStrokeWidth = styleStroke.getWidth();
									highlightStrokeWidth = baseStrokeWidth ? baseStrokeWidth * 3 : 1;
									styleStroke.setWidth(highlightStrokeWidth);
								}
							}
						} else {
							styleStroke = baseStyle.getStroke();
							if (styleStroke) {
								baseStrokeWidth = styleStroke.getWidth();
								highlightStrokeWidth = baseStrokeWidth ? baseStrokeWidth * 3 : 1;
								styleStroke.setWidth(highlightStrokeWidth);
							}
						}
					}
				}

				// Highlight the mouse-over feature using the mouse-over feature style
				if (feature === that.mouseoverFeature) {
					if (layerConfig.mouseover_style) {
						try {
							eval("baseStyle = " + layerConfig.mouseover_style);
						} catch (e) {
							console.log(e);
						}
					} else {
						var styleStroke, baseStrokeWidth, highlightStrokeWidth;
						if (Array.isArray(baseStyle)) {
							for (var j=0; j<baseStyle.length; j++) {
								styleStroke = baseStyle[j].getStroke();
								if (styleStroke) {
									baseStrokeWidth = styleStroke.getWidth();
									highlightStrokeWidth = baseStrokeWidth ? baseStrokeWidth * 3 : 1;
									styleStroke.setWidth(highlightStrokeWidth);
								}
							}
						} else {
							styleStroke = baseStyle.getStroke();
							if (styleStroke) {
								baseStrokeWidth = styleStroke.getWidth();
								highlightStrokeWidth = baseStrokeWidth ? baseStrokeWidth * 3 : 1;
								styleStroke.setWidth(highlightStrokeWidth);
							}
						}
					}
				}

				return baseStyle;
			}
		}

		// How to preload the layer
		//   https://stackoverflow.com/questions/33533911/ol3-force-loading-of-source
		var layer = new ol.layer.Vector({
			title: layerConfig.title,
			opacity: layerConfig.parentId == null ? 1 : 0,
			style: styleFunction(layerConfig, this),
			source: new ol.source.Vector({
				url: layerConfig.url,
				format: new ol.format.GeoJSON()
			})
		});
		layer.config = layerConfig;

		var eventListener = null;
		if (layerConfig.parentId == null) {
			this.rootGeoJSONLayer = layer;
			eventListener = layer.getSource().on('change', function(that, layer, eventListener) {
				return function() {
					var layerId = layer.config.id;
					var layerSource = layer.getSource();
					if (layerSource.getState() === 'ready') {
						that.geoJSONLayerMap[layerId]["state"] = "ready";
						layerSource.un('change', eventListener);
						// Zoom to layer
						if (that.mapDefaultExtent == null) {
							that.mapDefaultExtent = layer.getSource().getExtent();
						}
						that.olMap.getView().fit(that.mapDefaultExtent);
						// Add features to the feature map
						that.addFeaturesToMap(layer);
						// Fire callbacks
						if (that.getLayersState() === "ready") {
							that.fireLayersReadyCallbacks();
						}
					}
				};
			}(this, layer, eventListener));
		} else {
			eventListener = layer.getSource().on('change', function(that, layer, eventListener) {
				return function() {
					var layerId = layer.config.id;
					var layerSource = layer.getSource();
					if (layerSource.getState() === 'ready') {
						that.geoJSONLayerMap[layerId]["state"] = "ready";
						layerSource.un('change', eventListener);
						// Hide the layer
						layer.setVisible(false);
						// Add features to the feature map
						that.addFeaturesToMap(layer);
						// Fire callbacks
						if (that.getLayersState() === "ready") {
							that.fireLayersReadyCallbacks();
						}
					}
				};
			}(this, layer, eventListener));
		}

		this.geoJSONLayerMap[layerConfig.id] = {
			"layer": layer,
			"state": "loading"
		};
		layers.push(layer);
	}

	this.olMap = new ol.Map({
		layers: layers,
		target: this.config.target,
		// Disable mouse wheel zoom
		interactions: ol.interaction.defaults({
			altShiftDragRotate: false,
			pinchRotate: false,
			mouseWheelZoom: false,
			doubleClickZoom: false
		}),
		controls: ol.control.defaults({
			rotate: false,
			attributionOptions: ({
				collapsible: false
			})
		}),
		// Create a view. It doesn't really matter where it's pointing at, it will be re-positioned
		view: new ol.View({
			center: [0, 0],
			zoom: 2
		})
	});

	// Get extent (bounding box) from config, if present
	if (this.config.bbox) {
		// Projection of the config and projection of the map,
		//   used to reproject the default extent (initial zoom bounding box).
		var mapProjection = this.olMap.getView().getProjection();
		var lonLatProjection = ol.proj.get("EPSG:4326");
		// API: https://openlayers.org/en/latest/apidoc/ol.extent.html
		// ** IMPORTANT **
		//   ol.proj.transform doesn't work when negative numbers are passed as string (works fine with positive numbers).
		//   The "parseFloat" here is to prevent error when the config send numbers as string.
		this.mapDefaultExtent = ol.extent.boundingExtent(
			[
				ol.proj.transform([parseFloat(this.config.bbox.west), parseFloat(this.config.bbox.south)], lonLatProjection, mapProjection),
				ol.proj.transform([parseFloat(this.config.bbox.east), parseFloat(this.config.bbox.north)], lonLatProjection, mapProjection)
			]
		);

		// Set the map view
		this.olMap.getView().fit(this.mapDefaultExtent);

		/*
		// Display the default extent (bounding box) on the map
		//   Used for debugging
		var _mapDefaultExtentGeoJSON = {
			'type': 'FeatureCollection',
			'crs': {
				'type': 'name',
				'properties': {
					'name': mapProjection.getCode()
				}
			},
			'features': [{
				'type': 'Feature',
				'geometry': {
					'type': 'Polygon',
					'coordinates': [
						[
							[this.mapDefaultExtent[0], this.mapDefaultExtent[1]],
							[this.mapDefaultExtent[0], this.mapDefaultExtent[3]],
							[this.mapDefaultExtent[2], this.mapDefaultExtent[3]],
							[this.mapDefaultExtent[2], this.mapDefaultExtent[1]],
							[this.mapDefaultExtent[0], this.mapDefaultExtent[1]]
						]
					]
				}
			}]
		};

		var _mapDefaultExtentLayerSource = new ol.source.Vector({
			features: (new ol.format.GeoJSON()).readFeatures(_mapDefaultExtentGeoJSON)
		});

		var _mapDefaultExtentLayer = new ol.layer.Vector({
			source: _mapDefaultExtentLayerSource
		});

		this.olMap.addLayer(_mapDefaultExtentLayer);
		*/
	}

	var highlightFeature = function(that) {
		return function(pixel) {
			var feature = that.olMap.forEachFeatureAtPixel(
				pixel,
				function(feature) {
					return feature;
				},
				{
					hitTolerance: that.featureBuffer
				}
			);

			if (feature !== that.mouseoverFeature) {
				that.mouseoverFeature = feature;

				// Change the mouse cursor when the mouse is over a polygon
				if (that.mouseoverFeature) {
					that.map_target.css("cursor", "pointer");
				} else {
					that.map_target.css("cursor", "auto");
				}

				// Mark GeoJSON layers for re-draw
				// (they will be redrawn on the next "time frame")
				for (var layerId in that.geoJSONLayerMap) {
					if (that.geoJSONLayerMap.hasOwnProperty(layerId)) {
						var layer = that.geoJSONLayerMap[layerId]["layer"];
						layer.changed();
					}
				}
			}
		};
	}(this);


	this.olMap.on('pointermove',
		function(map) {
			return function(evt) {
				if (evt.dragging) {
					return;
				}
				var pixel = map.getEventPixel(evt.originalEvent);
				highlightFeature(pixel);
				// Propagate the event - Safari browser needs this
				return true;
			};
		}(this.olMap)
	);

	// On click event listener
	//   https://stackoverflow.com/questions/43004561/how-do-i-add-a-click-event-on-openlayers-4
	// NOTE: Use 'pointerup' instead of 'click' to handle overly sensitive devices
	//   that will trigger a 'drag' or 'dblclick' instead of 'click'.
	//   'pointerdown' is used to track the distance travelled (during 'drag')
	var pointerDownEvent = null;
	this.olMap.on('pointerdown', function(that) {
		return function(event) {
			pointerDownEvent = event;
			// Propagate the event - Safari browser needs this
			return true;
		};
	}(this));
	this.olMap.on('pointerup', function(that) {
		return function(event) {
			// Reset the mouse-over feature
			that.mouseoverFeature = null;

			var selectedFeature = null;
			var selectedLayer = null;

			that.olMap.forEachFeatureAtPixel(
				event.pixel,
				function(feature, layer) {
					selectedFeature = feature;
					that.selectedFeatureId = feature.getId();
					selectedLayer = layer;
					return true;
				},
				{
					hitTolerance: that.featureBuffer
				}
			);

			var dragDistance = 0;
			if (pointerDownEvent && pointerDownEvent.pixel && event && event.pixel) {
				var xDiff = pointerDownEvent.pixel[0] - event.pixel[0],
					yDiff = pointerDownEvent.pixel[1] - event.pixel[1];
				dragDistance = Math.sqrt(xDiff * xDiff + yDiff * yDiff);
			}

			// A drag distance of less than 10 pixel is considered a click
			if (dragDistance < 10) {
				if (selectedFeature && selectedLayer) {
					// Zoom to the clicked feature
					that.zoomToFeature(selectedFeature, that.getChildLayers(selectedLayer));
				} else {
					// Reset the map
					// Zoom to the root layer
					that.zoomToFeature(null, [that.rootGeoJSONLayer]);
					selectedLayer = that.rootGeoJSONLayer;
				}

				if (that.config.onFeatureClick) {
					that.config.onFeatureClick(selectedLayer, selectedFeature);
				}
			}
			// Propagate the event - Safari browser needs this
			return true;
		};
	}(this));
};

EAtlasSpatialPublisherMap.prototype.selectFeatureId = function(featureId) {
	this.selectedFeatureId = featureId;
};

EAtlasSpatialPublisherMap.prototype.getLayersState = function() {
	var allLayersReady = true;
	for (key in this.geoJSONLayerMap) {
		if (this.geoJSONLayerMap.hasOwnProperty(key) && this.geoJSONLayerMap[key]["state"] !== "ready") {
			allLayersReady = false;
			break;
		}
	}
	return allLayersReady ? "ready" : "loading";
};

EAtlasSpatialPublisherMap.prototype.onLayersReady = function(eventListenerId, callback) {
	this.onLayersReadyCallbackMap[eventListenerId] = callback;
};

EAtlasSpatialPublisherMap.prototype.unLayersReady = function(eventListenerId) {
	if (this.onLayersReadyCallbackMap[eventListenerId]) {
		delete this.onLayersReadyCallbackMap[eventListenerId];
	}
};

EAtlasSpatialPublisherMap.prototype.fireLayersReadyCallbacks = function() {
	for (eventListenerId in this.onLayersReadyCallbackMap) {
		if (this.onLayersReadyCallbackMap.hasOwnProperty(eventListenerId)) {
			var callback = this.onLayersReadyCallbackMap[eventListenerId];
			callback();
		}
	}
};


// Internal function
//   Adjust panel CSS
EAtlasSpatialPublisherMap.prototype.onWindowResize = function() {
	if (this.panel_el) {
		if (this.isPanelBellowMap()) {
			this.panel_button_el.hide();
			this.map_el.css('margin-right', '0');
			this.map_container_el.trigger('resize');
		} else {
			this.panel_button_el.show();
			this.panel_el.show();

			if (this.panel_el.isExpanded) {
				this.panel_el.width(jQuery(window).width() - this.min_map_width);
			} else if (this.config.panelCss) {
				this.panel_el.css(this.config.panelCss);
			}

			this.panel_button_el.css('right', this.panel_el.width() + 'px');
			this.map_el.css('margin-right', this.panel_el.width() + 'px');

			this.map_container_el.trigger('resize');
		}
	}

	// Adjust sidebar height (the side bar shown when a user is logged)
	jQuery('.sidebar_float').each(function(that) {
		return function(index, object) {
			var panelPosition = that.panel_el.css('position');
			var sidebarEl = jQuery(object);

			// Only set 'max-height' when the map panel is display to the right of the map (position absolute).
			// We don't need 'max-height' when the map panel is display bellow the map (position static)
			var sidebarMaxHeight = 'none';
			if (panelPosition === 'absolute') {
				var mapHeight = parseInt(that.map_el.css('height'));
				var sidebarMarginTop = parseInt(sidebarEl.css('margin-top'));
				sidebarMaxHeight = (mapHeight - sidebarMarginTop) + 'px';
			}

			sidebarEl.css('max-height', sidebarMaxHeight);
		};
	}(this));
};

EAtlasSpatialPublisherMap.prototype.showMap = function() {
	if (this.panel_el && !this.isPanelBellowMap()) {
		this.panel_el.animate({ width: this.initial_panel_width }, {
			duration: this.panelAnimationDuration
		});
		this.panel_button_el.animate({ right: this.initial_panel_width }, {
			duration: this.panelAnimationDuration,
			complete: function(that) {
				return function() {
					that.panel_button_el.html('&lt;');
					// Manually trigger window resize event to help Image Slider adjust itself
					jQuery(window).trigger('resize');
				};
			}(this)
		});
		this.map_el.animate({ marginRight: this.initial_panel_width }, {
			duration: this.panelAnimationDuration,
			step: function(that) {
				return function() {
					that.map_container_el.trigger('resize');
				};
			}(this)
		});
		this.panel_el.isExpanded = false;
	}
};

EAtlasSpatialPublisherMap.prototype.hideMap = function() {
	if (this.panel_el && !this.isPanelBellowMap()) {
		this.panel_el.animate({ width: jQuery(window).width() - this.min_map_width }, {
			duration: this.panelAnimationDuration
		});
		this.panel_button_el.animate({ right: jQuery(window).width() - this.min_map_width }, {
			duration: this.panelAnimationDuration,
			complete: function(that) {
				return function() {
					that.panel_button_el.html('&gt;');
					// Manually trigger window resize event to help Image Slider adjust itself
					jQuery(window).trigger('resize');
				};
			}(this)
		});
		this.map_el.animate({ marginRight: jQuery(window).width() - this.min_map_width }, {
			duration: this.panelAnimationDuration,
			step: function(that) {
				return function() {
					that.map_container_el.trigger('resize');
				};
			}(this)
		});
		this.panel_el.isExpanded = true;
	}
};

EAtlasSpatialPublisherMap.prototype.isPanelBellowMap = function() {
	return this.panel_el && this.panel_el.css('position') !== 'absolute';
};



EAtlasSpatialPublisherMap.prototype.setPanelContent = function(content) {
	this.panel_el.find('.content').html(content);
	this.panel_el.trigger('content-change');
};

EAtlasSpatialPublisherMap.prototype.loadPanelContent = function(contentUrl) {
	this.showPanelLoading();
	this.panel_el.find('.content').load(contentUrl, function(that) {
		return function() {
			that.hidePanelLoading();
			that.panel_el.trigger('content-change');
		};
	}(this));
};

EAtlasSpatialPublisherMap.prototype.showPanelLoading = function() {
	this.panel_el.find('.content-wrapper').hide();
	this.panel_el.find('.loading').show();
};

EAtlasSpatialPublisherMap.prototype.hidePanelLoading = function() {
	this.panel_el.find('.loading').hide();
	this.panel_el.find('.content-wrapper').show();
};


/* Map methods */

EAtlasSpatialPublisherMap.prototype.getChildLayers = function(layer) {
	var childrenLayers = [];
	for (var layerId in this.geoJSONLayerMap) {
		if (this.geoJSONLayerMap.hasOwnProperty(layerId)) {
			var childLayer = this.geoJSONLayerMap[layerId]["layer"];
			if (childLayer.config.parentId === layer.config.id) {
				childrenLayers.push(childLayer);
			}
		}
	}

	return childrenLayers;
};

EAtlasSpatialPublisherMap.prototype.getFeatureLayer = function(featureId, callback) {
	this.getFeatureIdMap("getFeatureLayer", function(that, featureId) {
		return function(featureIdMap) {
			if (featureIdMap && featureIdMap.hasOwnProperty(featureId)) {
				callback(featureIdMap[featureId]["layer"]);
			}
		};
	}(this, featureId));
};

EAtlasSpatialPublisherMap.prototype.zoomToFeatureId = function(featureId) {
	this.getFeatureIdMap("zoomToFeatureId", function(that, featureId) {
		return function(featureIdMap) {
			if (featureIdMap && featureIdMap.hasOwnProperty(featureId)) {
				var featureLayer = featureIdMap[featureId]["layer"];
				var parentLayer = that.getParentLayer(featureLayer);
				that.zoomToFeature(
						featureIdMap[featureId]["feature"],
						that.getChildLayers(parentLayer)
				);
			}
		};
	}(this, featureId));
};

EAtlasSpatialPublisherMap.prototype.getParentLayer = function(layer) {
	if (!layer.hasOwnProperty("config") || !layer.config.hasOwnProperty("parentId") || !this.geoJSONLayerMap[layer.config.parentId]["layer"]) {
		return this.rootGeoJSONLayer;
	}
	return this.geoJSONLayerMap[layer.config.parentId]["layer"];
};

EAtlasSpatialPublisherMap.prototype.resetZoom = function() {
	this.selectedFeatureId = null;
	this.mouseoverFeature = null;
	this.zoomToFeature(null, [this.rootGeoJSONLayer]);
};

// NOTE: The map can not be initialised when the map load since the layers are not available at that point.
EAtlasSpatialPublisherMap.prototype.getFeatureIdMap = function(callbackId, callback) {
	var layerState = this.getLayersState();
	if (layerState === "ready") {
		callback(this.featureMap);
	} else {
		this.onLayersReady(callbackId, function(that, callbackId, callback) {
			return function() {
				that.unLayersReady(callbackId);
				callback(that.featureMap);
			};
		}(this, callbackId, callback));
	}
};

EAtlasSpatialPublisherMap.prototype.addFeaturesToMap = function(layer) {
	var layerSource = layer.getSource();
	layerSource.forEachFeature(function(that) {
		return function(feature) {
			that.featureMap[feature.getId()] = {
				"feature": feature,
				"layer": layer
			};
		};
	}(this));
};

EAtlasSpatialPublisherMap.prototype.zoomToFeature = function(feature, layersToShow) {
	if (layersToShow && layersToShow.length > 0) {
		var layerState = this.getLayersState();
		if (layerState === "ready") {
			this._zoomToFeature(feature, layersToShow);
		} else {
			this.onLayersReady("zoomToFeature", function(that, feature, layersToShow) {
				return function() {
					that.unLayersReady("zoomToFeature");
					that._zoomToFeature(feature, layersToShow);
				};
			}(this, feature, layersToShow));
		}
	}
};

EAtlasSpatialPublisherMap.prototype._zoomToFeature = function(feature, layersToShow) {
	// Calculate the extent to zoom to
	var extent = null,
		i, layer, layerToShow;
	if (feature) {
		// A feature is selected - Zoom to feature
		extent = feature.getGeometry().getExtent();
	} else if (this.mapDefaultExtent != null) {
		// No feature is selected - Zoom to the default extent
		extent = this.mapDefaultExtent;
	} else if (layersToShow && layersToShow.length > 0) {
		// No feature is selected and there is not default extent - Zoom to layers extent (combined)
		extent = ol.extent.createEmpty();
		for (i=0; i<layersToShow.length; i++) {
			layer = layersToShow[i];
			ol.extent.extend(extent, layer.getSource().getExtent());
		}
	}

	// Zoom to extent only if there is an extent to zoom to
	if (extent) {
		// Find out which layers needs to be hidden
		// (all the GeoJSON which are visible and not in the layersToShow list)
		var layersToHide = [];
		if (layersToShow && layersToShow.length > 0) {
			for (var layerId in this.geoJSONLayerMap) {
				if (this.geoJSONLayerMap.hasOwnProperty(layerId)) {
					layer = this.geoJSONLayerMap[layerId]["layer"];
					if (layer.getVisible()) {
						// Check to be sure the layer is not in the list of layer to show
						var ignore = false;
						for (i=0; i<layersToShow.length; i++) {
							layerToShow = layersToShow[i];
							if (layer === layerToShow) {
								ignore = true;
								break;
							}
						}
						if (!ignore) {
							layersToHide.push(layer);
						}
					}
				}
			}
		}

		// Show the layers to show, with an opacity of 0 (unless they are already shown)
		if (layersToShow.length > 0) {
			for (i=0; i<layersToShow.length; i++) {
				layerToShow = layersToShow[i];
				if (!layerToShow.getVisible()) {
					layerToShow.setVisible(true);
					layerToShow.setOpacity(0);
				}
			}
			// Apply the layer visibility changes immediately
			this.olMap.renderSync();
		}

		// Slowly zoom to extent
		//   while showing the layers to show
		//   while hiding the layers to hide.
		this.olMap.getView().fit(extent, {
			duration: this.zoomAnimationDuration,
			easing: function(layersToHide, layersToShow) {
				return function(t) {
					var i, time = ol.easing.inAndOut(t);
					if (layersToShow && layersToShow.length > 0) {
						for (i=0; i<layersToShow.length; i++) {
							var layerToShow = layersToShow[i];
							if (layerToShow.getOpacity() < 1) {
								layerToShow.setOpacity(time);
							}
						}
					}
					if (layersToHide && layersToHide.length > 0) {
						for (i=0; i<layersToHide.length; i++) {
							var layerToHide = layersToHide[i];
							if (layerToHide.getOpacity() > 0) {
								layerToHide.setOpacity(1-time);
							}
						}
					}
					return time;
				};
			}(layersToHide, layersToShow),

			// *IMPORTANT*
			// The "easing" function will get cancelled if the user
			//   frenetically click the map.
			// The callback MUST set the visibility and the final opacity
			//   for each of the involved layers to fix incomplete animation.
			callback: function(layersToHide, layersToShow) {
				return function() {
					var i;
					if (layersToHide.length > 0) {
						for (i=0; i<layersToHide.length; i++) {
							var layerToHide = layersToHide[i];
							layerToHide.setVisible(false);
							layerToHide.setOpacity(0);
						}
					}
					if (layersToShow.length > 0) {
						for (i=0; i<layersToShow.length; i++) {
							var layerToShow = layersToShow[i];
							layerToShow.setVisible(true);
							layerToShow.setOpacity(1);
						}
					}
				};
			}(layersToHide, layersToShow)
		});
	}
};
