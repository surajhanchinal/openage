# Copyright 2015-2015 the openage authors. See copying.md for legal info.


from gi.repository import Gtk


def main():
    d = Gtk.MessageDialog(None, 0, Gtk.MessageType.INFO,
                          Gtk.ButtonsType.OK, 'openage mod manager')

    d.format_secondary_text('To be developed.');

    d.connect('delete-event', Gtk.main_quit)
    d.connect('response', Gtk.main_quit)

    d.show_all()
    Gtk.main()


if __name__ == "__main__":
    main()
