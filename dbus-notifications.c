#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dbus/dbus.h>

static char running = 0;

static void signal_handler_shutdown(int signal)
{
	(void)(signal);
	
	running = 0;
}

static void print_json_string(char *string)
{
	char *c = string;
	
	while(*c)
	{
		switch(*c)
		{
			case '"':
			{
				putchar('\\');
				putchar('"');
				break;
			}
			case '\\':
			{
				putchar('\\');
				putchar('\\');
				putchar('\\');
				putchar('\\');
				break;
			}
			case '\b':
			{
				putchar('\\');
				putchar('\\');
				putchar('b');
				break;
			}
			case '\f':
			{
				putchar('\\');
				putchar('\\');
				putchar('f');
				break;
			}
			case '\n':
			{
				putchar('\\');
				putchar('\\');
				putchar('n');
				break;
			}
			case '\r':
			{
				putchar('\\');
				putchar('\\');
				putchar('r');
				break;
			}
			case '\t':
			{
				putchar('\\');
				putchar('\\');
				putchar('t');
				break;
			}
			default:
			{
				putchar(*c);
				break;
			}
		}
		
		c++;
	}
}

static void cb_GetCapabilities(DBusConnection *connection, DBusMessage *message)
{
	DBusMessage *message_reply = NULL;
	DBusMessageIter arguments;
	const char *capabilities[] = { "body" };
	const char **capabilities_pointer = capabilities;
	size_t capabilities_amount = 1;
	
	message_reply = dbus_message_new_method_return(message);
	
	dbus_message_iter_init_append(message_reply, &arguments);
	if(!dbus_message_iter_append_fixed_array(&arguments, DBUS_TYPE_STRING, &capabilities_pointer, capabilities_amount))
	{
		fprintf(stderr, "Failed to append capabilities.\n");
		return;
	}
	
	if(!dbus_connection_send(connection, message_reply, NULL))
	{
		fprintf(stderr, "Failed to send capabilities.\n");
	}
	
	dbus_connection_flush(connection);
	
	dbus_message_unref(message_reply);
}

static void cb_Notify(DBusConnection *connection, DBusMessage *message)
{
	DBusMessage *message_reply = NULL;
	DBusMessageIter arguments;
	int current_type = DBUS_TYPE_INVALID;
	unsigned int argument_count = 0;
	char *notification_app_name = NULL;
	unsigned long int notification_replaces_id = 0;
	char *notification_summary = NULL;
	char *notification_body = NULL;
	long int notification_expire_timeout = 0;
	
	if(!dbus_message_iter_init(message, &arguments))
	{
		fprintf(stderr, "Failed to read notify arguments.\n");
		return;
	}
	
	while((current_type = dbus_message_iter_get_arg_type(&arguments)) != DBUS_TYPE_INVALID)
	{
		if(argument_count == 0 && current_type == DBUS_TYPE_STRING)
		{
			dbus_message_iter_get_basic(&arguments, &notification_app_name);
		}
		else if(argument_count == 1 && current_type == DBUS_TYPE_UINT32)
		{
			dbus_message_iter_get_basic(&arguments, &notification_replaces_id);
		}
		else if(argument_count == 3 && current_type == DBUS_TYPE_STRING)
		{
			dbus_message_iter_get_basic(&arguments, &notification_body);
		}
		else if(argument_count == 4 && current_type == DBUS_TYPE_STRING)
		{
			dbus_message_iter_get_basic(&arguments, &notification_summary);
		}
		else if(argument_count == 7 && current_type == DBUS_TYPE_INT32)
		{
			dbus_message_iter_get_basic(&arguments, &notification_expire_timeout);
		}
		
		argument_count++;
		dbus_message_iter_next(&arguments);
	}
	
	printf("{\"app_name\":\"");
	print_json_string(notification_app_name);
	printf("\",\"body\":\"");
	print_json_string(notification_body);
	printf("\",\"summary\":\"");
	print_json_string(notification_summary);
	printf("\",\"expire_timeout\":%li}\n", notification_expire_timeout);
	
	message_reply = dbus_message_new_method_return(message);
	
	dbus_message_iter_init_append(message_reply, &arguments);
	if(!dbus_message_iter_append_basic(&arguments, DBUS_TYPE_UINT32, &notification_replaces_id))
	{
		fprintf(stderr, "Failed to append notify.\n");
		return;
	}
	
	if(!dbus_connection_send(connection, message_reply, NULL))
	{
		fprintf(stderr, "Failed to send notify.\n");
	}
	
	dbus_connection_flush(connection);
	
	dbus_message_unref(message_reply);
}

