// 给定坐标(x,y)以及三角形的三个顶点坐标，判断(x,y)是否在三角形的内部
// Vector4f* vertices 是一个指针，里面包含3个点的坐标
//Eigen::Vector4f 是一个四维向量，通常在图形处理中用来表示三维空间中的点。
//这里的四个分量通常是 (x, y, z, w)，其中 x, y, z 表示点在三维空间中的坐标，而 w 是一个齐次坐标，常用于进行三维变换。
bool Rasterizer::inside_triangle(int x, int y, const Vector4f* vertices) //采用重心计算的方法判断
//这个函数首先将三角形的三个顶点和待检测的点转换为二维空间中的点。然后，它计算了这些点形成的向量，
//并用这些向量来计算三角形的总面积（实际上是面积的两倍，因为使用了叉积）。接着，它计算了重心坐标（alpha、beta、gamma），
//这些坐标表明了点 P 相对于三角形各顶点的位置。如果所有这些坐标都是非负的，那么点 P 就在三角形内部。
{
    // 将顶点转换为二维点（只考虑x, y坐标）
    Eigen::Vector2f v0(vertices[0][0], vertices[0][1]);
    Eigen::Vector2f v1(vertices[1][0], vertices[1][1]);
    Eigen::Vector2f v2(vertices[2][0], vertices[2][1]);

    // 将待检测点转换为Eigen::Vector2f
    Eigen::Vector2f P(x, y);

    // 计算向量
    Eigen::Vector2f v0P = P - v0;
    Eigen::Vector2f v1P = P - v1;
    Eigen::Vector2f v2P = P - v2;

    // 计算三角形的面积的两倍（伪叉积）
    Eigen::Vector2f v01 = v1 - v0;
    Eigen::Vector2f v02 = v2 - v0;
    float area = v01.x() * v02.y() - v01.y() * v02.x();

    // 计算重心坐标
    float alpha = (v1P.x() * v2P.y() - v1P.y() * v2P.x()) / area;
    float beta = (v2P.x() * v0P.y() - v2P.y() * v0P.x()) / area;
    float gamma = 1 - alpha - beta;

    // 检查点是否在三角形内
    return (alpha >= 0) && (beta >= 0) && (gamma >= 0);
}

