<?php

/**
 * @file
 * Displays a list of available CV browsers.
 *
 * @ingroup tripal_cvb
 */

$cvb_table = $variables['cvb_table'];
$pager = $variables['pager'];
if ($cvb_table) {
?>
<div class="tripal_cvb-data-block-desc tripal-data-block-desc">
  This is the list of available CV browsers.
</div>
<br/>
<?php
  print $cvb_table;
  print $pager;
}
else {
?>
  <div class="tripal_cvb-message">
    No CV browser found! Would you like to <?php
    echo l(t('create a new CV browser'), 'tripal_cvb/add'); ?>?<br/>
    <br/>
  </div>
<?php
}
?>
