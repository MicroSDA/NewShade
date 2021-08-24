#pragma once
#include "shade_pch.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace shade
{
	namespace math
	{
		// Allgiment for sse
		__declspec(align(16)) struct sse_vec4f
		{
			__m128 xyzw;
			
			sse_vec4f() = default;
			sse_vec4f(__m128& other)
			{
				xyzw = other;
			}
			sse_vec4f(glm::vec4& other)
			{
				xyzw = _mm_load_ps(glm::value_ptr(other));
			}
		};
		// Allgiment for sse
		__declspec(align(16)) struct sse_mat4f
		{
			sse_vec4f col0;
			sse_vec4f col1;
			sse_vec4f col2;
			sse_vec4f col3;

			sse_mat4f() = default;
			sse_mat4f(glm::mat4& other)
			{
				col0 = _mm_loadu_ps(glm::value_ptr(other));
				col1 = _mm_loadu_ps(glm::value_ptr(other) + 4);
				col2 = _mm_loadu_ps(glm::value_ptr(other) + 8);
				col3 = _mm_loadu_ps(glm::value_ptr(other) + 12);
			}

			sse_vec4f operator *(sse_vec4f& vec)
			{
				__m128 xxxx = _mm_shuffle_ps(vec.xyzw, vec.xyzw, _MM_SHUFFLE(0, 0, 0, 0));
				__m128 yyyy = _mm_shuffle_ps(vec.xyzw, vec.xyzw, _MM_SHUFFLE(1, 1, 1, 1));
				__m128 zzzz = _mm_shuffle_ps(vec.xyzw, vec.xyzw, _MM_SHUFFLE(2, 2, 2, 2));
				__m128 wwww = _mm_shuffle_ps(vec.xyzw, vec.xyzw, _MM_SHUFFLE(3, 3, 3, 3));

				return sse_vec4f(_mm_add_ps(
					_mm_add_ps(_mm_mul_ps(xxxx, col0.xyzw), _mm_mul_ps(yyyy, col1.xyzw)),
					_mm_add_ps(_mm_mul_ps(zzzz, col2.xyzw), _mm_mul_ps(wwww, col3.xyzw))
				));
			}

			sse_mat4f operator *(sse_mat4f& other)
			{
				sse_mat4f mat;
				mat.col0 = *this * other.col0; // Check *this !!!
				mat.col1 = *this * other.col1;
				mat.col2 = *this * other.col2;
				mat.col3 = *this * other.col3;
				return mat;
			}
		};
	}
}