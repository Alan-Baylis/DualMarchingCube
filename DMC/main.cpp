//
//  main.cpp
//  DMC
//
//  Created by Ye Kuang on 11/24/15.
//  Copyright © 2015 Ye Kuang. All rights reserved.
//

#include <iostream>
#include <bitset>
#include <cmath>

#include "utils.h"
#include "dmc.h"

namespace
{
    class Isosurface
    {
    public:
        virtual ~Isosurface() = default;
        
        virtual float value(float x, float y, float z) const = 0;
    };
    
    class SphereSurface : public Isosurface
    {
    public:
        float value(float x, float y, float z) const override
        {
            return sqrtf(x * x + y * y + z * z);
        }
    };
    
    class GyroidSurface : public Isosurface
    {
    public:
        float value(float x, float y, float z) const override
        {
            return (cosf(x) * sinf(y) + cosf(y) * sinf(z) + cosf(z) * sinf(x));
        }
    };
}

int main(int argc, const char * argv[]) {
    // insert code here...
    using namespace utils;
    using namespace dmc;
    
    // SphereSurface surface;
    GyroidSurface surface;
    float3 xyz_min(-5, -5, -5);
    float3 xyz_max(5, 5, 5);
    float3 xyz_range = xyz_max - xyz_min;
    float iso_value = -1.f;
    
    unsigned resolution = 15;
    Array3D<float> scalar_grid(resolution + 1, resolution + 1, resolution + 1);
    
    for (unsigned k = 0; k < scalar_grid.dim_z(); ++k)
    {
        float z = ijk_to_xyz(k, resolution, xyz_range.z, xyz_min.z);
        for (unsigned j = 0; j < scalar_grid.dim_y(); ++j)
        {
            float y = ijk_to_xyz(j, resolution, xyz_range.y, xyz_min.y);
            for (unsigned i = 0; i < scalar_grid.dim_x(); ++i)
            {
                float x = ijk_to_xyz(i, resolution, xyz_range.x, xyz_min.x);
                scalar_grid(i, j, k) = surface.value(x, y, z);
            }
        }
    }
    
    std::vector<float3> compact_vertices;
    std::vector<uint3> compact_triangles;
    dmc::run_dmc(compact_vertices, compact_triangles, scalar_grid, xyz_min, xyz_max, iso_value);
    for (const auto& vertex : compact_vertices)
    {
        std::cout << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
    }
    for (const auto& tri : compact_triangles)
    {
        std::cout << "f " << tri.x+1 << " " << tri.y+1 << " " << tri.z+1 << std::endl;
    }
    
    return 0;
}
