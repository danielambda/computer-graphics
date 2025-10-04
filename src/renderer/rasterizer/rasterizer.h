#pragma once

#include "resource.h"

#include <functional>
#include <iostream>
#include <linalg.h>
#include <limits>
#include <memory>


using namespace linalg::aliases;

static constexpr float DEFAULT_DEPTH = std::numeric_limits<float>::max();

namespace cg::renderer {
	template<typename VB, typename RT>
	class rasterizer {
	public:
		rasterizer(){};
		~rasterizer(){};
		void set_render_target(
				std::shared_ptr<resource<RT>> in_render_target,
				std::shared_ptr<resource<float>> in_depth_buffer = nullptr);
		void clear_render_target(
				const RT& in_clear_value, const float in_depth = DEFAULT_DEPTH);

		void set_vertex_buffer(std::shared_ptr<resource<VB>> in_vertex_buffer);
		void set_index_buffer(std::shared_ptr<resource<unsigned int>> in_index_buffer);

		void set_viewport(size_t in_width, size_t in_height);

		void draw(size_t num_vertexes, size_t vertex_offset);

		std::function<std::pair<float4, VB>(float4 vertex, VB vertex_data)> vertex_shader;
		std::function<cg::color(const VB& vertex_data, const float z)> pixel_shader;

	protected:
		std::shared_ptr<cg::resource<VB>> vertex_buffer;
		std::shared_ptr<cg::resource<unsigned int>> index_buffer;
		std::shared_ptr<cg::resource<RT>> render_target;
		std::shared_ptr<cg::resource<float>> depth_buffer;

		size_t width = 1920;
		size_t height = 1080;

		int edge_function(int2 a, int2 b, int2 c);
		bool depth_test(float z, size_t x, size_t y);
	};

	template<typename VB, typename RT>
	inline void rasterizer<VB, RT>::set_render_target(
			std::shared_ptr<resource<RT>> in_render_target,
			std::shared_ptr<resource<float>> in_depth_buffer
	) {
		if (in_render_target) {
			render_target = in_render_target;
	  }
		// TODO Lab: 1.06 Adjust `set_render_target`, and `clear_render_target` methods of `cg::renderer::rasterizer` class to consume a depth buffer
	}

	template<typename VB, typename RT>
	inline void rasterizer<VB, RT>::set_viewport(size_t in_width, size_t in_height) {
		width = in_width;
		height = in_height;
	}

	template<typename VB, typename RT>
	inline void rasterizer<VB, RT>::clear_render_target(
			const RT& in_clear_value, const float in_depth
	) {
		for (auto i = 0; i < render_target->count(); i++) {
			render_target->item(i) = in_clear_value;
	  }
		// TODO Lab: 1.06 Adjust `set_render_target`, and `clear_render_target` methods of `cg::renderer::rasterizer` class to consume a depth buffer
	}

	template<typename VB, typename RT>
	inline void rasterizer<VB, RT>::set_vertex_buffer(
		std::shared_ptr<resource<VB>> in_vertex_buffer
	) {
		vertex_buffer = in_vertex_buffer;
	}

	template<typename VB, typename RT>
	inline void rasterizer<VB, RT>::set_index_buffer(
			std::shared_ptr<resource<unsigned int>> in_index_buffer) {
		index_buffer = in_index_buffer;
	}

	template<typename VB, typename RT>
	inline void rasterizer<VB, RT>::draw(size_t num_vertexes, size_t vertex_offset) {
		size_t vertex_id = vertex_offset;

		while(vertex_id < vertex_offset + num_vertexes) {
			std::vector<VB> vertices(3);
			vertices[0] = vertex_buffer->item(index_buffer->item(vertex_id++));
			vertices[1] = vertex_buffer->item(index_buffer->item(vertex_id++));
			vertices[2] = vertex_buffer->item(index_buffer->item(vertex_id++));

			for(auto& vertex : vertices) {
				float4 coords{vertex.v.x, vertex.v.y, vertex.v.z, 1.f};
				auto processed = vertex_shader(coords, vertex);

				vertex.v.x = processed.first.x / processed.first.w;
				vertex.v.y = processed.first.y / processed.first.w;
				vertex.v.z = processed.first.z / processed.first.w;

				vertex.v.x = ( vertex.v.x + 1.f) * width  / 2.f;
				vertex.v.y = (-vertex.v.y + 1.f) * height / 2.f;
			}

			const int2 vertex_a(static_cast<int>(vertices[0].v.x),
										static_cast<int>(vertices[0].v.y));
			const int2 vertex_b(static_cast<int>(vertices[1].v.x),
										static_cast<int>(vertices[1].v.y));
			const int2 vertex_c(static_cast<int>(vertices[2].v.x),
										static_cast<int>(vertices[2].v.y));

			const int2 min_border(0, 0);
			const int2 max_border(width - 1, height - 1);

			const int2 min_aabb = clamp(min(vertex_a, min(vertex_b, vertex_c)),
																	min_border, max_border);
			const int2 max_aabb = clamp(max(vertex_a, max(vertex_b, vertex_c)),
																  min_border, max_border);

			for (int x = min_aabb.x; x <= max_aabb.x; x++)
			for (int y = min_aabb.y; y <= max_aabb.y; y++) {
				int2 point(x, y);
				int edge0 = edge_function(vertex_a, vertex_b, point);
				int edge1 = edge_function(vertex_b, vertex_c, point);
				int edge2 = edge_function(vertex_c, vertex_a, point);
				if (edge0 >= 0 && edge1 >= 0 && edge2 >= 0) {
					float depth = 1.f;
					auto result = pixel_shader(vertices[0], depth);
					render_target->item(x, y) = unsigned_color{0, 0, 0};// RT::from_color(result);
				}
			}
		}

		// TODO Lab: 1.06 Add `Depth test` stage to `draw` method of `cg::renderer::rasterizer`
	}

	template<typename VB, typename RT>
	inline int
	rasterizer<VB, RT>::edge_function(int2 a, int2 b, int2 c) {
		return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
	}

	template<typename VB, typename RT>
	inline bool rasterizer<VB, RT>::depth_test(float z, size_t x, size_t y) {
		if (!depth_buffer) {
			return true;
		}
		return depth_buffer->item(x, y) > z;
	}
}// namespace cg::renderer
