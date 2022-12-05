<?php

/**
 * @file
 * Navigation-related hook implementations.
 */

/**
 * Implements hook_preprocess_breadcrumb().
 */
function slac_preprocess_breadcrumb(array &$variables) {
  if ($variables['breadcrumb']) {
    /** @var \Drupal\Core\Render\Renderer $renderer */
    $renderer = \Drupal::service('renderer');
    $variables['include_current_page']
      = theme_get_setting('include_current_page_in_breadcrumb', 'slac') ?? TRUE;
    $themeConfig = \Drupal::config('slac.settings');
    $renderer->addCacheableDependency($variables, $themeConfig);

    if ($variables['include_current_page']) {
      $request = \Drupal::request();
      $route_match = \Drupal::routeMatch();
      $variables['#cache']['contexts'][] = 'route';
      $page_title = \Drupal::service('title_resolver')->getTitle($request, $route_match->getRouteObject());

      if (!empty($page_title)) {
        $variables['page_title'] = $page_title;
        $variables['breadcrumb'][] = [
          'text' => $page_title,
        ];
      }
    }
  }
}

function slac_theme_suggestions_menu_alter(array &$suggestions, array $vars) {
  if ($vars['menu_name'] == 'main' &&
    !empty($vars['menu_block_configuration']) &&
    $vars['menu_block_configuration']['level'] == 2
  ) {
    $suggestions[] = $vars['theme_hook_original'] . '__' . 'side_nav';
  }
}

/**
 * hook_preprocess_pager().
 */
function slac_preprocess_pager(array &$vars) {
  $element = $vars['pager']['#element'];
  $pager_manager = \Drupal::service('pager.manager');
  /** @var \Drupal\Core\Pager\Pager $pager */
  $pager = $pager_manager
    ->getPager($element);
  if ($pager) {
    $pager_max = $pager->getTotalPages();
    if (!empty($vars['items']['first'])) {
      $vars['items']['first']['text'] = '1';
    }
    if (!empty($vars['items']['last'])) {
      $vars['items']['last']['text'] = "${pager_max}";
    }
  }
}
