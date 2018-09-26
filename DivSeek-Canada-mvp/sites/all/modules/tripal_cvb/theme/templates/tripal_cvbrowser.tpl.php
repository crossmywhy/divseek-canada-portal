<?php

/**
 * @file
 * Tripal CV Browser browser page.
 *
 * Displays the root part of a given CV browser and initializes its action
 * javascript array.
 *
 * @ingroup tripal_cvb
 */

drupal_add_css(drupal_get_path('module', 'tripal_cvb') . '/theme/css/tripal_cvb.css');
drupal_add_js(drupal_get_path('module', 'tripal_cvb') . '/theme/js/tripal_cvb.js');
if ($browser->show_relationships) {
  drupal_add_css(drupal_get_path('module', 'tripal_cvb') . '/theme/css/relationships.css');
}
?>

<div class="tripal-cvb tripal-cvb-browser<?php
  echo ($browser->machine_name ? ' tripal-cvb-browser-' . $browser->machine_name : '');
?>">
<?php
if (count($cv_terms)) {
  echo "<ul class=\"tripal-cvb\">\n";
  // Check if we should display CVs.
  if ($browser->show_cv) {
    // Display each CV.
    foreach ($cv_terms as $cv_id => $terms) {
      echo
        '<li class="tripal-cvb tripal-cvb-root tripal-cvb-has-children tripal-cvb-expanded tripal-cvb-action-processed"><span class="tripal-cvb-cv tripal-cvb-cvid-'
        . $cv_id
        . '">'
        . current($terms)->cv
        . '</span>';
      echo "<ul class=\"tripal-cvb\">\n";
      // Displays first level nodes.
      foreach ($terms as $term) {
        if (0 < $term->children_count) {
          echo
            '    <li class="tripal-cvb tripal-cvb-has-children tripal-cvb-collapsed"><span class="tripal-cvb-cvterm tripal-cvb-cvtermid-'
            . $term->cvterm_id
            . '" title="('
            . $term->cv
            . ') '
            . str_replace('"', "'", $term->definition)
            . '">'
            . $term->name
            . '</span>';
        }
        else {
          echo
            '    <li class="tripal-cvb tripal-cvb-root tripal-cvb-leaf"><span class="tripal-cvb-cvterm tripal-cvb-cvtermid-'
            . $term->cvterm_id
            . '">'
            . $term->name
            . '</span>';
        }
        echo "</li>\n";
      }
      echo "</ul></li>\n";
    }
  }
  else {
    // Just displays first level nodes.
    foreach ($cv_terms as $cv_id => $terms) {
      foreach ($terms as $term) {
        if (0 < $term->children_count) {
          echo
            '    <li class="tripal-cvb tripal-cvb-root tripal-cvb-has-children tripal-cvb-collapsed"><span class="tripal-cvb-cvterm tripal-cvb-cvtermid-'
            . $term->cvterm_id
            . '" title="('
            . $term->cv
            . ') '
            . str_replace('"', "'", $term->definition)
            . '">'
            . $term->name
            . '</span>';
        }
        else {
          echo
            '    <li class="tripal-cvb tripal-cvb-root tripal-cvb-leaf"><span class="tripal-cvb-cvterm tripal-cvb-cvtermid-'
            . $term->cvterm_id
            . '">'
            . $term->name
            . '</span>';
        }
        echo "</li>\n";
      }
    }
  }
  echo "</ul>\n";
}
else {
  echo t("<span>No term found!</span>");
}
?>
  <script type="text/javascript">
    var tripal_cvb_<?php
      echo preg_replace('/\W/', '_', $browser->machine_name);
    ?> =
<?php
    echo json_encode($actions) . ";\n";

    // Adds cvterm data.
    foreach ($cv_terms as $cv_id => $terms) {
      foreach ($terms as $term) {
        echo
          "    jQuery('.tripal-cvb-cvtermid-"
          . $term->cvterm_id
          . "').data('cvterm',\n        "
          . json_encode($term)
          . "\n      );\n";
      }
    }
?>
  </script>
</div>
