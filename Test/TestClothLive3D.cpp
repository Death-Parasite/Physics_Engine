#include <raylib.h>
#include <rlgl.h> 
#include <raymath.h>
#include <cmath>
#include <vector>
#include "../Cloth/ClothSolver.h"
#include "../Math/VectorMath.h"

using namespace Physics;

std::vector<int> g_PinnedParticles;
Vec3 g_HumanPos(0, 0, 0);
static std::vector<Vec3> s_VertexNormalsCache;

void BuildHumanColliders(ClothSolver& solver, const Vec3& pos) {
    solver.ClearColliders();
    solver.AddSphereCollider(pos + Vec3(0.0f, 1.4f, 0.0f), 0.22f); 
    solver.AddCapsuleCollider(pos + Vec3(0.0f, 1.2f, 0.0f), pos + Vec3(0.0f, 1.0f, 0.0f), 0.12f); 
    solver.AddCapsuleCollider(pos + Vec3(-0.45f, 0.95f, 0.0f), pos + Vec3(0.45f, 0.95f, 0.0f), 0.25f); 
    solver.AddCapsuleCollider(pos + Vec3(0.0f, 0.9f, 0.05f), pos + Vec3(0.0f, 0.2f, 0.05f), 0.28f); 
}

void LoadScene(ClothSolver& solver, int sceneID, int gridW, int gridH, float spacing) {
    solver = ClothSolver(); 
    g_PinnedParticles.clear();
    g_HumanPos = Vec3(0, 0, 0);

    solver.InitializeGridMesh(gridW, gridH, spacing, 2.2f);

    if (sceneID == 1) { 
        solver.AddSphereCollider(Vec3(0.0f, 0.3f, 0.0f), 0.8f);
    } 
    else if (sceneID == 2) { 
        for (int y = 0; y < gridH; ++y) solver.PinParticle(y * gridW);
    }
    else if (sceneID == 3) { 
        BuildHumanColliders(solver, g_HumanPos);
        for (int x = 0; x < gridW; ++x) {
            solver.PinParticle(x);
            g_PinnedParticles.push_back(x);
        }
    }
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT); 
    InitWindow(1280, 720, "NextGen Engine - FINAL BUILD: 8,192 Faces | OpenMP AVX Turbo | GLB Ready");
    SetTargetFPS(144); // Unlocked to 144Hz monitor spec

    Camera3D camera = { 0 };
    camera.position = Vector3{ 3.2f, 2.0f, 3.2f }; 
    camera.target   = Vector3{ 0.0f, 0.5f, 0.0f }; 
    camera.up       = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // --- THE 1.5x MAGNUM OPUS TARGET: 65x65 Grid = EXACTLY 8,192 FACES ---
    int gridW = 65; 
    int gridH = 65;
    float spacing = 0.038f; // Hyper-dense AAA silk

    ClothSolver solver;
    int currentScene = 1; 
    LoadScene(solver, currentScene, gridW, gridH, spacing);

    Vec3 gravity(0.0f, -9.81f, 0.0f);
    Vec3 wind(0.0f, 0.0f, 0.0f);
    bool isPaused = true; 

    // --- SMART GLB LOADER HOOK ---
    Model characterGLB = { 0 };
    bool hasGLB = false;
    if (FileExists("character.glb")) {
        characterGLB = LoadModel("character.glb");
        hasGLB = true;
    }

    Vec3 lightDir = Math::Normalize(Vec3(-0.5f, -1.0f, -0.5f));

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_ORBITAL); 
        if (IsKeyPressed(KEY_SPACE)) isPaused = !isPaused;

        for (int i = 1; i <= 3; ++i) {
            int targetID = (i == 3) ? 3 : i; 
            if (IsKeyPressed(KEY_ZERO + i) && currentScene != targetID) {
                currentScene = targetID;
                LoadScene(solver, currentScene, gridW, gridH, spacing);
                isPaused = false;
            }
        }

        wind = (currentScene == 2) ? Vec3(15.0f, 2.0f, 5.0f) : Vec3(0,0,0);

        if (currentScene == 3 && !isPaused) {
            Vec3 moveDelta(0, 0, 0);
            float walkSpeed = 2.8f * GetFrameTime();
            if (IsKeyDown(KEY_UP)) moveDelta.z -= walkSpeed;
            if (IsKeyDown(KEY_DOWN)) moveDelta.z += walkSpeed;
            if (IsKeyDown(KEY_LEFT)) moveDelta.x -= walkSpeed;
            if (IsKeyDown(KEY_RIGHT)) moveDelta.x += walkSpeed;

            if (Math::Length(moveDelta) > 0.0f) {
                g_HumanPos = g_HumanPos + moveDelta;
                BuildHumanColliders(solver, g_HumanPos);
                for (int pIdx : g_PinnedParticles) solver.MoveParticle(pIdx, moveDelta);
            }
        }

        if (!isPaused) {
            // 8,000 faces me stability ke liye 5 sub-steps best sweet spot hai
            solver.StepSimulation(GetFrameTime(), gravity, wind, 5); 
        }

        // PRE-CALCULATE NORMALS
        const auto& pData = solver.GetParticleData();
        s_VertexNormalsCache.assign(pData.count, Vec3(0,0,0)); 
        
        size_t triSize = pData.triangles.size();
        for (size_t i = 0; i < triSize; i += 3) {
            int i1 = pData.triangles[i];
            int i2 = pData.triangles[i+1];
            int i3 = pData.triangles[i+2];
            Vec3 fn = Math::Cross(pData.positions[i2] - pData.positions[i1], pData.positions[i3] - pData.positions[i1]);
            s_VertexNormalsCache[i1] = s_VertexNormalsCache[i1] + fn;
            s_VertexNormalsCache[i2] = s_VertexNormalsCache[i2] + fn;
            s_VertexNormalsCache[i3] = s_VertexNormalsCache[i3] + fn;
        }
        for (size_t i = 0; i < pData.count; ++i) s_VertexNormalsCache[i] = Math::Normalize(s_VertexNormalsCache[i]);

        // RENDER
        BeginDrawing();
            ClearBackground(Color{ 18, 18, 20, 255 }); 
            BeginMode3D(camera);
                DrawGrid(10, 1.0f); 

                // DRAW CHARACTER (GLB OR FALLBACK)
                if (currentScene == 3 && hasGLB) {
                    Vector3 glbPos = { g_HumanPos.x, g_HumanPos.y - 0.4f, g_HumanPos.z };
                    DrawModel(characterGLB, glbPos, 1.0f, WHITE);
                } else {
                    for (const auto& s : solver.GetSpheres()) DrawSphere(Vector3{s.center.x, s.center.y, s.center.z}, s.radius, Color{65,65,70,255});
                    for (const auto& c : solver.GetCapsules()) {
                        Vector3 pA = {c.pA.x, c.pA.y, c.pA.z}; Vector3 pB = {c.pB.x, c.pB.y, c.pB.z};
                        DrawSphere(pA, c.radius, Color{65,65,70,255}); DrawSphere(pB, c.radius, Color{65,65,70,255});
                        DrawCylinderEx(pA, pB, c.radius, c.radius, 16, Color{65,65,70,255});
                    }
                }

                if (currentScene == 2) DrawCylinder(Vector3{ -gridW*spacing*0.5f, 2.2f, -gridH*spacing*0.5f }, 0.04f, 0.04f, 4.0f, 12, DARKGRAY);

                rlDisableBackfaceCulling(); 
                rlBegin(RL_TRIANGLES);
                Color baseColor = (currentScene == 1) ? Color{ 160, 25, 35, 255 } : (currentScene == 2) ? Color{ 20, 115, 225, 255 } : Color{ 30, 180, 100, 255 };  

                for (size_t i = 0; i < triSize; i += 3) {
                    int idx1 = pData.triangles[i]; int idx2 = pData.triangles[i+1]; int idx3 = pData.triangles[i+2];
                    Vec3 p1 = pData.positions[idx1]; Vec3 n1 = s_VertexNormalsCache[idx1];
                    Vec3 p2 = pData.positions[idx2]; Vec3 n2 = s_VertexNormalsCache[idx2];
                    Vec3 p3 = pData.positions[idx3]; Vec3 n3 = s_VertexNormalsCache[idx3];

                    auto FireVertex = [&](const Vec3& n, const Vec3& p) {
                        float diffuse = fmaxf(0.0f, Math::Dot(n, lightDir) * -1.0f);
                        float intensity = 0.3f + (0.7f * diffuse);
                        rlColor4ub((unsigned char)(baseColor.r * intensity), (unsigned char)(baseColor.g * intensity), (unsigned char)(baseColor.b * intensity), 255);
                        rlNormal3f(n.x, n.y, n.z); rlVertex3f(p.x, p.y, p.z);
                    };

                    FireVertex(n1, p1); FireVertex(n2, p2); FireVertex(n3, p3);
                    FireVertex(Vec3(-n1.x,-n1.y,-n1.z), p1); FireVertex(Vec3(-n3.x,-n3.y,-n3.z), p3); FireVertex(Vec3(-n2.x,-n2.y,-n2.z), p2);
                }
                rlEnd();
                rlEnableBackfaceCulling(); 

            EndMode3D();

            DrawText(currentScene == 1 ? "1: THE RE4 SPHERE DRAPE" : currentScene == 2 ? "2: THE HIGH-POLY FLAG" : "3: THE WALKING T-SHIRT (GLB Smart-Link)", 20, 15, 20, RAYWHITE);
            DrawText(TextFormat("FINAL SPECS: 65x65 Grid = %d EXACT AAA FACES", triSize/3), 20, 42, 16, GREEN);
            DrawText(hasGLB ? "GLB STATUS: 'character.glb' Mounted & Bound to Rig!" : "GLB STATUS: Default Mannequin (Drop 'character.glb' in folder to upgrade)", 20, 62, 14, YELLOW);
            
            Color fpsC = (GetFPS() >= 80) ? GREEN : (GetFPS() >= 50) ? YELLOW : RED;
            DrawText(TextFormat("FPS: %d", GetFPS()), 1140, 20, 24, fpsC);
        EndDrawing();
    }
    if (hasGLB) UnloadModel(characterGLB);
    CloseWindow();
    return 0;
}