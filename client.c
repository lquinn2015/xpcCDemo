#import <stdio.h>
#import "xpc/xpc.h" // this should be updated or just extern every xpc call and the linker will resolve later
// see https://github.com/googleprojectzero/p0tools/tree/master/iOSOnMac


typedef enum : uint64_t {
    XPCRouteUnknown,
    XPCRouteFoo,
    XPCRouteBar,
} XPCRoute;

void XPCConnect(void);
void XPCDisconnect(void);

void foo(void);
int64_t bar(int64_t num1, int64_t *outNum2);

xpc_connection_t XPCConnection;

void XPCConnect() {
    xpc_connection_t connection = xpc_connection_create_mach_service("com.johnappleseed.ExampleDaemon", NULL, XPC_CONNECTION_MACH_SERVICE_PRIVILEGED);
    
    xpc_connection_set_event_handler(connection, ^(xpc_object_t event) {
        return;
    });
    
    xpc_connection_resume(connection);
    XPCConnection = connection;
}

void XPCDisconnect() {
    xpc_connection_cancel(XPCConnection);
    
    xpc_release(XPCConnection);
    XPCConnection = NULL;
}

void foo() {
    xpc_object_t msg = xpc_dictionary_create(NULL, NULL, 0);
    xpc_dictionary_set_uint64(msg, "route", XPCRouteFoo);
    
    __unused xpc_object_t response = xpc_connection_send_message_with_reply_sync(XPCConnection, msg);
}

int64_t bar(int64_t num1, int64_t *outNum2) {
    xpc_object_t msg = xpc_dictionary_create(NULL, NULL, 0);
    xpc_dictionary_set_uint64(msg, "route", XPCRouteBar);
    
    xpc_dictionary_set_int64(msg, "num1", num1);
    
    xpc_object_t response = xpc_connection_send_message_with_reply_sync(XPCConnection, msg);
    
    int64_t status = xpc_dictionary_get_int64(response, "status");
    int64_t num2 = xpc_dictionary_get_int64(response, "num2");
    
    *outNum2 = num2;
    return status;
}

int main(int argc, const char *argv[]) {
    XPCConnect();
    
    printf("[foo] sending request to daemon...\n");
    foo();
    printf("[foo] received response from daemon\n");
    
    printf("[bar] sending request to daemon...\n");
    int64_t num1 = 41;
    int64_t num2;
    int64_t status = bar(num1, &num2);
    printf("[bar] received response from daemon. status is %lld and num2 is %lld\n", status, num2);
    
    XPCDisconnect();
    
    return 0;
}
