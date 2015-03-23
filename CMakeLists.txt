set ( SOURCE
	"src/socket_client.cpp"
	"src/socket_server.cpp"
	"src/socket_connector.cpp"
	"src/receiver.cpp"
)
set ( HEADERS
        "include/socket_connection/socket_client.h"
        "include/socket_connection/socket_server.h"
	"include/socket_connection/socket_connector.h"
	"include/socket_connection/receiver.h"
	"include/socket_connection/socket_listener.h"
)

add_library ( socket_connection SHARED ${SOURCE} ${HEADERS})
