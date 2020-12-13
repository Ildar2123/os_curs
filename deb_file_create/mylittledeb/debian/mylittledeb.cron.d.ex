#
# Regular cron jobs for the mylittledeb package
#
0 4	* * *	root	[ -x /usr/bin/mylittledeb_maintenance ] && /usr/bin/mylittledeb_maintenance
