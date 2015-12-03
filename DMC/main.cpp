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
#include "png_loader.h"
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
            return 2.0 * (cosf(x) * sinf(y) + cosf(y) * sinf(z) + cosf(z) * sinf(x));
        }
    };
    
    void test_dmc()
    {
        using namespace utils;
        using namespace png_load;
        using namespace dmc;
        
        // SphereSurface surface;
        // GyroidSurface surface;
        float3 xyz_min(0, 0, 0);
        float3 xyz_max(10, 10, 10);
        // float3 xyz_range = xyz_max - xyz_min;
        float iso_value = 0.8f;
        
        // unsigned resolution = 3;
        // Array3D<float> scalar_grid(resolution + 1, resolution + 1, resolution + 1);
        
        // Actually it's 400x296x320, downsample
        unsigned i_resl(200), j_resl(148), k_resl(236/2);
        Array3D<float> scalar_grid(i_resl, j_resl, k_resl);
        
        {
            PngLoader loader("walnut_pngs/walnut_", k_resl);
            std::vector<unsigned char> png_data;
            
            float grid_val;
            for (unsigned slice_k = 0; slice_k < k_resl; ++slice_k)
            {
                png_data.clear();
                loader.load(slice_k * 2, png_data);
                
                for (unsigned j = 0; j < j_resl; ++j)
                {
                    for (unsigned i = 0; i < i_resl; ++i)
                    {
                        grid_val = png_data[4 * j * i_resl + 2 * i] > 0 ? 1.0f : 0.0f;
                        scalar_grid(i, j, slice_k) = grid_val;
                    }
                }
                // std::cout << "done for slice " << slice_k << std::endl;
            }
        }
        /*
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
        */
        std::vector<float3> compact_vertices;
        std::vector<uint3> compact_triangles;
        dmc::run_dmc(compact_vertices, compact_triangles, scalar_grid, xyz_min, xyz_max, iso_value, 16);
        for (const auto& vertex : compact_vertices)
        {
            std::cout << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
        }
        for (const auto& tri : compact_triangles)
        {
            std::cout << "f " << tri.x+1 << " " << tri.y+1 << " " << tri.z+1 << std::endl;
        }
    }
}
int main(int argc, const char * argv[]) {
    // insert code here...
    using namespace utils;
    using namespace dmc;
    
    // std::cout << argmax(1, 2, 3, 4, -2, -3, 6, 0) << std::endl;
    // std::cout << calc_radian({0,0}, {1,0}, {2.732, 1});
    test_dmc();
    
    return 0;
}
