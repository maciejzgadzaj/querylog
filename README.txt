
SUMMARY
=======

The Database Query Log is a module targetted at Drupal developers
and inquisitive admins. Similarly to Devel module, it can save
database queries executed by Drupal to its own table, allowing
to browse and filter through them afterwards. It also can print
all executed queries at the bottom of each page.

What differentiates it from Devel is that it is much more
specialized. While Devel saves only overview of each query
(without values), this module saves complete queries, exactly
as they were executed. It also allows to filter queries to
save/discard based on calling function and/or path.

Additionally it provides Recent Database Queries report (similar
to Recent Log Entries), allowing to easily browse through all
saved queries and filter them based on various criteria.

Created mainly for development purposes, not recommended
to be used on production (especially high-traffic) sites.



INSTALLATION NOTES
==================

* Configure user permissions in Administration / User management /
  Permissions / querylog module

* Configure module settings in Administration / Site configuration /
  Logging and alerts / Database query log

  Please note that function- and page-specific filtering works in exactly
  the same way as block visibility settings.

  Also, it is advised that path 'admin/reports/querylog*' is excluded
  in Page specific visibility settings, otherwise browsing through
  Recent Database Entries list could get pretty difficult, as by default
  it is sorted by date in descending order, and each page load would add
  new set of queries to the top of the list.


AUTHOR/MAINTAINER
=================

maciej.zgadzaj AT gmail DOT com
