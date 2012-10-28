; $Id $

DESCRIPTION
--------------------------
The Readability module will analyze your site content grading how difficult it is to read. It performs five popular automated readability tests that provide a grade level equivalency of a page's content:

<ul>
  <li>Flesch Kincaid</li>
  <li>Gunning Fog Score</li>
  <li>Coleman Liau Inde</li>
  <li>SMOG Index</li>
  <li>Automated Readability Index</li>
</ul>

It leverages the <a href=”http://drupal.org/project/contentanalysis”>Content Analysis API</a> to enable analysis of;
<ul>
  <li>nodes from the node edit form</li>
  <li>any page on your Drupal site from the content analysis block</li>
  <li>any page on the web from the content analysis admin page</li>
</ul>

The analysis is based on the <a href=”http://code.google.com/p/php-text-statistics”>php-text-statistics</a> project. Just download the object class TextStatistics.php and supporting files. Then place the files in a directory named “php-text-statistics”.  It is free, open source project.

INSTALLATION
---------------
* Upload the Readability directory to a modules directory on your server such as sites/all/modules or sites/default/modules.
* Download the php-text-statistics class at http://code.google.com/p/php-text-statistics. Create a directory called “php-text-statistics” under the Readability module directory and upload the php-text-statistic package file into it.
* Enable the module via the admin > site building > modules
* Optional: go to the Readability admin page customize module settings.
* Go to any page provides the Content Analysis form and click “Analyze content” to perform the tests


CREDITS
----------------------------
Authored and maintained by Tom McCracken <tom AT leveltendesign DOT com> twitter: @levelten_tom
Sponsored by LevelTen Interactive - http://www.leveltendesign.com