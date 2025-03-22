g++ pnode-server.cpp -o pnode-server.bin \
-I ../../lib/PObject/ ../../lib/PObject/*.cpp \
-I ../../lib/Utils/ ../../lib/Utils/pid_controller.c \
-I ../../lib/PUtils/ ../../lib/PUtils/PUtils.cpp \
-I ../../lib/PMessage/ ../../lib/PMessage/PMessage.cpp \
-I ../../lib/PMessage/platform1/ ../../lib/PMessage/platform1/platform1.pb.c \
-I ../../lib/PMessage/nanopb-0.4.9.1-linux-x86/ ../../lib/PMessage/nanopb-0.4.9.1-linux-x86/*.c \
-lenet -lnanomsg

g++ pnode-cli.cpp -o pnode-cli.bin \
-I ../../lib/PObject/ ../../lib/PObject/*.cpp \
-I ../../lib/Utils/ ../../lib/Utils/pid_controller.c \
-I ../../lib/PUtils/ ../../lib/PUtils/PUtils.cpp \
-I ../../lib/PMessage/ ../../lib/PMessage/PMessage.cpp \
-I ../../lib/PMessage/platform1/ ../../lib/PMessage/platform1/platform1.pb.c \
-I ../../lib/PMessage/nanopb-0.4.9.1-linux-x86/ ../../lib/PMessage/nanopb-0.4.9.1-linux-x86/*.c \
-lenet -lnanomsg
