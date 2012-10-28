<?php

/**
 * Returns HTML for a textfield form element.
 * But with additional work for auto detect
 *
 * @param $variables
 *   An associative array containing:
 *   - element: An associative array containing the properties of the element.
 *     Properties used: #title, #value, #description, #size, #maxlength,
 *     #required, #attributes, #autocomplete_path.
 *
 * @ingroup themeable
 */
function theme_freetag_detect_field($vars) {
  $element = $vars['element'];
  $element['#attributes']['type'] = 'text';
  element_set_attributes($element, array('id', 'name', 'value', 'size', 'maxlength'));
  _form_set_class($element, array('form-text'));

  $extra = '';
  if ($element['#autocomplete_path'] && drupal_valid_path($element['#autocomplete_path'])) {
    drupal_add_library('system', 'drupal.autocomplete');
    $element['#attributes']['class'][] = 'form-autocomplete';

    $attributes = array();
    $attributes['type'] = 'hidden';
    $attributes['id'] = $element['#attributes']['id'] . '-autocomplete';
    $attributes['value'] = url($element['#autocomplete_path'], array('absolute' => TRUE));
    $attributes['disabled'] = 'disabled';
    $attributes['class'][] = 'autocomplete';
    $extra = '<input' . drupal_attributes($attributes) . ' />';
  }

  $output = '<input' . drupal_attributes($element['#attributes']) . ' />';

  // now the action button
  $form = array();
  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Scan for terms'),
    '#name' => $element['#attributes']['id'] . '-scan-button',
    '#id' => $element['#attributes']['id'] . '-scan-button',
    '#attributes' => array(
      'class' => array('freetag-detect-scan-button'),
    ),
  );

  $scan_extra = '<div class="clearfix form-wrapper">'.drupal_render($form).'</div>';

  return $output . $extra. $scan_extra;
}
