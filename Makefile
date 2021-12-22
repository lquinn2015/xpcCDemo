
PLATFORM = iphoneos
ARCHS = arm64

SDK         =   $(PLATFORM)
SYSROOT     =   $(shell xcrun --sdk $(SDK) --show-sdk-path)
CLANG       :=  $(shell xcrun --sdk $(SDK) --find clang)
CODESIGN := $(shell which codesign)
CFLAGS = -fobjc-arc -fmodules  -fobjc-arc -fmodules
CC      =   $(CLANG) -isysroot $(SYSROOT) -arch $(ARCHS) $(CFLAGS)

OUT = build
CLIENT = client
CLIENT_SRC = client.c
SERVER = server
SERVER_SRC = server.c

deploy: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_SRC)
	$(CC) -o $(OUT)/$(CLIENT) $(CLIENT_SRC)
	$(CODESIGN) -s '-' --entitlements ent.xml -f --generate-entitlement-der $(OUT)/$(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) -o $(OUT)/$(SERVER) $(SERVER_SRC)
	$(CODESIGN) -s '-' --entitlements ent.xml -f --generate-entitlement-der $(OUT)/$(SERVER)
