# dbus-notifications

This program creates a server which listens on the [DBus](https://wiki.freedesktop.org/www/Software/dbus/). It tracks notification messages and response to it according to the [specs](https://developer.gnome.org/notification-spec/). In addition all received notifications will be displayed in JSON ([RFC 4627](https://www.ietf.org/rfc/rfc4627.txt)) format (on `stdout`).

Actions, [Hints](https://developer.gnome.org/notification-spec/#hints), [Markup](https://developer.gnome.org/notification-spec/#markup) and [Icons](https://developer.gnome.org/notification-spec/#icons-and-images) are not supported.

## Output

    {"app_name":"notify-send","body":"Example body","summary":"Some text...","expire_timeout":1000}

* `app_name`: The optional name of the application sending the notification. Can be blank.
* `body`: The optional detailed body text. Can be empty.
* `summary`: The summary text briefly describing the notification.
* `expire_timeout`: The timeout time in milliseconds since the display of the notification at which the notification should automatically close.
If `-1`, the notification's expiration time is dependent on the notification server's settings, and may vary for the type of notification. If `0`, never expire.

## License

lastest GPL-license (see LICENSE-file)

Copyright (C) 2015 NIPE-SYSTEMS, [http://nipe-systems.de](http://nipe-systems.de)
