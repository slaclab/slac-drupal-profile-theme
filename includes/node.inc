<?php

/**
 * @file
 * Node-related hook implementations.
 */

use Drupal\Core\Url;
use Drupal\node\Entity\Node;
use Drupal\paragraphs\Entity\Paragraph;

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
    /** @var \Drupal\node\NodeInterface $node */
    $node = $variables['node'];

    // Determine the type of the hero paragraph if the field exists.
    if ($node->hasField('field_hero_paragraph')) {
      if($node->get('field_hero_paragraph')->first()) {
        $hero_field = $node->get('field_hero_paragraph')->first()->getValue();

        /** @var Paragraph $hero_paragraph */
        $hero_paragraph = Paragraph::load($hero_field['target_id']);
        $variables['hero_type'] = $hero_paragraph->getType();
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
