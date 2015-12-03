set ( SOURCE
	"src/socket_connection_handler.cpp"
	"src/socket_connector.cpp"
	"src/receiver.cpp"
)
set ( HEADERS
        "include/socket_connection/socket_connection_handler.h"
	"include/socket_connection/socket_connector.h"
	"include/socket_connection/receiver.h"
	"include/socket_connection/socket_listener.h"
)

add_library ( socket_connection SHARED ${SOURCE} ${HEADERS})
target_link_libraries( socket_connection PRIVATE lmscore )
