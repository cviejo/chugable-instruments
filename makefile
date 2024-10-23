DEST?=/usr/local/lib/chuck

CHUGINS=clds cldsp rngs

CHUGS=$(foreach CHUG,$(CHUGINS),src/$(CHUG)/$(CHUG).chug)

current:
	@echo "[chugins build]: please use one of the following configurations:"
	@echo "   make linux or make mac"

mac osx linux linux-alsa linux-jack: $(CHUGS)
$(CHUGS):
	make -C $(dir $@) $(MAKECMDGOALS)

clean install: $(CHUGINS)
$(CHUGINS):
	make -C src/$(basename $@) $(MAKECMDGOALS)
