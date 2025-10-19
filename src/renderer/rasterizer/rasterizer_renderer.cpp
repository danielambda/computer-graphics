#include "rasterizer_renderer.h"

#include "resource.h"
#include "utils/resource_utils.h"
#include "utils/timer.h"
#include <memory>


void cg::renderer::rasterization_renderer::init() {
	load_model();
	load_camera();

	rasterizer = std::make_shared<cg::renderer::rasterizer<cg::vertex, cg::unsigned_color>>();
	rasterizer->set_viewport(settings->width, settings->height);
	render_target =
		std::make_shared<cg::resource<cg::unsigned_color>>(
			settings->width,
			settings->height
		);
	depth_buffer =
		std::make_shared<cg::resource<float>>(
			settings->width,
			settings->height
		);
	rasterizer->set_render_target(render_target, depth_buffer);
}

void cg::renderer::rasterization_renderer::render() {
	float4x4 matrix = mul(
		camera->get_projection_matrix(),
		camera->get_view_matrix(),
		model->get_world_matrix()
	);

	rasterizer->vertex_shader = [&](float4 vertex, cg::vertex vertex_data) {
		return std::make_pair(mul(matrix, vertex), vertex_data);
	};

	rasterizer->pixel_shader = [](cg::vertex data, float z){
		return cg::color::from_float3(data.ambient);
	};

	rasterizer->clear_render_target(cg::unsigned_color{48, 37, 60}); // #30253C

	for(size_t shape_id = 0; shape_id < model->get_index_buffers().size(); shape_id++) {
		rasterizer->set_vertex_buffer(model->get_vertex_buffers()[shape_id]);
		rasterizer->set_index_buffer (model->get_index_buffers() [shape_id]);
		rasterizer->draw             (model->get_index_buffers() [shape_id]->count(), 0);
	}

	cg::utils::save_resource(*render_target, settings->result_path);
}

void cg::renderer::rasterization_renderer::destroy() {}

void cg::renderer::rasterization_renderer::update() {}
