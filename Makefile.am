LANG=C
LC_ALL=c

AM_CFLAGS=-O2 -Wall -W -Wshadow -Wextra -fvisibility=hidden \
	  	$(PTHREAD_CFLAGS)
AM_LDFLAGS=-Wl,-O2,--no-undefined-version,--enable-new-dtags,--as-needed,--warn-common
AM_CC=$(PTHREAD_CC)
AM_CPPFLAGS=-I$(SRCDIR)/src/bin

LIBS+=$(PTHREAD_LIBS) 

dist_man_MANS = charn.1

doc_DIR=doc
doc_DATA=
doc_DATA+=README.md

dist_doc_DATA=
dist_doc_DATA+=COPYING charn.xhtml

bin_PROGRAMS=charn

charn_SOURCES = src/bin/charn.c src/bin/dbus.c src/bin/epoll.c src/bin/xcb.c

%.1: $(doc_DIR)/man/man1/%.xml $(doc_DIR)/@PACKAGE_NAME@.ent
	$(XSLTPROC) --path $(doc_DIR) -o $@ --nonet /usr/share/xml/docbook/stylesheet/docbook-xsl/manpages/docbook.xsl $<

%.xhtml: $(doc_DIR)/man/man1/%.xml $(doc_DIR)/@PACKAGE_NAME@.ent
	$(XSLTPROC) --path $(doc_DIR) -o $@ --nonet /usr/share/xml/docbook/stylesheet/docbook-xsl/xhtml-1_1/docbook.xsl $<

$(doc_DIR)/@PACKAGE_NAME@.ent:
	@mkdir -p $(@D)
	echo -e '<!ENTITY dhfirstname "Nick">\n<!ENTITY dhsurname "Black">\n<!ENTITY dhemail "nick.black@sprezzatech.com">\n<!ENTITY dhusername "&dhfirstname; &dhsurname;">\n<!ENTITY dhrelease "$(VERSION)">' > "$@"

clean-local:
	rm -rf $(MANS) $(doc_DIR)/@PACKAGE_NAME@.ent

uninstall-local:
	rmdir "$(DESTDIR)$(docdir)"
