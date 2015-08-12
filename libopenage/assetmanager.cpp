// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "assetmanager.h"

#include "error/error.h"
#include "log/log.h"
#include "texture.h"
#include "util/compiler.h"
#include "util/file.h"
#include "watch/watch.h"

namespace openage {

AssetManager::AssetManager(qtsdl::GuiItemLink *gui_link)
	:
	missing_tex{nullptr},
	gui_link{gui_link} {

	this->watch_manager = watch::WatchManager::create();
}


const util::Path &AssetManager::get_asset_dir() {
	return this->asset_path;
}


void AssetManager::set_asset_dir(const util::Path& new_path) {
	if (this->asset_path != new_path) {
		this->asset_path = new_path;
		this->clear();
	}
}


void AssetManager::set_engine(Engine *engine) {
	this->engine = engine;
}


Engine *AssetManager::get_engine() const {
	return this->engine;
}


std::shared_ptr<Texture> AssetManager::load_texture(const std::string &name,
                                                    bool use_metafile,
                                                    bool null_if_missing) {

	// the texture to be associated with the given filename
	std::shared_ptr<Texture> tex;

	util::Path tex_path = this->asset_path[name];

	// try to open the texture filename.
	if (not tex_path.is_file()) {
		// TODO: add/fetch inotify watch on the containing folder
		// to display the tex as soon at it exists.

		if (null_if_missing) {
			return nullptr;
		}
		else {
			// return the big X texture instead
			tex = this->get_missing_tex();
		}
	}
	else {
		// create the texture!
		tex = std::make_shared<Texture>(tex_path, use_metafile);

		this->watch_manager->watch_file(
			filename,
			[=](watch::event_type, std::string) {
				tex->reload();
			}
		);
	}

	// pass back the shared_ptr<Texture>
	return tex;
}


Texture *AssetManager::get_texture(const std::string &name, bool use_metafile,
                                   bool null_if_missing) {
	// check whether the requested texture was loaded already
	auto tex_it = this->textures.find(name);

	// the texture was not loaded yet:
	if (tex_it == this->textures.end()) {
		auto tex = this->load_texture(name, use_metafile, null_if_missing);

		if (tex.get() != nullptr) {
			// insert the texture into the map
			this->textures.insert(std::make_pair(name, tex));
		}

		//  and return the texture pointer.
		return tex.get();
	}

	return tex_it->second.get();
}


void AssetManager::check_updates() {
	this->watch_manager->check_changes();
}

std::shared_ptr<Texture> AssetManager::get_missing_tex() {

	// if not loaded, fetch the "missing" texture (big red X).
	if (unlikely(this->missing_tex.get() == nullptr)) {
		this->missing_tex = std::make_shared<Texture>(
			this->asset_path["missing.png"],
			false
		);
	}

	return this->missing_tex;
}

void AssetManager::clear() {
#if WITH_INOTIFY
	for (auto& watch_fd : this->watch_fds) {
		int result = inotify_rm_watch(this->inotify_fd, watch_fd.first);
		if (result < 0) {
			log::log(WARN << "Failed to remove inotify watches");
		}
	}
	this->watch_fds.clear();
#endif

	this->textures.clear();
}

} // openage
