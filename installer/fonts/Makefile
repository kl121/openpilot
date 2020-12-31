
.PHONY: all
all: install


install : fontinstalled.dummy


fontinstalled.dummy : script
	@touch fontinstalled.dummy

script :
	@sh -c /data/openpilot/installer/fonts/installer.sh
	@sh -c /data/openpilot/installer/fonts/installer2.sh




.PHONY: clean
clean:
	rm -f fontinstalled
