#include "rasterizer_renderer.h"

#include "resource.h"
#include "utils/resource_utils.h"
#include "utils/timer.h"
#include <memory>


void cg::renderer::rasterization_renderer::init() {
	rasterizer = std::make_shared<cg::renderer::rasterizer<cg::vertex, cg::unsigned_color>>();
	rasterizer->set_viewport(settings->width, settings->height);
	render_target =
		std::make_shared<cg::resource<cg::unsigned_color>>(
			settings->width,
			settings->height
		);
	rasterizer->set_render_target(render_target);
	// TODO Lab: 1.03 Adjust `cg::renderer::rasterization_renderer` and `cg::renderer::renderer` classes to consume `cg::world::model`
	// TODO Lab: 1.04 Setup an instance of camera `cg::world::camera` class in `cg::renderer::renderer` and `cg::renderer::rasterization_renderer`
	// TODO Lab: 1.06 Add depth buffer in `cg::renderer::rasterization_renderer`
}
void cg::renderer::rasterization_renderer::render() {
	rasterizer->clear_render_target(cg::unsigned_color{.r = 32, .g = 32, .b = 32});
	cg::utils::save_resource(*render_target, settings->result_path);

	// TODO Lab: 1.04 Implement `vertex_shader` lambda for the instance of `cg::renderer::rasterizer`
	// TODO Lab: 1.05 Implement `pixel_shader` lambda for the instance of `cg::renderer::rasterizer`
	// TODO Lab: 1.03 Adjust `cg::renderer::rasterization_renderer` and `cg::renderer::renderer` classes to consume `cg::world::model`
}

void cg::renderer::rasterization_renderer::destroy() {}

void cg::renderer::rasterization_renderer::update() {}