// 给定坐标(x,y)以及三角形的三个顶点坐标，计算(x,y)对应的重心坐标[alpha, beta, gamma]
tuple<float, float, float> Rasterizer::compute_barycentric_2d(float x, float y, const Vector4f* v)
//这个函数的逻辑与之前的 inside_triangle 函数类似，但是它返回重心坐标（alpha, beta, gamma），
//而不是一个布尔值。重心坐标表示点 P 相对于三角形顶点的位置。如果这些坐标都是非负的，并且它们的和为 1，那么点 P 就在三角形内部。
{
    // 将顶点转换为二维点（只考虑x, y坐标）
    Eigen::Vector2f v0(v[0][0], v[0][1]);
    Eigen::Vector2f v1(v[1][0], v[1][1]);
    Eigen::Vector2f v2(v[2][0], v[2][1]);

    // 将待检测点转换为Eigen::Vector2f
    Eigen::Vector2f P(x, y);

    // 计算三角形面积的两倍（使用伪叉积）
    Eigen::Vector2f v01 = v1 - v0;
    Eigen::Vector2f v02 = v2 - v0;
    float area = v01.x() * v02.y() - v01.y() * v02.x();

    // 计算重心坐标
    float alpha = ((v1 - P).x() * (v2 - P).y() - (v1 - P).y() * (v2 - P).x()) / area;
    float beta = ((v2 - P).x() * (v0 - P).y() - (v2 - P).y() * (v0 - P).x()) / area;
    float gamma = 1.0f - alpha - beta;

    // 返回重心坐标
    return std::make_tuple(alpha, beta, gamma);
}
// 对当前渲染物体的所有三角形面片进行遍历，进行几何变换以及光栅化
void Rasterizer::draw(const std::vector<Triangle>& TriangleList, const GL::Material& material,
                      const std::list<Light>& lights, const Camera& camera)
{
    // 遍历所有三角形
    for (const Triangle& tri : TriangleList) {
        std::array<VertexShaderPayload, 3> transformed_vertices;
        std::array<Vector3f, 3> world_positions;

        // 对每个顶点应用顶点着色器
        for (int i = 0; i < 3; ++i) {
            VertexShaderPayload payload { tri.vertex[i], tri.normal[i] };
            transformed_vertices[i] = vertex_shader(payload);

            // 将顶点坐标变换到世界坐标系
            Eigen::Vector4f world_pos = model * tri.vertex[i];
            world_positions[i] = world_pos.head<3>(); // 只取前三个分量(x, y, z)
        }

        Triangle tramsformed_triangle;

        // 遍历每个顶点，将位置和法线从VertexShaderPayload复制到Triangle
        for (int i = 0; i < 3; ++i) {
            tramsformed_triangle.vertex[i] = transformed_vertices[i].position; // 顶点位置
            tramsformed_triangle.normal[i] = transformed_vertices[i].normal;   // 法线向量
        }

        /*
        cout<<"normal"<<endl;
        for (int i=0; i<3; ++i)
        {
            cout<<tramsformed_triangle.normal[i]<<endl;
        }
        cout<<endl;
        */

        // 调用光栅化函数
        rasterize_triangle(tramsformed_triangle, world_positions, material, lights, camera);
    }
}
// 对当前三角形进行光栅化
// t的顶点在屏幕坐标，法线在相机坐标
// world_pos在世界坐标系下三个点
void Rasterizer::rasterize_triangle(const Triangle& t, const std::array<Vector3f, 3>& world_pos, 
                                    GL::Material material, const std::list<Light>& lights,
                                    Camera camera)
{
    std::array<Vector3f, 3> world_vertex_pos=world_pos;  //世界坐标系下的顶点坐标
    std::array<Vector3f, 3> world_normals;  //世界坐标下的法线坐标
    //std::array<Vector3f, 3> view_vertex_pos=world_pos;  //相机坐标系下的顶点坐标
    
    
    for (int i = 0; i < 3; ++i)
        world_normals[i] = t.normal[i];
/*
    for (int i = 0; i < 3; ++i) {
        // 将世界坐标系的顶点转换为齐次坐标
        Eigen::Vector4f world_pos_homogeneous(world_pos[i].x(), world_pos[i].y(), world_pos[i].z(), 1.0f);

        // 使用视图矩阵进行坐标变换
        Eigen::Vector4f view_pos_homogeneous = view * world_pos_homogeneous;

        // 从齐次坐标转换回三维坐标
        view_vertex_pos[i] = view_pos_homogeneous.head<3>() / view_pos_homogeneous.w();
    }*/
    

    for (int x = 0; x < width; ++x) 
        for (int y = 0; y < height; ++y) 
        {
            // 判断像素是否在三角形内部
            if (inside_triangle(x, y, t.vertex))
            {
                tuple<float, float, float>barycentric_tuple = compute_barycentric_2d(x, y, t.vertex);

                // 提取第一个元素（alpha）
                float alpha = std::get<0>(barycentric_tuple);
                // 提取第二个元素（beta）
                float beta = std::get<1>(barycentric_tuple);
                // 提取第三个元素（gamma）
                float gamma = std::get<2>(barycentric_tuple);

                //weight
                Eigen::Vector3f weight(t.vertex[0][3],t.vertex[1][3],t.vertex[2][3]);

                // Z  1 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                float Z=1/(alpha/weight[0] + beta/weight[1] + gamma/weight[2]);
                
                // 对深度值进行透视矫正插值
                //Eigen::Vector3f interpolated_view_vertex=interpolate(alpha,beta,gamma,view_vertex_pos[0],view_vertex_pos[1],view_vertex_pos[2],weight,Z);

                //深度测试
                float depth = alpha *  t.vertex[0][2]/ weight[0] + beta * t.vertex[1][2] / weight[1] +
                              gamma * t.vertex[2][2] / weight[2];
                depth *= Z;

                int now_index = get_index(x,y);
                if (depth < depth_buf[now_index])
                    depth_buf[now_index] = depth; 
                else
                {
                    //cout<<"hh"<<endl;
                    continue;
                }
                // 对顶点坐标进行透视矫正插值  
                Eigen::Vector3f interpolated_vertex=interpolate(alpha,beta,gamma,world_vertex_pos[0],world_vertex_pos[1],world_vertex_pos[2],weight,Z);


                // 对法线进行透视矫正插值 
                Eigen::Vector3f interpolated_normal=interpolate(alpha,beta,gamma,world_normals[0],world_normals[1],world_normals[2],weight,Z);
                interpolated_normal = interpolated_normal.normalized();
                //cout<<"interpolate_normal = "<<interpolated_normal<<endl;

                FragmentShaderPayload interpolate_payload(interpolated_vertex, interpolated_normal);
                Eigen::Vector3f color = phong_fragment_shader(interpolate_payload, material, lights, camera);

                //cout<<"color ("<<x<<","<<y<<")" <<color<<endl; 

                Eigen::Vector2i point(x, y);
                set_pixel(point, color);
            }
        }
}
// vertex shader
// 将顶点坐标变换到投影平面，再进行视口变换；同时将法线向量变换到世界坐标
// 系用于后续插值
VertexShaderPayload vertex_shader(const VertexShaderPayload& payload)
{
    VertexShaderPayload output;

    // 对顶点坐标进行模型-视图-投影变换
    Eigen::Vector4f position = Uniforms::MVP * payload.position;

    // 进行透视除法，只对x, y, z坐标除以w
    if (position.w() != 0.0f) 
    {
        position.x() /= position.w();
        position.y() /= position.w();
        //position.z() /= position.w();
    }

    //std::cout << "width" << Uniforms::width << std::endl;
    //std::cout << "height" << Uniforms::height << std::endl;

    //std::cout << "Before viewport transform - position.x: " << position.x() << std::endl;
    //std::cout << "Before viewport transform - position.y: " << position.y() << std::endl;

    // 视口变换
    // 将NDC坐标映射到屏幕坐标
    position.x() = (position.x() + 1.0f) * Uniforms::width / 2.0f;
    position.y() = (position.y() + 1.0f) * Uniforms::height / 2.0f;
    // 这里假设z坐标已经在合适的范围内（[-1, 1]）

    //std::cout << "After viewport transform - position.x: " << position.x() << std::endl;
    //std::cout << "After viewport transform - position.y: " << position.y() << std::endl;

    // 对法线进行模型变换（逆转置矩阵）
    Eigen::Vector4f normal = Eigen::Vector4f(payload.normal[0], payload.normal[1], payload.normal[2], 0.0f);
    normal = Uniforms::inv_trans_M * normal; //这边inv_trans_M 是变换到相机坐标
    normal.normalize(); // 确保法线是单位向量

    // 更新输出
    output.position = position;
    output.normal = Eigen::Vector3f(normal[0], normal[1], normal[2]);

    return output; 
}


