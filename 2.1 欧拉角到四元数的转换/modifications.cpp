void Toolbar::layout_mode(Scene& scene)
{
    if (ImGui::BeginTabItem("Layout")) {
        if (mode != WorkingMode::LAYOUT) {
            on_selection_canceled();
            mode = WorkingMode::LAYOUT;
        }
        scene_hierarchies(scene);

        Object* selected_object = scene.selected_object;
        if (selected_object != nullptr) {
            material_editor(selected_object->mesh.material);
            ImGui::SeparatorText("Transform");
            ImGui::Text("Translation");
            ImGui::PushID("Translation##");
            Vector3f& center = selected_object->center;
            xyz_drag(&center.x(), &center.y(), &center.z(), POSITION_UNIT);
            ImGui::PopID();

            ImGui::Text("Scaling");
            ImGui::PushID("Scaling##");
            Vector3f& scaling = selected_object->scaling;
            xyz_drag(&scaling.x(), &scaling.y(), &scaling.z(), SCALING_UNIT);
            ImGui::PopID();

            const Quaternionf& r             = selected_object->rotation;
            auto [x_angle, y_angle, z_angle] = quaternion_to_ZYX_euler(r.w(), r.x(), r.y(), r.z());
            ImGui::Text("Rotation (ZYX Euler)");
            ImGui::PushID("Rotation##");
            ImGui::PushItemWidth(0.3f * ImGui::CalcItemWidth());
            ImGui::DragFloat("pitch", &x_angle, ANGLE_UNIT, -180.0f, 180.0f, "%.1f deg",
                             ImGuiSliderFlags_AlwaysClamp);
            ImGui::SameLine();
            ImGui::DragFloat("yaw", &y_angle, ANGLE_UNIT, -90.0f, 90.0f, "%.1f deg",
                             ImGuiSliderFlags_AlwaysClamp);
            ImGui::SameLine();
            ImGui::DragFloat("roll", &z_angle, ANGLE_UNIT, -180.0f, 180.0f, "%.1f deg",
                             ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopItemWidth();
            ImGui::PopID();

            // 获取当前四元数旋转
            Quaternionf& q = selected_object->rotation;

            // 从四元数转换为欧拉角
            auto [original_x_angle, original_y_angle, original_z_angle] = quaternion_to_ZYX_euler(q.w(), q.x(), q.y(), q.z());
            
            // 根据新的欧拉角和原始角度计算四元数
            Quaternionf qx (AngleAxisf((x_angle - original_x_angle) * M_PI / 180.0f, Vector3f::UnitX()));
            Quaternionf qy (AngleAxisf((y_angle - original_y_angle) * M_PI / 180.0f, Vector3f::UnitY()));
            Quaternionf qz (AngleAxisf((z_angle - original_z_angle) * M_PI / 180.0f, Vector3f::UnitZ()));

            // 更新物体的旋转
            selected_object->rotation = q * (qz * qy * qx);
            
            /*
            selected_object->rotation = AngleAxisf(radians(x_angle), Vector3f::UnitX()) *
                                        AngleAxisf(radians(y_angle), Vector3f::UnitY()) *
                                        AngleAxisf(radians(z_angle), Vector3f::UnitZ());
                */
        }
        ImGui::EndTabItem();
    }
}
