// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <eigen3/Eigen/Dense>

#include "../log/log.h"
#include "../error/error.h"
#include "resources/shader_source.h"
#include "opengl/renderer.h"
#include "vulkan/windowvk.h"
#include "vulkan/renderer.h"
#include "window.h"


namespace openage {
namespace renderer {
namespace tests {

void renderer_demo_0() {
	vulkan::VlkWindow window { "openage renderer testing" };
	window.make_context_current();
	{
		// Test Vulkan
		vulkan::VlkRenderer vk_render { window.get_instance(), window.get_surface() };
		vk_render.do_the_thing();
	}

	auto renderer = std::make_unique<opengl::GlRenderer>(window.get_context());

	auto vshader_src = resources::ShaderSource(
		resources::shader_source_t::glsl_vertex,
		R"s(
#version 330

layout(location=0) in vec2 position;
uniform mat4 mvp;

void main() {
/*
	gl_Position.x = 2.0 * float(gl_VertexID & 1) - 1.0;
	gl_Position.y = 2.0 * float((gl_VertexID & 2) >> 1) - 1.0;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;

	gl_Position = mvp * gl_Position;
*/
gl_Position = mvp * vec4(position, 0.0, 1.0);
}
)s");

	auto fshader_src = resources::ShaderSource(
		resources::shader_source_t::glsl_fragment,
		R"s(
#version 330

uniform vec4 color;
uniform uint u_id;

layout(location=0) out vec4 col;
layout(location=1) out uint id;

void main() {
	if (color.a == 0.0) {
		discard;
	}
	col = color;
	id = u_id + 1u;
}
)s");

	auto vshader_display_src = resources::ShaderSource(
		resources::shader_source_t::glsl_vertex,
		R"s(
#version 330


layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;
out vec2 v_uv;

void main() {
/*
	gl_Position.x = 2.0 * float(gl_VertexID & 1) - 1.0;
	gl_Position.y = 2.0 * float((gl_VertexID & 2) >> 1) - 1.0;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;

	v_uv = gl_Position.xy * 0.5 + 0.5;
*/
gl_Position = vec4(position, 0.0, 1.0);
v_uv = uv;
}
)s");


	auto fshader_display_src = resources::ShaderSource(
		resources::shader_source_t::glsl_fragment,
		R"s(
#version 330

uniform sampler2D color_texture;

in vec2 v_uv;
out vec4 col;

void main() {
	col = texture(color_texture, v_uv);
}
)s");

	auto shader = renderer->add_shader( { vshader_src, fshader_src } );
	auto shader_display = renderer->add_shader( { vshader_display_src, fshader_display_src } );

	auto transform1 = Eigen::Affine3f::Identity();
	transform1.prescale(Eigen::Vector3f(0.4f, 0.2f, 1.0f));
	transform1.prerotate(Eigen::AngleAxisf(30.0f * 3.14159f / 180.0f, Eigen::Vector3f::UnitX()));
	transform1.pretranslate(Eigen::Vector3f(-0.4f, -0.6f, 0.0f));

	auto unif_in1 = shader->new_uniform_input(
		"mvp", transform1.matrix(),
		"color", Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
		"u_id", 1u
	);

	auto transform2 = Eigen::Affine3f::Identity();
	transform2.prescale(Eigen::Vector3f(0.3f, 0.1f, 1.0f));
	transform2.prerotate(Eigen::AngleAxisf(50.0f * 3.14159f / 180.0f, Eigen::Vector3f::UnitZ()));

	auto transform3 = transform2;

	transform2.pretranslate(Eigen::Vector3f(0.3f, 0.1f, 0.3f));

	auto unif_in2 = shader->new_uniform_input(
		"mvp", transform2.matrix(),
		"color", Eigen::Vector4f(0.0f, 1.0f, 0.0f, 1.0f),
		"u_id", 2u
	);

	transform3.prerotate(Eigen::AngleAxisf(90.0f * 3.14159f / 180.0f, Eigen::Vector3f::UnitZ()));
	transform3.pretranslate(Eigen::Vector3f(0.3f, 0.1f, 0.5f));

	auto unif_in3 = shader->new_uniform_input(
		"mvp", transform3.matrix(),
		"color", Eigen::Vector4f(0.0f, 0.0f, 1.0f, 1.0f),
		"u_id", 3u
	);

	auto quad = renderer->add_mesh_geometry( resources::MeshData { resources::init_quad_t {} } );
	Renderable obj1 {
		unif_in1.get(),
		quad.get(),
		true,
		true,
	};

	Renderable obj2{
		unif_in2.get(),
		quad.get(),
		true,
		true,
	};

	Renderable obj3 {
		unif_in3.get(),
		quad.get(),
		true,
		true,
	};

	auto size = window.get_size();
	auto color_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::rgba8));
	auto id_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::r32ui));
	auto depth_texture = renderer->add_texture(resources::TextureInfo(size.x, size.y, resources::pixel_format::depth24));
	auto fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get(), depth_texture.get() } );

	auto color_texture_uniform = shader_display->new_uniform_input("color_texture", color_texture.get());
	Renderable display_obj {
		color_texture_uniform.get(),
		quad.get(),
		false,
		false,
	};

	RenderPass pass {
		{ obj1, obj2, obj3 },
		fbo.get()
	};

	RenderPass display_pass {
		{ display_obj },
		renderer->get_display_target(),
	};

	resources::TextureData id_texture_data = id_texture->into_data();
	bool texture_data_valid = false;

	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	// what is this
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	window.add_mouse_button_callback([&] (SDL_MouseButtonEvent const& ev) {
			auto x = ev.x;
			auto y = ev.y;

			log::log(INFO << "Clicked at location (" << x << ", " << y << ")");
			if (!texture_data_valid) {
				id_texture_data = id_texture->into_data();
				texture_data_valid = true;
			}
			auto id = id_texture_data.read_pixel<uint32_t>(x, y);
			log::log(INFO << "Id-texture-value at location: " << id);
			if (id == 0) {
				//no renderable at given location
				log::log(INFO << "Clicked at non existent object");
				return;
			}
			id--; //real id is id-1
			log::log(INFO << "Object number " << id << " clicked.");
		} );

	window.add_resize_callback([&] {
			auto new_size = window.get_size();

			texture_data_valid = false;
			// handle in renderer..
			glViewport(0, 0, new_size.x, new_size.y);

			// resize fbo
			color_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::rgba8));
			id_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::r32ui));
			depth_texture = renderer->add_texture(resources::TextureInfo(new_size.x, new_size.y, resources::pixel_format::depth24));
			fbo = renderer->create_texture_target( { color_texture.get(), id_texture.get(), depth_texture.get() } );

			shader_display->update_uniform_input(color_texture_uniform.get(), "color_texture", color_texture.get());
			pass.target = fbo.get();
		} );

	while (!window.should_close()) {
		renderer->render(pass);
		renderer->render(display_pass);
		window.update();
		window.get_context()->check_error();
	}
}

void renderer_demo(int demo_id) {
	switch (demo_id) {
	case 0:
		renderer_demo_0();
		break;

	default:
		log::log(MSG(err) << "unknown renderer demo " << demo_id << " requested.");
		break;
	}
}

}}} // namespace openage::renderer::tests
