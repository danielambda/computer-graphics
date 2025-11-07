#include "raytracer_renderer.h"

#include "utils/resource_utils.h"
#include <random>

cg::renderer::ray_tracing_renderer::ray_tracing_renderer(
  std::shared_ptr<cg::settings> settings
) : renderer{std::move(settings)} {}

void cg::renderer::ray_tracing_renderer::init() {
  renderer::load_model();
  renderer::load_camera();

  //                position                , color
  lights.push_back({float3{0, 1.58f, -0.03f}, float3{0.78f, 0.78f, 0.78f}});
  lights.push_back({float3{0, 1.5f , 2.f   }, float3{0.25f, 0.25f, 0.25f}});

  render_target = std::make_shared<cg::resource<cg::unsigned_color>>(
    settings->width,
    settings->height
  );

  raytracer = std::make_shared<cg::renderer::raytracer<cg::vertex, cg::unsigned_color>>();
  raytracer->set_render_target(render_target);
  raytracer->set_viewport(
    settings->width,
    settings->height
  );
  raytracer->set_index_buffers( model->get_index_buffers());
  raytracer->set_vertex_buffers(model->get_vertex_buffers());
  raytracer->miss_shader = [](const ray& ray) {
    payload payload{};
    payload.color = {0, 0, 0};
    return payload;
  };

	std::mt19937 random(std::random_device{}());
	std::uniform_real_distribution<float> uniform(-1, 1);
  raytracer->closest_hit_shader = [&](const ray& ray, payload& payload, const triangle<vertex>& triangle, size_t depth) {
    float3 position = ray.position + payload.t * ray.direction;
    float3 normal = linalg::normalize
      ( payload.bary.x * triangle.na
      + payload.bary.y * triangle.nb
      + payload.bary.z * triangle.nc
      );

    float3 result_color = triangle.emissive;

    // { // Monte carlo
    //   float3 random_direction{uniform(random), uniform(random), uniform(random)};
    //   if (linalg::dot(normal, random_direction) < 0) {
    //     random_direction = -random_direction;
    //   }
    //   cg::renderer::ray to_next_object{position, random_direction};
    //   auto next_payload = raytracer->trace_ray(to_next_object, depth);
    //   result_color
    //     += triangle.diffuse
    //      * next_payload.color.to_float3()
    //      * std::max(linalg::dot(normal, to_next_object.direction), 0.f);
    // }

    { // Classic
      for (const light& light : lights) {
        cg::renderer::ray to_light{position, light.position - position};
        auto shadow_payload = shadow_raytracer->trace_ray(to_light, 1, linalg::length(light.position - position));
        if (shadow_payload.t < 0) {
          result_color
		        += triangle.diffuse
		         * light.color
		         * std::max(linalg::dot(normal, to_light.direction), 0.f);
        }
      }
    }

    payload.color = color::from_float3(result_color);
    return payload;
  };

  shadow_raytracer = std::make_shared<cg::renderer::raytracer<cg::vertex, cg::unsigned_color>>();
  shadow_raytracer->set_render_target(render_target);
  shadow_raytracer->set_viewport(
    settings->width,
    settings->height
  );
  shadow_raytracer->set_index_buffers( model->get_index_buffers());
  shadow_raytracer->set_vertex_buffers(model->get_vertex_buffers());
  shadow_raytracer->any_hit_shader = [](const ray&, payload& payload, const triangle<vertex>&) {
    return payload;
  };
  shadow_raytracer->miss_shader = [](const ray&) {
    payload payload{};
    payload.t = -1;
    return payload;
  };
}

void cg::renderer::ray_tracing_renderer::destroy() {}

void cg::renderer::ray_tracing_renderer::update() {}

void cg::renderer::ray_tracing_renderer::render() {
  raytracer->clear_render_target({0, 0, 0});
  raytracer->build_acceleration_structure();

  shadow_raytracer->build_acceleration_structure();

  raytracer->ray_generation(
    camera->get_position(),
    camera->get_direction(),
    camera->get_right(),
    camera->get_up(),
    settings->raytracing_depth,
    settings->accumulation_num
  );

  utils::save_resource(*render_target, settings->result_path);
}
