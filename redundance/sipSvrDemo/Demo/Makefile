include ../../project/linux/Makefile.global

SO = libsipsvr.so

INCLUDE_DIR += $(PROJECT_ROOT_DIR)public/include/hpr/
INCLUDE_DIR += $(PROJECT_ROOT_DIR)public/include/hlog/

LDLIBS += -Wl,-Bstatic $(LIB_DIR)libtinyxml.a -Wl,-Bdynamic -lhpr -lhlog -lsqlite3 -losipparser2 -lgsoap_server -lcmsws_sip_service -lsipws_service

.PHONY:clean
clean:
	-rm $(OBJS) -rf
	-rm *.so -rf
