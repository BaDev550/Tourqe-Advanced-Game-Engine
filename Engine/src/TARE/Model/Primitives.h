#pragma once
#include <vector>
#include <cmath>
#include "TAGE/Common/TDefines.h"

namespace Primitives {
	void CreateWireSphere(uint& vao, uint& vbo);
	void CreateWireCone(uint& vao, uint& vbo);
	void CreatePointVAO(uint& vao, uint& vbo);
	void CreateArrowVAO(uint& vao, uint& vbo);
}