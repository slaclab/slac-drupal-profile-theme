<?php

/**
 * @file
 * Views-related hook implementations.
 */

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Render\Markup;
use Drupal\Core\Url;

/**
 * Implements hook_preprocess_HOOK().
 */
function slac_preprocess_views_view(array &$variables) {
  $variables['path'] = $variables['view']->getRequest()->getPathInfo();
  $variables['is_dark'] = !empty($variables['view_array']['#is_dark'])
    ? is_array($variables['view_array']['#is_dark'])
      ? $variables['view_array']['#is_dark']['content']
      : $variables['view_array']['#is_dark']
    : ($variables['id'] == 'related_content_for_detail_pages' && $variables['display_id'] == 'news_article_related_stories') ||
      ($variables['id'] == 'related_content_for_media_pages' && $variables['display_id'] == 'related_images_videos');
  if (!empty($variables['rows']) && !empty($variables['rows'][0])) {
    $variables['rows'][0]['#rows']['#is_dark'] = $variables['is_dark'];
  }
}

/**
 * Implements hook_preprocess_views_view_unformatted().
 */
function slac_preprocess_views_view_unformatted(array &$variables) {
  if (!empty($variables['rows']['#is_dark'])) {
    $variables['is_dark'] = is_array($variables['rows']['#is_dark'])
      ? $variables['rows']['#is_dark']['content']
      : $variables['rows']['#is_dark'];
    unset($variables['rows']['#is_dark']);
  }
}

function slac_preprocess_views_slac_accordion_view(array &$variables) {
  slac_preprocess_views_view_unformatted($variables);
}

/**
 * Implements hook_theme_suggestions_HOOK_alter().
 */
function slac_theme_suggestions_views_view_alter(array &$suggestions, array $variables) {
  $view = $variables['view'];
  $suggestions[] = 'views_view__' . $view->id();
  $suggestions[] = 'views_view__' . $view->current_display;
  $suggestions[] = 'views_view__' . $view->id() . '__' . $view->current_display;
}

/**
 * Implements hook_theme_suggestions_HOOK_alter().
 */
function slac_theme_suggestions_views_view_unformatted_alter(array &$suggestions, array $variables) {
  $view = $variables['view'];
  $suggestions[] = 'views_view_unformatted__' . $view->id();
  $suggestions[] = 'views_view_unformatted__' . $view->current_display;
  $suggestions[] = 'views_view_unformatted__' . $view->id() . '__' . $view->current_display;
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function slac_form_views_exposed_form_alter(&$form, FormStateInterface $form_state, $form_id) {
  if (!empty($form['#id']) && $form['#id'] == 'views-exposed-form-landing-page-filtered-content-block-faqs-filtered-accordion-list') {
    $form['actions']['submit']['#is_faq'] = TRUE;
  }
}

/**
 * Implements hook_preprocess_views_view_field().
 */
function slac_preprocess_views_view_field(array &$variables) {
  // Alter search result output if an event has an external link.
  // We have to do this the messy way because we're using fields in the search
  // result view.
  if ($variables['view']->current_display == 'search_list') {
    if (isset($variables['row']->_entity) && $variables['row']->_entity->bundle() == 'event' && $variables['field']->field == 'title') {
      $node = $variables['row']->_entity;
      if ($node->field_boolean_2->value && $node->field_link->getValue()) {
        $current_link = $variables['output']->__toString();
        preg_match('/class="([^"]*)"/', $current_link, $match);
        $classes = $match[1] ?? [];
        $link_text = strip_tags($current_link);
        $url = Url::fromUri($node->field_link->getValue()[0]['uri'], ['attributes' => ['class' => $classes]]);
        $link = Link::fromTextAndUrl($link_text, $url);
        $variables['output'] = Markup::create($link->toString());
      }
    }
  }
}
