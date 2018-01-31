<?php
/**
 * Original file: www/modules/system/page.tpl.php
 * NOTE: The page title has been moved into the node.tpl.php template for better flexibility.
 */

	$context = new EAtlas_spatial_publisher_template_context($node, NULL);
?>

	<header>
		<div class="header nocontent">
			<?php
			// Find the 3 blocks of the top menu and "hide" them from the header
			$_mainMenuLeft = NULL;
			$_mainMenuRight = NULL;
			$_searchForm = NULL;
			foreach($page['header'] as $blockDelta => $block) {
				if (isset($block['#block']) && is_object($block['#block'])) {
					if ($block['#block']->delta === 'main-menu') {
						$_mainMenuLeft = $block;
						hide($page['header'][$blockDelta]);
					} else if ($block['#block']->subject === 'Main menu right') {
						$_mainMenuRight = $block;
						hide($page['header'][$blockDelta]);
					} else if ($block['#block']->module === 'search' && $block['#block']->delta === 'form') {
						$_searchForm = $block;
						hide($page['header'][$blockDelta]);
					}
				}
			}
			?>
			<div id="main-menus">
				<div class="main-menus-content">
					<div class="menu-left">
						<?php print render($_mainMenuLeft); ?>
					</div>
					<div class="menu-right">
						<?php print render($_searchForm); ?>
						<?php print render($_mainMenuRight); ?>
					</div>
				</div>
			</div>

			<?php print render($page['header']); ?>
			<?php print render($page['header_print']); ?>
		</div>
	</header>

	<section>
		<div class="navigation_map_node">
			<!-- [View] [Edit] [Outline] tabs (for authenticated users) -->
			<?php if ($tabs): ?>
				<div id="tabs-wrapper" class="clearfix">
				<?php print render($tabs); ?></div>
			<?php endif; ?>

			<!-- Floating left (for authenticated users) -->
			<?php if ($page['sidebar_float']): ?>
				<aside>
					<div class="sidebar_float">
						<?php print render($page['sidebar_float']); ?>
					</div>
				</aside>
			<?php endif; ?>
			<!-- end Floating left -->

			<a id="main-content"></a>

			<?php if ($action_links): ?><ul class="action-links"><?php print render($action_links); ?></ul><?php endif; ?>

			<script>
				var extra_content =
					<?php
						// Put page blocks HTML into a JavaScript variable
						// (they will be added to the right panel)
						foreach (array_keys($page['content']) as $key) {
							if ($key !== 'system_main' && $key[0] !== '#') {
								print json_encode(drupal_render($page['content'][$key])) . ' + ';
							}
						}
					?>
					'';
			</script>

			<?php print render($page['content']); ?>
		</div>

	</section>
