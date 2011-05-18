SIGSEGV_DIR=libsigsegv-2.6
GMP_DIR=gmp-5.0.2
TECLA_DIR=libtecla
BUDDY_DIR=buddy-2.4
CVC3_DIR=cvc3-2011-05-13
SMTA_DIR=smta-0.1
MAUDE_DIR=Maude-2.6
PREFIX=local


all: build

build:
	# libtecla
	make -C $(TECLA_DIR) TARGETS=normal TARGET_LIBS=static install
	# libsigsegv
	make -C $(SIGSEGV_DIR) install
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
	#make -C $(SMTA_DIR) clean
	make -C $(MAUDE_DIR) clean
	rm -rf $(PREFIX)

