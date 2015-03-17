# Copyright 2015-2015 the openage authors. See copying.md for legal info.

import os

from .pack import Pack
from .pack_config import PackConfig
from openage.log import dbg


class Bundle:
    """
    Data bundle for openage.

    This is a "mod pack" that feeds the engine with content.

    The media conversion basically creates such a pack and stores
    it to your disk.

    Mod pack layout:
    ./pack.cfg

    """

    # filename of the bundle root config
    rootcfg_filename = "pack.cfg"

    def __init__(self):
        pass

    def load(self, path):
        """
        Load the data bundle from a given path.
        """

        # test if it's a directory or archive file
        if os.path.isfile(path):
            dbg("loading bundle from %s..." % path)
            self.load_dir(path)
        elif os.path.isdir(path):
            raise NotImplementedError("archive loading not supported yet")
        else:
            raise FileNotFoundError("can't find '%s'" % path)

    def load_dir(self, path):
        rootcfg_path = os.path.join(path, self.rootcfg_filename)
        if not os.path.isfile(rootcfg_path):
            raise FileNotFoundError("root config file missing: %s" %
                                    rootcfg_path)

        with open(rootcfg_path, "r") as f:
            pcfg = PackConfig()
            pcfg.read(f)

        self.rootcfg = pcfg.get_attrs()
        self.pack = Pack(self.rootcfg)

    def save(self, path):
        """
        Save this bundle to the given filename

        The path is the foldername of the output dir.
        """

        raise NotImplementedError("saving bundles not yet implemented")