//使用 Blinn Phong 模型计算每个片元 (像素) 的颜色
Vector3f phong_fragment_shader(const FragmentShaderPayload& payload, GL::Material material,
                               const std::list<Light>& lights, Camera camera)
{
    Vector3f color(0, 0, 0);
    Vector3f point = payload.world_pos; // 片元在世界坐标系中的位置
    Vector3f normal = payload.world_normal.normalized(); // 法线需要标准化

    // 环境光 - 直接使用材质的 ambient 分量
    Vector3f ambient_light = material.ambient;
    ambient_light = ambient_light * 1;

    // 遍历所有光源，计算漫反射和镜面反射
    for (const Light& light : lights) 
    {
        Vector3f light_dir = (light.position - point).normalized(); // 从点到光源的方向
        float distance_square = (light.position - point).squaredNorm(); // 距离的平方，用于计算衰减

        // 光照衰减
        float attenuated_light = light.intensity / distance_square;

        // 漫反射
        float diff = std::max(light_dir.dot(normal), 0.0f); // 光源方向和法线的点积
        Vector3f diffuse_light = diff * material.diffuse * attenuated_light; // 乘以衰减后的光照

        // 镜面反射 - Blinn-Phong 模型
        Vector3f view_dir = (camera.position - point).normalized(); // 从点到相机的方向
        Vector3f halfway_dir = (light_dir + view_dir).normalized(); // Blinn-Phong 的半向量
        float spec = pow(std::max(normal.dot(halfway_dir), 0.0f), material.shininess); // 镜面反射分量计算
        Vector3f specular_light = spec * material.specular * attenuated_light; // 乘以衰减后的光照

        // 计算总照明
        color += diffuse_light + specular_light;
    }

    // 加入环境光
    color += ambient_light;

    // 确保颜色值在合法范围内
    color = color.cwiseMin(Vector3f(1, 1, 1)); // 颜色值不超过1
    // 将颜色从 [0, 1] 范围转换到 [0, 255] 范围以供显示
    color *= 255.0f;

    return color;
}
