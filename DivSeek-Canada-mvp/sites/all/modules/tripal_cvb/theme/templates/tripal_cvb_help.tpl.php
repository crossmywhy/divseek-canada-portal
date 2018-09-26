<?php

/**
 * @file
 * Tripal CV Browser help page.
 *
 * @ingroup tripal_cvb
 */
?>

<h3>About Tripal CV Browser</h3>
<p>
  Tripal CV Browser extension provides a tree browser that enable CV and
  ontology browsing.
</p>
<p>
  The browser base URL is &quot;<?php echo url('tripal/cvbrowser/'); ?>&quot;
  followed by either &quot;cv/&quot; and a Chado CV identifier (cv_id) or
  &quot;cvterm/&quot; and a Chado CV term identifier (cvterm_id).<br/>
  Example:
  <?php echo l(url('tripal/cvbrowser/cv/3'), 'tripal/cvbrowser/cv/3'); ?><br/>
  If you want to browse more than one CV or CV term subtree on the browser, you
  can specify several identifiers separated by the plus &quot;+&quot; sign. You
  can also use CV names or CV term names (case sensistive) instead of
  identifiers but for the later, you may have more than one CV term matching. To
  access this kind of browser, users must have the permission &quot;Use tripal
  CV browser page&quot;.
</p>
<p>
  It is also possible to define custom CV browsers with fixed CV or CV term
  identifiers and action links on CV terms. Such browser are accessible to
  everyone as Drupal blocks (browser access is managed on a block basis using
  block settings). These browsers can be
  <?php echo l(t('created'), 'tripal_cvb/add'); ?> and managed from the page
  <?php echo l(url('tripal_cvb'), 'tripal_cvb'); ?>. They can be also accessed
  through the browser base URL followed by &quot;browser/&quot; and the CV
  browser machine name. Example:
  &quot;<?php echo url('tripal/cvbrowser/browser/my-cv-browser'); ?>&quot; where
  &quot;my-cv-browser&quot; is the machine name of a created CV browser.
</p>
