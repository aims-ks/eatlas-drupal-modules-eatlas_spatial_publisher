(function ($) {
	$(document).ready(
		function() {
			if ($('.map_panel .goto-top-button').length) {
				// Hide the "top" button
				// NOTE: If the user doesn't have JavaScript, the button will always be visible.
				$('.map_panel .goto-top-button').css('bottom', -100);

				// Slowly show the button as the panel scroll down
				$('.map_panel').scroll(
					function() {
						mapPanel = $(this);
						panelTop = mapPanel.attr('scrollTop');
						topButton = mapPanel.find('.goto-top-button');

						if (panelTop < 105) {
							topButton.css('bottom', panelTop - 100 + 'px');
						} else {
							topButton.css('bottom', 5 + 'px');
						}
					}
				);

				// Same thing for Mobile device, using the Window
				if ($('#content-top').length) { // If "#content-top" exist
					$(window).scroll(
						function() {
							// NOTE: Subtract the top position of the "content-top",
							//   we don't want the button to show up when the page
							//   is already at the "top" or even higher
							//   (when the map is visible).
							windowTop = $(window).scrollTop() - $('#content-top').position().top;
							topButton = $('.goto-top-button');

							if (windowTop < 105) {
								topButton.css('bottom', windowTop - 100 + 'px');
							} else {
								topButton.css('bottom', 5 + 'px');
							}
						}
					);
				}
			}
		}
	);
}(jQuery));
