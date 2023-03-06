#pragma once

#include "noise.h"
#include "shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>

struct SamplePoint {
	glm::vec2 pos;
	float val;

	SamplePoint(float x, float y, float val) : pos(x, y), val(val) { }
};

class MarchingSquares {
	/* image resolution */
	unsigned int grid_spacing = 20;
	unsigned int window_width  = 800;
	unsigned int window_height = 800;
	unsigned int rows = window_height / grid_spacing;  // the number of rows/columns
	unsigned int cols = window_width / grid_spacing;   // of SQUARES, not points.
	float noise_width  = 5;
	float noise_height = 5;
	
	/* sample points */
	std::vector<SamplePoint> sample_points;
	unsigned int vao_sample_points, vbo_sample_points;

	/* isolines */
	typedef std::pair<const glm::vec2&, const glm::vec2&> Isoline;
	std::vector<Isoline> cells[16];

	const glm::vec2 cell_bottom{ 0.5 * grid_spacing, 0.0          };
	const glm::vec2 cell_top   { 0.5 * grid_spacing, grid_spacing };
	const glm::vec2 cell_left  { 0.0,          0.5 * grid_spacing };
	const glm::vec2 cell_right { grid_spacing, 0.5 * grid_spacing };
	
	std::vector<glm::vec2> isolines;  // stores the isolines to be drawn
	unsigned int vao_isolines, vbo_isolines;

	/* shaders */
	Shader shader_sample_points;
	Shader shader_isolines;

public:
	MarchingSquares() :
		shader_sample_points("src/shader/sample_points.vert", "src/shader/sample_points.frag"),
		shader_isolines("src/shader/isolines.vert", "src/shader/isolines.frag")
	{
		sample_points.reserve((rows + 1) * (cols + 1));

		setup_shaders();
		setup_vertex_objects();
		initialize_cells();
	}

	void setup_shaders()
	{
		shader_sample_points.use();
		shader_sample_points.setFloat("window_width", window_width);
		shader_sample_points.setFloat("window_height", window_height);

		shader_isolines.use();
		shader_isolines.setFloat("window_width", window_width);
		shader_isolines.setFloat("window_height", window_height);
	}

	void setup_vertex_objects()
	{
		// sample points

		glGenVertexArrays(1, &vao_sample_points);
		glGenBuffers(1, &vbo_sample_points);

		glBindVertexArray(vao_sample_points);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_sample_points);

