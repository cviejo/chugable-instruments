CHUGINS=clds cldsp rngs

current:
	@echo "[chugins build]: please use one of the following configurations:"
	@echo "   make linux or make mac"

$(MAKECMDGOALS): $(CHUGINS)
$(CHUGINS):
	make -C src/$(basename $@) $(MAKECMDGOALS)
