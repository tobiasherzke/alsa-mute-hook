LDLIBS += -lasound
all: alsa-mute-hook
install: all
	install -d $(DESTDIR)/bin
	install alsa-mute-hook turn-speakers-on.sh turn-speakers-off.sh $(DESTDIR)/bin/
clean:
	rm -f alsa-mute-hook
