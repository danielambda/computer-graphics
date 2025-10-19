#include "renderer.h"

#include "utils/error_handler.h"

#ifdef RASTERIZATION
#include "renderer/rasterizer/rasterizer_renderer.h"
#endif

#ifdef RAYTRACING
#include "renderer/raytracer/raytracer_renderer.h"
#endif

#ifdef DX12
#include "renderer/dx12/dx12_renderer.h"
#endif

using namespace cg::renderer;

cg::renderer::renderer::renderer(std::shared_ptr<cg::settings> settings) : settings{std::move(settings)} {}

unsigned cg::renderer::renderer::get_height() { return settings->height; }

unsigned cg::renderer::renderer::get_width() { return settings->width; }


std::shared_ptr<renderer> cg::renderer::make_renderer(std::shared_ptr<cg::settings> settings) {
#ifdef RASTERIZATION
	auto renderer = std::make_shared<cg::renderer::rasterization_renderer>(std::move(settings));
	return renderer;
#endif

#ifdef RAYTRACING
	auto renderer = std::make_shared<cg::renderer::ray_tracing_renderer>(std::move(settings));
	return renderer;
#endif

#ifdef DX12
	auto renderer = std::make_shared<cg::renderer::dx12_renderer>(std::move(settings));
	return renderer;
#endif

	THROW_ERROR("Type of renderer is not selected");
}

void cg::renderer::renderer::move_forward(float delta) {
	camera->set_position(
			camera->get_position() +
			camera->get_direction() * delta * frame_duration);
}

void cg::renderer::renderer::move_backward(float delta) {
	camera->set_position(
			camera->get_position() -
			camera->get_direction() * delta * frame_duration);
}

void cg::renderer::renderer::move_left(float delta) {
	camera->set_position(
			camera->get_position()
			- camera->get_right() * delta * frame_duration);
}

void cg::renderer::renderer::move_right(float delta) {
	camera->set_position(
			camera->get_position() +
			camera->get_right() * delta * frame_duration);
}

void cg::renderer::renderer::move_yaw(float delta) {
	camera->set_theta(camera->get_theta() + delta);
}

void cg::renderer::renderer::move_pitch(float delta) {
	camera->set_phi(camera->get_phi() + delta);
}

void cg::renderer::renderer::load_model() {
	model = std::make_shared<cg::world::model>();
	model->load_obj(settings->model_path);
}

void cg::renderer::renderer::load_camera() {
	camera = std::make_shared<cg::world::camera>();
	camera->set_height(static_cast<float>(settings->height));
	camera->set_width (static_cast<float>(settings->width));
	camera->set_position(float3{
		settings->camera_position[0],
		settings->camera_position[1],
		settings->camera_position[2]
	});
	camera->set_phi					 (settings->camera_phi);
	camera->set_theta				 (settings->camera_theta);
	camera->set_angle_of_view(settings->camera_angle_of_view);
	camera->set_z_near			 (settings->camera_z_near);
	camera->set_z_far				 (settings->camera_z_far);
}
