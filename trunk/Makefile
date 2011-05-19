SIGSEGV_DIR=libsigsegv-2.6
TECLA_DIR=libtecla
BUDDY_DIR=buddy-2.4
GMP_DIR=gmp-5.0.2
CVC3_DIR=
SMTA_DIR=smta-0.1
MAUDE_DIR=Maude-2.6
PREFIX=
MAUDE_BIN=


all: build

install: build
	cp $(MAUDE_DIR)/src/Main/maude $(MAUDE_BIN)

build:
	# libsigsegv
	make -C $(SIGSEGV_DIR) install
	# libtecla
	make -C $(TECLA_DIR) TARGETS=normal TARGET_LIBS=static install
	# buddy
	make -C $(BUDDY_DIR) install
	# gmp
	make -C $(GMP_DIR) install
	# cvc3
	make -C $(CVC3_DIR) install
	# smta
	#make -C $(SMTA_DIR) install
	# maude
	make -C $(MAUDE_DIR)

clean:
	make -C $(SIGSEGV_DIR) clean
	make -C $(TECLA_DIR) clean
	make -C $(BUDDY_DIR) clean
	make -C $(GMP_DIR) clean
	make -C $(CVC3_DIR) clean
	make -C $(SMTA_DIR) clean
	make -C $(MAUDE_DIR) clean
	rm -rf $(PREFIX)

