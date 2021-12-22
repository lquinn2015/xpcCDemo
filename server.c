#import <CoreFoundation/CoreFoundation.h>
#import "xpc/xpc.h"

typedef enum : uint64_t {
    XPCRouteUnknown,
    XPCRouteFoo,
    XPCRouteBar,
} XPCRoute;

void XPCListen(void);
void XPCDelisten(void);
void XPCDispatch(xpc_object_t event);

void foo(void);
int64_t bar(int64_t num1, int64_t *outNum2);

xpc_connection_t XPCService;

void XPCListen() {
    xpc_connection_t service = xpc_connection_create_mach_service("com.johnappleseed.ExampleDaemon", NULL, XPC_CONNECTION_MACH_SERVICE_LISTENER);
    
    xpc_connection_set_event_handler(service, ^(xpc_object_t connection) {
        xpc_connection_set_event_handler(connection, ^(xpc_object_t event) {
            XPCDispatch(event);
        });
        xpc_connection_resume(connection);
    });
    
    xpc_connection_resume(service);
    XPCService = service;
}

void XPCDelisten() {
    xpc_connection_cancel(XPCService);
    XPCService = NULL;
}

void XPCDispatch(xpc_object_t event) {
    xpc_type_t type = xpc_get_type(event);
    if (type == XPC_TYPE_ERROR) {
        return;
    }
    
    uint64_t route = xpc_dictionary_get_uint64(event, "route");
    
    if (route == XPCRouteFoo) {
        xpc_connection_t remote = xpc_dictionary_get_remote_connection(event);
        
        foo();
        
        xpc_object_t reply = xpc_dictionary_create_reply(event);
        xpc_dictionary_set_int64(reply, "status", 0);
        
        xpc_connection_send_message(remote, reply);
    }
    else if (route == XPCRouteBar) {
        xpc_connection_t remote = xpc_dictionary_get_remote_connection(event);
        
        int64_t num1 = xpc_dictionary_get_int64(event, "num1");
        
        int64_t num2;
        int64_t status = bar(num1, &num2);
        
        xpc_object_t reply = xpc_dictionary_create_reply(event);
        xpc_dictionary_set_int64(reply, "status", status);
        xpc_dictionary_set_int64(reply, "num2", num2);
        
        xpc_connection_send_message(remote, reply);
    }
    else {
        xpc_connection_t remote = xpc_dictionary_get_remote_connection(event);
        
        xpc_object_t reply = xpc_dictionary_create_reply(event);
        xpc_dictionary_set_int64(reply, "status", 1);
        
        xpc_connection_send_message(remote, reply);
    }
}

void foo() {
    printf("[foo] processing request from client...\n");
}

int64_t bar(int64_t num1, int64_t *outNum2) {
    printf("[bar] processing request from client... num1 is %lld\n", num1);
    *outNum2 = 42;
    return 0;
}

int main(int argc, const char *argv[]) {
    XPCListen();
    CFRunLoopRun();
    return 0;
}
