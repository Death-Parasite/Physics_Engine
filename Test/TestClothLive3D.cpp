#include <raylib.h>
#include <raymath.h>
#include "../Cloth/ClothSolver.h"

using namespace Physics;

int main() {
    const int screenW = 1280;
    const int screenH = 720;
    InitWindow(screenW, screenH, "NextGen Physics Engine - SIGGRAPH Sphere Drape Benchmark");
    SetTargetFPS(60);

    Camera3D camera = { 0 };
    camera.position = Vector3{ 3.5f, 2.5f, 3.5f }; 
    camera.target   = Vector3{ 0.0f, 0.5f, 0.0f }; 
    camera.up       = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    int gridW = 18; // 18x18 = 324 High-Res Silk Particles
    int gridH = 18;
    ClothSolver solver;
    solver.InitializeGridMesh(gridW, gridH, 0.12f, 1.5f);

    // Gola exact center par rakha hai, radius 0.8 meter
    Vec3 sphereCenter(0.0f, 0.6f, 0.0f);
    float sphereRadius = 0.8f;
    solver.SetSphereCollider(sphereCenter, sphereRadius);

    // NOTE: Koii bhi particle Pin nahi kiya! Poora kapda free-fall karega!

    Vec3 gravity(0.0f, -9.81f, 0.0f);
    bool isPaused = true; // Shuru me Pause rakha hai taaki tu SPACE dabakar girta hua dekhe!

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_ORBITAL); 
        if (IsKeyPressed(KEY_SPACE)) isPaused = !isPaused;

        if (IsKeyPressed(KEY_R)) {
            solver = ClothSolver(); 
            solver.InitializeGridMesh(gridW, gridH, 0.12f, 1.5f);
            solver.SetSphereCollider(sphereCenter, sphereRadius);
            isPaused = true;
        }

        if (!isPaused) {
            float dt = GetFrameTime();
            solver.StepSimulation(dt, gravity, 10); // 10 sub-steps for ultra smooth drape
        }

        BeginDrawing();
            ClearBackground(Color{ 20, 20, 25, 255 }); 

            BeginMode3D(camera);
                DrawGrid(10, 1.0f); 

                // Draw Solid Red Sphere Collider
                DrawSphere(Vector3{sphereCenter.x, sphereCenter.y, sphereCenter.z}, sphereRadius, Color{ 180, 40, 40, 255 });
                DrawSphereWires(Vector3{sphereCenter.x, sphereCenter.y, sphereCenter.z}, sphereRadius, 16, 16, Color{ 255, 100, 100, 100 });

                const auto& pData = solver.GetParticleData();

                for (size_t i = 0; i < pData.triangles.size(); i += 3) {
                    Vec3 p1 = pData.positions[pData.triangles[i]];
                    Vec3 p2 = pData.positions[pData.triangles[i+1]];
                    Vec3 p3 = pData.positions[pData.triangles[i+2]];

                    Vector3 v1 = Vector3{ p1.x, p1.y, p1.z };
                    Vector3 v2 = Vector3{ p2.x, p2.y, p2.z };
                    Vector3 v3 = Vector3{ p3.x, p3.y, p3.z };

                    DrawTriangle3D(v1, v2, v3, Color{ 0, 180, 220, 210 });
                    DrawTriangle3D(v1, v3, v2, Color{ 0, 130, 180, 210 }); 

                    DrawLine3D(v1, v2, Color{ 200, 240, 255, 150 });
                    DrawLine3D(v2, v3, Color{ 200, 240, 255, 150 });
                }

            EndMode3D();

            DrawText("BENCHMARK: THE SIGGRAPH SPHERE DRAPE TEST", 20, 20, 20, RAYWHITE);
            DrawText(isPaused ? "STATUS: PAUSED [Press SPACE to Drop Cloth!]" : "STATUS: SIMULATING [Press SPACE to Pause]", 20, 50, 16, isPaused ? YELLOW : GREEN);
            DrawText("CONTROLS: [SPACE] Drop/Pause | [R] Reset Drop | [Drag Mouse] Orbit Camera", 20, 75, 16, LIGHTGRAY);
            DrawFPS(1180, 20);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}