static void cb_NotificationClosed(DBusConnection *connection, DBusMessage *message)
{
	DBusMessage *signal_reply = NULL;
	DBusMessageIter arguments;
	unsigned long int return_id = 0;
	unsigned long int return_reason = 3;
	
	if(!dbus_message_iter_init(message, &arguments))
	{
		fprintf(stderr, "Failed to read notify arguments.\n");
		return;
	}
	
	if(dbus_message_iter_get_arg_type(&arguments) == DBUS_TYPE_UINT32)
	{
		dbus_message_iter_get_basic(&arguments, &return_id);
	}
	
	signal_reply = dbus_message_new_signal("/org/freedesktop/Notifications", "org.freedesktop.Notifications", "NotificationClosed");
	if(signal_reply == NULL)
	{
		fprintf(stderr, "Failed to create signal.\n");
		return;
	}
	
	dbus_message_iter_init_append(signal_reply, &arguments);
	if(!dbus_message_iter_append_basic(&arguments, DBUS_TYPE_UINT32, &return_id))
	{
		fprintf(stderr, "Failed to append signal.\n");
		return;
	}
	
	if(!dbus_message_iter_append_basic(&arguments, DBUS_TYPE_UINT32, &return_reason))
	{
		fprintf(stderr, "Failed to append signal.\n");
		return;
	}
	
	if(!dbus_connection_send(connection, signal_reply, NULL))
	{
		fprintf(stderr, "Failed to send signal.\n");
	}
	
	dbus_connection_flush(connection);
	
	dbus_message_unref(signal_reply);
}

static void cb_GetServerInformation(DBusConnection *connection, DBusMessage *message)
{
	DBusMessage *message_reply = NULL;
	DBusMessageIter arguments;
	const char *return_name = "dwm-status-notification-server";
	const char *return_vendor = "freedesktop.org";
	const char *return_version = "1.0";
	const char *return_spec_version = "1.2";
	
	message_reply = dbus_message_new_method_return(message);
	
	dbus_message_iter_init_append(message_reply, &arguments);
	if(!dbus_message_iter_append_basic(&arguments, DBUS_TYPE_STRING, &return_name))
	{
		fprintf(stderr, "Failed to append server information.\n");
		return;
	}
	
	if(!dbus_message_iter_append_basic(&arguments, DBUS_TYPE_STRING, &return_vendor))
	{
		fprintf(stderr, "Failed to append server information.\n");
		return;
	}
	
	if(!dbus_message_iter_append_basic(&arguments, DBUS_TYPE_STRING, &return_version))
	{
		fprintf(stderr, "Failed to append server information.\n");
		return;
	}
	
	if(!dbus_message_iter_append_basic(&arguments, DBUS_TYPE_STRING, &return_spec_version))
	{
		fprintf(stderr, "Failed to append server information.\n");
		return;
	}
	
	if(!dbus_connection_send(connection, message_reply, NULL))
	{
		fprintf(stderr, "Failed to send server information.\n");
	}
	
	dbus_connection_flush(connection);
	
	dbus_message_unref(message_reply);
}

int main(void)
{
	int return_code = 0;
	DBusError error;
	DBusConnection *connection = NULL;
	DBusMessage *message = NULL;
	
	signal(SIGINT, signal_handler_shutdown);
	signal(SIGTERM, signal_handler_shutdown);
	
	dbus_error_init(&error);
	
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if(dbus_error_is_set(&error))
	{
		fprintf(stderr, "Connection Error: %s\n", error.message);
		dbus_error_free(&error);
		
		return -1;
	}
	if(connection == NULL)
	{
		fprintf(stderr, "Connection Error: unknown error\n");
		dbus_error_free(&error);
		
		return -1;
	}
	
	return_code = dbus_bus_request_name(connection, "org.freedesktop.Notifications", DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
	if(dbus_error_is_set(&error))
	{
		fprintf(stderr, "Connection Error: %s\n", error.message);
		dbus_error_free(&error);
		
		return -1;
	}
	if(DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != return_code)
	{
		return -1;
	}
	
	running = 1;
	while(running)
	{
		dbus_connection_read_write(connection, 1000);
		message = dbus_connection_pop_message(connection);
		
		if(NULL == message)
		{
			continue;
		}
		
		if(dbus_message_is_method_call(message, "org.freedesktop.Notifications", "GetCapabilities"))
		{
			cb_GetCapabilities(connection, message);
		}
		else if(dbus_message_is_method_call(message, "org.freedesktop.Notifications", "Notify"))
		{
			cb_Notify(connection, message);
		}
		else if(dbus_message_is_method_call(message, "org.freedesktop.Notifications", "CloseNotification"))
		{
			cb_NotificationClosed(connection, message);
		}
		else if(dbus_message_is_method_call(message, "org.freedesktop.Notifications", "GetServerInformation"))
		{
			cb_GetServerInformation(connection, message);
		}
		
		dbus_message_unref(message);
	}
	
	return 0;
}