		glVertexAttribPointer(0,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(SamplePoint),
							  reinterpret_cast<void*>(0));
		glVertexAttribPointer(1,
							  1,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(SamplePoint),
							  reinterpret_cast<void*>(sizeof(glm::vec2)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// isolines
		glGenVertexArrays(1, &vao_isolines);
		glGenBuffers(1, &vbo_isolines);

		glBindVertexArray(vao_isolines);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_isolines);

		glVertexAttribPointer(0,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(glm::vec2),
							  reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void initialize_cells()
	{
		cells[0] = std::vector<Isoline>{ };
		cells[1] = std::vector<Isoline>{ Isoline{ cell_left, cell_bottom } };
		cells[2] = std::vector<Isoline>{ Isoline{ cell_bottom, cell_right } };
		cells[3] = std::vector<Isoline>{ Isoline{ cell_left, cell_right } };
		cells[4] = std::vector<Isoline>{ Isoline{ cell_top, cell_right } };
		cells[5] = std::vector<Isoline>{ Isoline{ cell_left, cell_top }, Isoline{ cell_bottom, cell_right } };
		cells[6] = std::vector<Isoline>{ Isoline{ cell_top, cell_bottom } };
		cells[7] = std::vector<Isoline>{ Isoline{ cell_left, cell_top } };
		cells[8] = std::vector<Isoline>{ Isoline{ cell_left, cell_top } };
		cells[9] = std::vector<Isoline>{ Isoline{ cell_top, cell_bottom } };
		cells[10] = std::vector<Isoline>{ Isoline{ cell_left, cell_bottom }, Isoline{ cell_top, cell_right } };
		cells[11] = std::vector<Isoline>{ Isoline{ cell_top, cell_right } };
		cells[12] = std::vector<Isoline>{ Isoline{ cell_left, cell_right } };
		cells[13] = std::vector<Isoline>{ Isoline{ cell_bottom, cell_right } };
		cells[14] = std::vector<Isoline>{ Isoline{ cell_left, cell_bottom } };
		cells[15] = std::vector<Isoline>{ };
	}

	inline size_t get_cell_idx(float bl, float br, float tr, float tl)
	{
		return (bl > 0) * 1 + (br > 0) * 2 + (tr > 0) * 4 + (tl > 0) * 8;
	}

	float lerp_root(float x0, float f0, float x1, float f1)
	{
		return x0 - f0 * (x1 - x0) / (f1 - f0);
	}

	inline glm::vec2 lerp_root_x(const SamplePoint& left, const SamplePoint& right)
	{
		return glm::vec2(lerp_root(left.pos.x, left.val, right.pos.x, right.val), left.pos.y);
	}

	inline glm::vec2 lerp_root_y(const SamplePoint& top, const SamplePoint& bottom)
	{
		return glm::vec2(top.pos.x, lerp_root(top.pos.y, top.val, bottom.pos.y, bottom.val));
	}

	void compute_isolines(const SamplePoint& bl, const SamplePoint& br,
						  const SamplePoint& tr, const SamplePoint& tl)
	{
		size_t idx = get_cell_idx(bl.val, br.val, tr.val, tl.val);

		switch (idx) {
		case 1:
		case 14:
			isolines.emplace_back(lerp_root_y(tl, bl));
			isolines.emplace_back(lerp_root_x(bl, br));
			break;
		case 2:
		case 13:
			isolines.emplace_back(lerp_root_x(bl, br));
			isolines.emplace_back(lerp_root_y(br, tr));
			break;
		case 3:
		case 12:
			isolines.emplace_back(lerp_root_y(tl, bl));
			isolines.emplace_back(lerp_root_y(tr, br));
			break;
		case 4:
		case 11:
			isolines.emplace_back(lerp_root_x(tl, tr));
			isolines.emplace_back(lerp_root_y(tr, br));
			break;
		case 5:
			isolines.emplace_back(lerp_root_y(tl, bl));
			isolines.emplace_back(lerp_root_x(tl, tr));
			isolines.emplace_back(lerp_root_x(bl, br));
			isolines.emplace_back(lerp_root_y(tr, br));
			break;
		case 10:
			isolines.emplace_back(lerp_root_y(tl, bl));
			isolines.emplace_back(lerp_root_x(bl, br));
			isolines.emplace_back(lerp_root_x(tl, tr));
			isolines.emplace_back(lerp_root_y(tr, br));
			break;
		case 6:
		case 9:
			isolines.emplace_back(lerp_root_x(tl, tr));
			isolines.emplace_back(lerp_root_x(bl, br));
			break;
		case 7:
		case 8:
			isolines.emplace_back(lerp_root_y(tl, bl));
			isolines.emplace_back(lerp_root_x(tl, tr));
			break;
		}
	}


public:
	void compute_sample_points(double t) {
		sample_points.clear();

		double min = DBL_MAX, max = DBL_MIN;

		for (size_t y = 0; y < rows + 1; ++y) {
		for (size_t x = 0; x < cols + 1; ++x) {
			double noise = Noise::noise(x * noise_width / (cols + 1),
										y * noise_height / (rows + 1),
										t);

			if (noise > max) { max = noise; }
			if (noise < min) { min = noise; }

			sample_points.emplace_back(x * grid_spacing, y * grid_spacing, 2 * noise);
		}
		}

		//std::cout << "[" << min << ", " << max << "]" << std::endl;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_sample_points);
		glBufferData(GL_ARRAY_BUFFER,
					 sample_points.size() * sizeof(SamplePoint),
					 sample_points.data(),
					 GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void march_squares() {
		isolines.clear();

		for (size_t y = 0; y < rows; ++y) {
		for (size_t x = 0; x < cols; ++x) {
			const SamplePoint& bl = sample_points[ y      * (rows + 1) +  x     ];
			const SamplePoint& br = sample_points[ y      * (rows + 1) + (x + 1)];
			const SamplePoint& tr = sample_points[(y + 1) * (rows + 1) + (x + 1)];
			const SamplePoint& tl = sample_points[(y + 1) * (rows + 1) +  x     ];

			compute_isolines(bl, br, tr, tl);
		}
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo_isolines);
		glBufferData(GL_ARRAY_BUFFER,
					 isolines.size() * sizeof(glm::vec2),
					 isolines.data(),
					 GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void render()
	{
		// draw background
		glClearColor(142.0f / 255.0f, 206.0f / 255.0f, 205.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// draw sample points
		shader_sample_points.use();

		glBindVertexArray(vao_sample_points);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_sample_points);

		glPointSize(4);
		glDrawArrays(GL_POINTS, 0, sample_points.size());
		glPointSize(1);

		// draw isolines
		shader_isolines.use();

		glBindVertexArray(vao_isolines);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_isolines);
		
		glLineWidth(2);
		glDrawArrays(GL_LINES, 0, isolines.size());
		glLineWidth(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};