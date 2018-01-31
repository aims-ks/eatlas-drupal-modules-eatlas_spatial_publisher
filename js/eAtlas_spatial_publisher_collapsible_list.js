(function ($) {
	$(document).bind('init-collapsible', function() {

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
					// target_el.height() is always 0 when the content is loaded using "load" function.
					// None of the jQuery events is trigger AFTER the element has been rendered and its height has been set.
					// The best solution I could find is to count the number of children and enable the
					//   expand / collapse feature when there is more than 4 elements.
					//var target_height = target_el.height();

					if (target_el.children().length > 5) {
						target_el.css('max-height', '100px');
						target_el.css('overflow', 'hidden');

						// The collapsible element exists and is NOT empty
						button_el.show();

						button_el.click(function(el) {
							return function() {
								var maxHeight = el.css('max-height');
								var button_label = null;
								if (!maxHeight || maxHeight === 'none') {
									el.css('max-height', '100px');
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
}(jQuery));
