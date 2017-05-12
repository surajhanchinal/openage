// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_CONTEXT_H_
#define OPENAGE_RENDERER_CONTEXT_H_

#include <memory>
#include <SDL2/SDL.h>

#include "context_state.h"
#include "../coord/window.h"

namespace openage {
namespace renderer {

class Buffer;
class ProgramSource;
class Program;
class Texture;
class TextureData;
class VertexState;


/**
 * Available context types.
 * Specifies all available backends.
 * autodetect: use vulkan > opengl. could use some tuning :D
 */
enum class context_type {
	autodetect,
	vulkan,
	opengl,
};



/**
 * Represents the rendering context.
 * This class provides functionality that is dispatched according to the
 * used backend.
 *
 * GPUs are state machines, this context stores/manages the state for
 * the requested backend driver.
 * This represents, for example: OpenGL, Vulkan, OpenGLES, WebGL, ...
 */
class Context {
public:
	Context(context_type type);
	virtual ~Context() = default;

	/**
	 * Create a context according to the requested type.
	 */
	static std::unique_ptr<Context> generate(context_type t);

	/**
	 * Called before the drawing window is created.
	 */
	virtual void prepare() = 0;

	/**
	 * Returns the SDL2 window flags for the requested context type.
	 */
	virtual uint32_t get_window_flags() const = 0;

	/**
	 * Actually creates the requested context for the given SDL window.
	 */
	virtual void create(SDL_Window *window) = 0;

	/**
	 * Setup calls for the newly created context.
	 * After creation, the context may want to have some setup
	 * calls that configure functionality.
	 */
	virtual void setup() = 0;

	/**
	 * Destroy the context to unregister it.
	 */
	virtual void destroy() = 0;

	/**
	 * Return the context properties such as max texture units,
	 * vertex attributes, etc.
	 */
	virtual context_capability get_capabilities() = 0;

	/**
	 * Enable or disable a given context feature.
	 * @param feature the feature to modify
	 * @param on whether to set this feature on or off.
	 */
	virtual void set_feature(context_feature feature, bool on) = 0;

	/**
	 * Save this context's framebuffer as a png screenshot.
	 */
	virtual void screenshot(const std::string &filename) = 0;

	/**
	 * Perform error checking to detect backend context problems.
	 */
	virtual void check_error() = 0;

	/**
	 * Register some texture data to the context.
	 * @returns the newly created Texture handle.
	 */
	virtual std::unique_ptr<Texture> register_texture(const TextureData &data) = 0;

	/**
	 * Register some shader pipeline program to the context.
	 * @returns the newly created Program handle.
	 */
	virtual std::unique_ptr<Program> register_program(const ProgramSource &data) = 0;

	/**
	 * Create a buffer handle
	 * @returns the newly created buffer state.
	 */
	virtual std::unique_ptr<Buffer> create_buffer(size_t size=0) = 0;

	/**
	 * Create a vertex state tracker
	 */
	virtual std::unique_ptr<VertexState> create_vertex_state() = 0;

	/**
	 * Resize the context because the surrounding window size was updated.
	 */
	void resize(const coord::window &new_size);

	/**
	 * Context state tracking. Contains state that is common
	 * to all actual context backends.
	 */
	ContextState state;

protected:
	/**
	 * Perform context-specific calls to resize the drawing canvas.
	 */
	virtual void resize_canvas(const coord::window &new_size) = 0;

	/**
	 * Type of this context, namely the backend variant.
	 */
	context_type type;

	/**
	 * Render surface size.
	 */
	coord::window canvas_size;
};

}} // namespace openage::renderer

#endif