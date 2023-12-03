void Scene::simulation_update()
{
    // 这次模拟的总时长不是上一帧的时长，而是上一帧时长与之前帧剩余时长的总和，
    // 即上次调用 simulation_update 到现在过了多久。

    // 以固定的时间步长 (time_step) 循环模拟物体运动，每模拟一步，模拟总时长就减去一个
    // time_step ，当总时长不够一个 time_step 时停止模拟。

    // 根据刚才模拟时间步的数量，更新最后一次调用 simulation_update 的时间 (last_update)。

    
    if (!during_animation) {
        return; // 如果没有在模拟中，则直接返回
    }

    time_point now = steady_clock::now();
    duration elapsed_time = now - last_update;
    float elapsed_seconds = duration_cast<duration>(elapsed_time).count();

    while (elapsed_seconds > time_step) {
        for (auto object : all_objects) 
        {
            // 使用 Object::update 方法更新物体状态
            object->update(all_objects);
        }

        elapsed_seconds -= time_step;
    }

    duration remaining_time = std::chrono::duration<float>(elapsed_seconds);

    // 更新 last_update 时间
    // 注意：这里我们从 elapsed_time 中减去 remaining_time 来获得实际上模拟过的时间
    last_update = last_update + std::chrono::duration_cast<std::chrono::steady_clock::duration>(elapsed_time - remaining_time);


    /*
    使用 last_update += (elapsed_time - remaining_time); 与直接设置 last_update = steady_clock::now(); 的主要区别在于它们如何处理时间更新。

    last_update += (elapsed_time - remaining_time); 这行代码的意图是将 last_update 设置为最后一次模拟步骤之后的确切时间点。
    这意味着即使执行 simulation_update 函数本身需要花费一些时间（代码执行时间），last_update 也会精确地反映模拟的时间进度，而不是实际世界的时间。

    last_update = steady_clock::now(); 直接将 last_update 设置为当前的实时时间点。
    这意味着 last_update 包含了执行 simulation_update 函数的时间。如果这个函数的执行时间不可忽略，它可能导致模拟时间比实际过去的时间快。

    在大多数情况下，函数执行时间是非常短的，所以两种方法之间的差别可能非常小，几乎可以忽略不计。
    然而，在实时系统或对精度要求极高的模拟中，更准确地跟踪模拟时间可能是重要的。在这种情况下，使用 last_update += (elapsed_time - remaining_time); 更合适，因为它提供了更精确的模拟时间控制。*/
}
void Object::update(vector<Object*>& all_objects)
{
    // 首先调用 step 函数计下一步该物体的运动学状态。
    KineticState current_state{center, velocity, force / mass};
    KineticState next_state = step(prev_state, current_state);
    //(void)next_state;
    // 将物体的位置移动到下一步状态处，但暂时不要修改物体的速度。
    
    // 更新物体的位置和速度为下一步的状态
    center = next_state.position;  // 更新位置
    velocity = next_state.velocity;  // 更新速度
    force = next_state.acceleration * mass; 


    // 更新上一步的状态为当前状态
    prev_state = current_state;

    
    // 遍历 all_objects，检查该物体在下一步状态的位置处是否会与其他物体发生碰撞。
    for (auto object : all_objects) {
        (void)object;

        // 检测该物体与另一物体是否碰撞的方法是：
        // 遍历该物体的每一条边，构造与边重合的射线去和另一物体求交，如果求交结果非空、
        // 相交处也在这条边的两个端点之间，那么该物体与另一物体发生碰撞。
        // 请时刻注意：物体 mesh 顶点的坐标都在模型坐标系下，你需要先将其变换到世界坐标系。
        for (size_t i = 0; i < mesh.edges.count(); ++i) {
            array<size_t, 2> v_indices = mesh.edge(i);
            (void)v_indices;
            // v_indices 中是这条边两个端点的索引，以这两个索引为参数调用 GL::Mesh::vertex
            // 方法可以获得它们的坐标，进而用于构造射线。
            if (BVH_for_collision) {
            } else {
            }
            // 根据求交结果，判断该物体与另一物体是否发生了碰撞。
            // 如果发生碰撞，按动量定理计算两个物体碰撞后的速度，并将下一步状态的位置设为
            // current_state.position ，以避免重复碰撞。
        }
    } 
    // 将上一步状态赋值为当前状态，并将物体更新到下一步状态。
}
// Function to perform a single Forward Euler step
KineticState forward_euler_step([[maybe_unused]] const KineticState& previous,
                                const KineticState& current)
{
    // Δt 是时间步长，可以是一个常数，例如 0.01 或者是一个可配置的值
    float delta_time = time_step; // 假设 time_step 已经定义了

    // 根据前向欧拉法，下一个状态的位置是当前位置加上当前速度乘以时间步长
    Eigen::Vector3f next_position = current.position + current.velocity * delta_time;

    // 下一个状态的速度是当前速度加上当前加速度乘以时间步长
    Eigen::Vector3f next_velocity = current.velocity + current.acceleration * delta_time;

    // 返回下一个状态
    return KineticState(next_position, next_velocity, current.acceleration);
}
