// Function to perform a single Runge-Kutta step
KineticState runge_kutta_step([[maybe_unused]]const KineticState& previous, const KineticState& current) {
    float delta_time = time_step; // time_step 已经定义了

    // 第一步，计算 k1
    KineticState k1 = derivative(current);
    k1.position = current.velocity * delta_time;
    k1.velocity = current.acceleration * delta_time;

    // 第二步，计算 k2
    KineticState temp = current;
    temp.position += k1.position * 0.5f;
    temp.velocity += k1.velocity * 0.5f;
    KineticState k2 = derivative(temp);
    k2.position = (current.velocity + k1.velocity * 0.5f) * delta_time;
    k2.velocity = (current.acceleration + k1.acceleration * 0.5f) * delta_time;

    // 第三步，计算 k3
    temp.position = current.position + k2.position * 0.5f;
    temp.velocity = current.velocity + k2.velocity * 0.5f;
    KineticState k3 = derivative(temp);
    k3.position = (current.velocity + k2.velocity * 0.5f) * delta_time;
    k3.velocity = (current.acceleration + k2.acceleration * 0.5f) * delta_time;

    // 第四步，计算 k4
    temp.position = current.position + k3.position;
    temp.velocity = current.velocity + k3.velocity;
    KineticState k4 = derivative(temp);
    k4.position = (current.velocity + k3.velocity) * delta_time;
    k4.velocity = (current.acceleration + k3.acceleration) * delta_time;

    // 结合所有步骤计算最终位置和速度
    Eigen::Vector3f next_position = current.position + (k1.position + 2.0f * k2.position + 2.0f * k3.position + k4.position) / 6.0f;
    Eigen::Vector3f next_velocity = current.velocity + (k1.velocity + 2.0f * k2.velocity + 2.0f * k3.velocity + k4.velocity) / 6.0f;

    // 返回新的运动状态
    return KineticState(next_position, next_velocity, current.acceleration);
}

// Function to perform a single Backward Euler step
KineticState backward_euler_step([[maybe_unused]]const KineticState& previous, const KineticState& current) 
{
    KineticState next;

    // 时间步长
    float dt = time_step;

    // 使用当前加速度更新速度（加速度假设为恒定）
    next.velocity = current.velocity + current.acceleration * dt;

    // 使用更新后的速度更新位置
    next.position = current.position + next.velocity * dt;

    // 假设加速度不变
    next.acceleration = current.acceleration;

    return next;
}

// Function to perform a single Symplectic Euler step
KineticState symplectic_euler_step([[maybe_unused]]const KineticState& previous, const KineticState& current)
{
    KineticState next;

    // 时间步长
    float dt = time_step;

    // 显式更新速度
    next.velocity = current.velocity + current.acceleration * dt;

    // 使用更新后的速度隐式更新位置
    next.position = current.position + next.velocity * dt;

    // 假设加速度不变
    next.acceleration = current.acceleration;

    return next;
}
