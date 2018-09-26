Tripal CV Browser Extension Module
==================================

CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Requirements
 * Installation
 * Configuration
 * Actions
 * Q & A
 * Maintainers


INTRODUCTION
------------

This module can be used to display and browse a hierarchical tree representation
of any controlled vocabulary (CV) stored into Chado (cvterm table). The
vocabulary to browse must have relationships stored into the cvterm_relationship
table where the children term is the subject (subject_id) and the parent term is
the object (object_id). Circular relationships and network relationships are not
an issue since the tree is not generated nor loaded when the CV browser page is
accessed; Only root elements are displayed and each first level of a subtree is
loaded (ajax) when the used click on the parent node.

A generic CV browser page can be used to browse any CV from CV root term(s) or
from user-selected terms. The URL of the page can be constructed this way:
'tripal/cvbrowser/' + <either 'cv' or 'cvterm', depending on the type of
identifiers provided after> + <CV or CV term identifier(s) or name(s) (separated
by "+" signs if more than on value is provided)>.
Examples:
 - tripal/cvbrowser/cv/biological_process+molecular_function+cellular_component
 - tripal/cvbrowser/cvterm/1234


REQUIREMENTS
------------

This module requires the following:

 * Tripal 7.x-2.x (not tested under 3.x) (http://www.drupal.org/project/tripal)
 
 * jQuery >= 1.7 (use jQuery update module)


INSTALLATION
------------

 * Install as you would normally install a contributed Drupal module. See:
   https://drupal.org/documentation/install/modules-themes/modules-7
   for further information.

 * Enable the module in "Admin menu > Site building > Modules" (/admin/modules).


CONFIGURATION
-------------

 * Configure the CV browser in "Administration > Tripal > Extensions >
   Tripal CV Browser > Settings" (/admin/tripal/extension/tripal_cvb).

 * Configure user permissions in "Administration > People > Permissions"
   (/admin/people/permissions):

   - "Use Tripal CV browser page": allows users to access to the generic CV
     browser page. With this permission, any CV or CV term can be browsed as the
     user only needs to know the CV name or CV cv_id or CV term name or CV term
     cvterm_id in order to use it in the browser page URL. If you want to
     restrict CV browsing, then do not give this permission to your users.
     Also note that the generic browser requires "Access CV term details as
     json" permission (explained below) as well to work properly.

   - "Access CV term details as json": allows users to access to CV term details
     as JSON data. Users would not access to these data directly but through
     AJAX call of the CV browsers. With this permission, any CV term can be
     retrieved as the user only needs to know the CV term identifier cvterm_id
     in order to fetch its details. While this permission virtually gives access
     to any CV term, it is required by the CV browsers (generic or in blocks) in
     order to work properly.

   - "Administer Tripal CV Browser": allows users to access to the
     CV browser administration pages and see the administration help page.

 * After creating custom CV browser(s), place them on your site using the block
   configuration page in "Administration > Structure > Blocks"
   (admin/structure/block). You can also configure access permission and block
   visibility for each block using their "configuration" link.
   If you prefer to have your browsers displayed as a page instead of a block,
   you can use the Drupal path 'tripal/cvbrowser/browser/' + the CV browser
   machine name. In this case, access are managed using the "Use tripal cv
   browser page" permission.


ACTIONS
-------

Actions are operations that can be performed automatically or by a user click
on a term of a CV browser. There are several types of actions:

 - View: it uses a display of a Drupal view to render something given a
   "cvterm_id" as parameter. The "use Ajax" setting of the view display must be
   turned on.

 - Path: it uses a Drupal path and append a given "cvterm_id" to it as parameter
   in order to display the associated page content if no other pattern has been
   specified in the path. Otherwise, the "%" character will be replaced by the
   "cvterm_id". It is also possible to use the following replacement patterns:
   - "!cvterm_id": will be replaced by the Chado "cvterm_id";
     ex.: "41"
   - "!cvterm": will be replaced by the CV term name;
     ex.: "organelle inheritance"
   - "!dbxref_id": will be replaced by the Chado "dbxref_id";
     ex.: "46"
   - "!accession": will be replaced by the CV term accession (identfier) on the
     database it comes from;
     ex.: "0048308"
   - "!cv": will be replaced by the CV name;
     ex.: "biological_process"
   - "!cv_id": will be replaced by the Chado "cv_id";
     ex.: "15"
   - "!db": will be replaced by the database name the CV term comes from.
     ex.: "GO"
   ex.: "admin/tripal/chado/tripal_cv/cv/15/cvterm/edit" would become
        "admin/tripal/chado/tripal_cv/cv/15/cvterm/edit/41"
   and "admin/tripal/chado/tripal_cv/cv/!cv_id/cvterm/edit/!cvterm_id" would
        become "admin/tripal/chado/tripal_cv/cv/15/cvterm/edit/41"

 - External URL: it displays a link to an external site page. Several parameters
   can be replaced in that URL if you use the following placeholders:
   - "!cvterm": will be replaced by the CV term name;
     ex.: "organelle inheritance"
   - "!accession": will be replaced by the CV term accession (identfier) on the
     database it comes from;
     ex.: "0048308"
   - "!cv": will be replaced by the CV name;
     ex.: "biological_process"
   - "!db": will be replaced by the database name the CV term comes from.
     ex.: "GO"
   ex.: "https://www.google.com/search?q=!cvterm" would become
        "https://www.google.com/search?q=organelle+inheritance"

 - Javascript: it executes the given javascript function name given it a
   "cvterm" object as only argument. This object has the following properties:
   - "name": the CV term name;
     ex.: "organelle inheritance"
   - "cv": the CV term CV name;
     ex.: "biological_process"
   - "definition": the CV term definition;
     ex.: "The partitioning of organelles between daughter cells at cell
           division. [GOC:jid]"
   - "db": the CV term database of origin name;
     ex.: "GO"
   - "urlprefix": the CV term database URL prefix;
     ex.: "http://amigo.geneontology.org/amigo/term/GO:"
   - "dbxref": the CV term database accession;
     ex.: "0048308"
   - "is_obsolete": the CV term "obsolete" status (boolean "0" or "1");
     ex.: "0"
   - "is_relationshiptype": the CV term "relationship type" status (boolean "0"
     or "1");
     ex.: "0"
   - "relationship": the name of the relationship between the CV term and its
     parent term;
     ex.: "is_a"
   - "children_count": the number of children of the CV term;
     ex.: "7"
   - "cvterm_id": Chado cvterm_id;
     ex.: "41"
   - "cv_id": Chado cv_id;
     ex.: "15"
   - "dbxref_id": Chado dbxref_id.
     ex.: "46"
   ex.: "Drupal.tripal_cvb.cvtermDump" (this is a function provided by this
     module) which prototype is ("Drupal" is a predefinied variable):
     "Drupal.tripal_cvb.cvtermDump = function (cvterm) { ... };"

Action field (ie. action to use) content depends on the type of action. For
"View", it must contain "the view machine name" + colon + "the display id" but
the user interface separate these parameters into 2 dedicated fields for
convenience. For "Path", it must be the Drupal path with or without ending
slash. For "External URL", it must be the full URL (replacement patterns were
described in previous paragraph). And finally, for "Javascript", it must be the
function name without parenthesis or any other piece of code.

"Link label" is the label of the action that is displayed before the action has
been executed.

The Auto-run checkbox setup if the action should be executed when the page is
loaded or when the user clicks on the action link. This setting is ignored by
the "External URL" type of action.

The target defines where the output of the action should be displayed (setting
ignored by External URL and Javascript). There are 3 types of targets:

 - Term line: it will be displayed on the same line as the CV term on the CV
   browser tree.

 - Theme region: it will be displayed on a region of the current Drupal theme.

 - DOM object identifier: it will be displayed in the HTML element matching the
   given identifier (it is based on jQuery selectors). By default, it will be
   the CV term <span> element.

The target identifier is used to identify the corresponding target according to
its type: it is ignored for the Term line, it is the region machine name for
the Theme region and it is a jQuery selector for the DOM object identifier.

The last setting is the insertion method: "Replace" will replace the content of
the corresponding target and "Append" will add the content to the given
target.
Note: the target settings are ignored for the "External URL" type of action.


Q & A
-----
Q: How can I remove the "[is a]" and "[part of]" prefix on my browser?
A: You have to override the CSS styles:
  .tripal-cvb-relationship-is_a >.tripal-cvb-cvterm:before and 
  .tripal-cvb-relationship-part_of >.tripal-cvb-cvterm:before

  For instance:
  .tripal-cvb-relationship-is_a >.tripal-cvb-cvterm:before,
  .tripal-cvb-relationship-part_of >.tripal-cvb-cvterm:before  {
    color: inherit;
    content: "" !important;
    padding-right: 0;
  }
  
Q: Can I customize the tree nodes according to their parental relationship?
A: Yes, use CSS class names of the form:
  "tripal-cvb-relationship-" + relationship name with groups of non-word
  characters replaced by a single "_".
  ex.: "part of" becomes "tripal-cvb-relationship-part_of".
       "weird 'relationship'" becomes
       "tripal-cvb-relationship-weird_relationship_".

Q: How can I hide actions that should not be performed on certain nodes?
A: You should add an action that is automatically launched that targets the CSS
  class of the term line. That action should generate the appropriate class name
  according to the term and the actions that can be performed on it. Then, using
  CSS, you can show or hide term actions. Each action has specific CSS class
  names: one giving its number in the list of term actions and one related to
  the action itself.


MAINTAINERS
-----------

Current maintainers:

 * Valentin Guignon (vguignon) - https://www.drupal.org/user/423148
 * Gaëtan Droc
