CHUGINS=clds cldsp rngs onset-detector

current:
	@echo "[chugins build]: please use one of the following configurations:"
	@echo "   make linux, make mac, make osx, make clean, or make install"

$(MAKECMDGOALS): $(CHUGINS)
$(CHUGINS):
	make -C src/$(basename $@) $(MAKECMDGOALS)
