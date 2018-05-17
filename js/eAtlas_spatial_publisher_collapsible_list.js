(function ($) {
	$(document).ready(function() {
		// NOTE: The custom event 'content-change' is trigger by "EAtlasSpatialPublisherMap.js"
		$('.eatlas_spatial_publisher_map .map_panel').bind('content-change', function() {

			// Add functionality to hide / show an HTML element specified by the attribute "target".
			$('.collapsible-button').each(function(index, button) {
				var button_el = $(button);
				var target_id = button_el.attr('target');
				if (target_id) {
					var target_el = $('#' + target_id);
					if (target_el) {
						target_el.hide();
						if (target_el.children().length > 0) {
							// The collapsible element exists and is NOT empty
							button_el.show();

							button_el.click(function(el) {
								return function() {
									el.toggle();

									var button_label = el.is(":visible") ? button_el.attr("collapseLabel") : button_el.attr("expandLabel");
									if (button_label) {
										button_el.html(button_label);
									}
								};
							}(target_el));
						} else {
							// The collapsible element is empty
							button_el.hide();
						}
					} else {
						// The collapsible element doesn't exists
						button_el.hide();
					}
				}
			});

			// Add functionality to shrink / expend an HTML element specified by the attribute "target".
			// When the element is shrank, it's height is 100px.
			$('.collapsible-minheight-button').each(function(index, button) {
				var button_el = $(button);
				var target_id = button_el.attr('target');
				if (target_id) {
					var target_el = $('#' + target_id);
					if (target_el) {

						if (target_el.children().length > 3) {
							target_el.css('max-height', '200px');
							target_el.css('overflow', 'hidden');

							// The collapsible element exists and is NOT empty
							button_el.show();

							button_el.click(function(el) {
								return function() {
									var maxHeight = el.css('max-height');
									var button_label = null;
									if (!maxHeight || maxHeight === 'none') {
										el.css('max-height', '200px');
										button_label = button_el.attr("expandLabel");
									} else {
										el.css('max-height', 'none');
										button_label = button_el.attr("collapseLabel");
									}

									if (button_label) {
										button_el.html(button_label);
									}
								};
							}(target_el));
						} else {
							// The collapsible element is empty
							button_el.hide();
						}
					} else {
						// The collapsible element doesn't exists
						button_el.hide();
					}
				}
			});
		});
	});
}(jQuery));
