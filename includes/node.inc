<?php

/**
 * @file
 * Node-related hook implementations.
 */

use Drupal\node\Entity\Node;
use Drupal\paragraphs\Entity\Paragraph;
use Drupal\Core\Url;
use Drupal\Component\Utility\Html;

/**
 * Implements hook_preprocess_node().
 */
function slac_preprocess_node(&$variables) {
  // Array of view modes to add regions to.
  $allowed_view_modes = ['full'];

  if (in_array($variables['view_mode'], $allowed_view_modes)) {
    // Array of regions to add to node template.
    $allowed_regions = ['status'];
    _add_regions_to_template($allowed_regions, $variables);
  }

  // Some content types that may display a table of contents. First check to make
  // sure that the view_mode is full (and not card, etc.) indicating that the
  // node's detail page is being displayed. If the node is a type that displays a
  // table of contents, first find the TOC markers if any in the content
  // paragraphs field and then create a themeable list of markers as an array
  // that is assigned to a template variable.
  if ($variables['view_mode'] == 'full') {
    $toc_content_types = ['landing_page', 'page', 'news_article'];
    $toc_container_field = 'field_paragraphs';

    /** @var \Drupal\node\NodeInterface $node */
    $node = $variables['node'];

    if (in_array($node->getType(), $toc_content_types)) {
      if ($items = slac_preprocess_node_find_toc_markers($node, $toc_container_field)) {
        $variables['toc_list'] = slac_preprocess_node_generate_toc_list($items);
      }
    }

    // Determine hte type of the hero paragraph if the field exists.
    if ($node->hasField('field_hero_paragraph')) {
      if($node->get('field_hero_paragraph')->first()) {
        $hero_field = $node->get('field_hero_paragraph')->first()->getValue();

        /** @var Paragraph $hero_paragraph */
        $hero_paragraph = Paragraph::load($hero_field['target_id']);
        $variables['hero_type'] = $hero_paragraph->getType();

        // In the special case that the hero type indicates the page is
        // being used as a microsite, indicate as such to the template.
        if ($variables['hero_type'] == 'microsite_hero') {
          $variables['is_microsite'] = TRUE;
        }
      }
      else {
        $variables['hero_type'] = 'none';
      }
    }
  }
}

/**
* Custom function to add page regions to templates.
*/
function _add_regions_to_template($allowed_regions, &$variables) {
  // Get active theme.
  $theme = \Drupal::theme()->getActiveTheme()->getName();

  // Get current theme regions.
  $available_regions = system_region_list($theme, 'REGIONS_ALL');

  // Validate allowed regions with available regions.
  $regions = array_intersect(array_keys($available_regions), $allowed_regions);

  foreach ($regions as $key => $region) {
    // Load blocks from this region and sort them.
    $blocks = \Drupal::entityTypeManager()
      ->getStorage('block')
      ->loadByProperties(array('theme'  => $theme, 'region' => $region));
    uasort($blocks, 'Drupal\block\Entity\Block::sort');

    // Build blocks and assign to template variable.
    $build = array();
    $builder = \Drupal::entityTypeManager()->getViewBuilder('block');
    foreach ($blocks as $key => $block) {
      if ($block->access('view')) {
        $build[$key] = $builder->view($block, 'block');
      }
    }
    $variables['region_' . $region] = $build;
  }
}

/**
 * Implements hook_preprocess_node().
 */
function slac_preprocess_node__person(&$vars) {
  if ($vars['view_mode'] == 'full') {
    if (!empty($vars['content']['field_body'][0])) {
      $body_field = $vars['elements']['#node']->get('field_body');
      $vars['content']['field_body_trimmed'] = $body_field->view([
        'type' => 'text_trimmed',
        'label' => 'hidden',
        'settings' => [
          'trim_length' => 500
        ],
      ]);
    }
  }
}

/**
 * Implements hook_preprocess_node().
 */
function slac_preprocess_node_find_toc_markers($node, $field) {
  // Collector for list items to be created; this will be an ordered list.
  $list_items = [];

  // Extract the set of paragraphs from the content field of the node.
  $paragraphs = $node->get($field)->getValue();

  // If we found paragraphs in the specified container field, loop
  // through and find the TOC markers.
  if (count($paragraphs)) {
    foreach ($paragraphs as $item) {
      $paragraph = Paragraph::load($item['target_id']);
      if ($paragraph->getType() == 'table_of_contents') {
        $list_items[] = $paragraph->field_short_text->value;
      }
    }
  }

  return $list_items;
}

/**
 * _slac_preprocess_node_generate_toclist
 *
 * Helper function that converts an array of strings into an ordered list
 * of links. Each link is titled with a string.
 */
function slac_preprocess_node_generate_toc_list($items) {
  // Create a URL from a route fragment on the <current> page for theming an anchor link.
  // Store the link into container and return an array with keys aligned for theming using the
  // "toc" component. Make explicit the array indexing for later use if needed.
  // The URL fragment is created using the same function as used in the table_of_contents
  // paragraph display, see includes/paragraph.inc.
  $links = [];

  foreach ($items as $ix => $item) {
    $links[$ix] = [
      'title' => $item,
      'href' => Url::fromRoute('<current>', [], ['fragment' => Html::cleanCssIdentifier($item)])
        ->toString(),
    ];
  }

  return $links;
}

/**
 * Implements hook_preprocess_node__HOOK().
 */
function slac_preprocess_node__news_article(&$vars) {
  if ($vars['view_mode'] == 'card' || $vars['view_mode'] == 'teaser_card') {
    if (!empty($vars['content']['field_teaser'][0]) && $vars['content']['field_teaser']['#formatter'] == 'trimmed_first_wysiwyg_paragraph') {
      $is_truncated = $vars['elements']['field_teaser'][0]['#attributes']['is_truncated'];
    } elseif (!empty($vars['node']->field_paragraphs) && $vars['content']['field_paragraphs']['#formatter'] == 'trimmed_first_wysiwyg_paragraph') {
      $is_truncated = $vars['elements']['field_paragraphs'][0]['#attributes']['is_truncated'];
    }
    if (isset($is_truncated)) {
      $vars['is_truncated'] = $is_truncated;
    }
  }
  if ($vars['view_mode'] == 'full') {
    $node = $vars['elements']['#node'];
    if (!$node->field_text->value && $node->field_link->first()) {
      /** @var Url $url */
     $url = $node->field_link->first()->getUrl();
     if ($url->getRouteName() === 'entity.node.canonical') {
       $nid = $url->getRouteParameters()['node'];
       $node = Node::load($nid);
       if ($node->bundle() === 'person') {
         $vars['content']['field_text'] = $node->get('title')->view();
         $renderer = \Drupal::service('renderer');
         $renderer->addCacheableDependency($vars, $node);
       }
     }
    }
  }
}

/**
 * Implements hook_preprocess_node__HOOK().
 */
function slac_preprocess_node__blog(&$vars) {
  slac_preprocess_node__news_article($vars);
}
