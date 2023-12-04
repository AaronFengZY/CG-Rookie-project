# 这份代码是用前向欧拉法模拟运动

模拟的方式也十分简单，就是简单的近似：

实现在 `simulation/solver.cpp` 中的 `forward_euler_step` 函数。

$$
x_{k+1} = x_k + v\Delta t
$$

$$
v_{k+1} = v_k + a\Delta t
$$


另外要注意的就是 `Scene/scene.cpp` 中的 `Scene::simulation_update` 函数。这个函数中主要做的工作是更新 `last_update` 时间。注意：这里我们从 `elapsed_time` 中减去 `remaining_time` 来获得实际上模拟过的时间：

```cpp
last_update = last_update + std::chrono::duration_cast<std::chrono::steady_clock::duration>(elapsed_time - remaining_time);
