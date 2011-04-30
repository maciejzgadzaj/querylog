
SUMMARY
=======

The Database Query Log is a module targetted at Drupal developers
and inquisitive admins. It can save all/selected database queries
executed by Drupal to its own database table, allowing to browse
through them afterwards.

Not advised to be used on production sites.


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
