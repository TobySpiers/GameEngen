# GameEngen — Claude Brief

## Project Overview
A rendering testbed built on OpenGL 4.6 Core Profile, intended for experimenting with compute shaders and raymarching, with potential to grow into a simple 3D game.

## Coding Standards

### Naming
- **PascalCase** for class and struct names, and for all function/method names
- **camelCase** for variables and parameters
- **Booleans** are always prefixed with a lowercase `b` (e.g. `bTickEnabled`, `bPendingDestroy`, `bIsGrounded`)

### Braces
- Curly braces are **always** used for if statements, for loops, while loops, and any other control flow — no single-line braceless bodies
- Curly braces **always** occupy their own line

```cpp
// Correct
if (condition)
{
    DoSomething();
}

// Wrong
if (condition) DoSomething();
if (condition)
    DoSomething();
```

## Architecture

### Folder Structure
```
GameEngen/
  Audio/
    SoundManager.h / .cpp
  Core/
    Flowstate.h             ← abstract base class
    FlowstateManager.h/.cpp
    ServiceLocator.h / .cpp
  Flowstates/
    GameFlowstate.h / .cpp  ← concrete states live here
  Rendering/
    Renderer.h / .cpp
    Shader.h   / .cpp
    Texture.h  / .cpp
  GameEngen.cpp       ← entry point; platform init only
Assets/
  shaders/
  sprites/
  sounds/
  music/
third_party/                ← all external libraries, not managed by vcpkg
```

### Coordinate System
- **(0, 0) is always bottom-left** — in screen space, UV space, and texture data
- `stbi_set_flip_vertically_on_load(true)` is set in `Texture.cpp` and must stay — it is a fundamental requirement for correct OpenGL texture loading, not a compensating hack
- The orthographic projection uses `glm::ortho(0, width, 0, height, -1, 1)` (bottom-left origin)

## Third-Party Libraries
All libraries live under `third_party/` and are linked manually via the `.vcxproj`. No vcpkg.

| Library | Purpose |
|---|---|
| GLAD | OpenGL 4.6 Core function loader |
| GLFW | Windowing and input |
| GLM | Vector/matrix math |
| Dear ImGui | Debug UI |
| stb_image | Texture loading (single-header) |
| OpenAL Soft | Audio playback (`OpenAL32.dll` router + `soft_oal.dll` implementation) |
| dr_wav | WAV decoding (single-header) |
| dr_mp3 | MP3 decoding (single-header) |

### Single-Header Implementation Macros
Each of these must be defined in **exactly one** `.cpp` file:
- `#define STB_IMAGE_IMPLEMENTATION` — in `Rendering/Texture.cpp`
- `#define DR_WAV_IMPLEMENTATION` — in `Audio/SoundManager.cpp`
- `#define DR_MP3_IMPLEMENTATION` — in `Audio/SoundManager.cpp`

## GameObject System (`Core/GameObject`, `Core/GameObjectManager`)
- `GameObject` is the abstract base for all game entities. Virtual lifecycle methods: `OnSpawn()`, `OnTick(float deltaTime)`, `OnDraw(Renderer& renderer)`, `OnDestroy()`
- Each object has a `Transform` (position, rotation as Euler angles, scale — all `glm::vec3`) and three bool flags: `bTickEnabled`, `bDrawEnabled`, `bPendingDestroy`
- `SetTickEnabled` / `SetDrawEnabled` notify the `GameObjectManager` immediately via `ServiceLocator::GetGameObjectManager()`, so the internal lists stay in sync without a back-pointer per object
- `Destroy()` sets `bPendingDestroy = true` and calls `RequestDestroy` on the manager; actual removal is deferred until `FlushDestroyQueue()` at the end of `Tick` and `Draw`
- `GameObjectManager` owns all objects via `vector<unique_ptr<GameObject>>` and maintains separate `tickableObjects` and `drawableObjects` raw-pointer lists for O(M) iteration
- Both `Tick` and `Draw` iterate a **snapshot copy** of their respective list so mid-frame calls to `SetTickEnabled`, `SetDrawEnabled`, or `Destroy` are safe
- `Spawn<T>(args...)` is a templated factory: constructs T, adds to `allObjects`, populates tick/draw lists, then calls `OnSpawn()`. Has a `static_assert` to enforce T derives from `GameObject`
- `GameObjectManager::Tick(dt)` and `Draw(renderer)` are called from `FlowstateUpdate` — flowstates decide when to run the object loop

## Service Locator (`Core/ServiceLocator`)
- Provides global access to `Renderer`, `SoundManager`, and the GLFW window pointer
- Services are **lazily constructed** on first access — `GetRenderer()` creates the renderer with `"shaders/"`, `GetSoundManager()` creates it with default `maxSources`
- `ProvideWindow(GLFWwindow*)` must be called once in `main` after the window is created
- `Shutdown()` must be called in `main` **after** the `FlowstateManager` is destroyed, so that `FlowstateExit` can still safely use services during teardown

## Flowstate Pattern (`Core/Flowstate`, `Core/FlowstateManager`)
- `Flowstate` is an abstract base with `FlowstateEnter()`, `FlowstateUpdate(float deltaTime)`, and `FlowstateExit()`; only `FlowstateUpdate` is pure virtual
- `FlowstateManager::SwitchTo()` queues a transition — the actual switch (Exit → Enter) is deferred to the top of the next `Update()` so the current state is never destroyed mid-execution
- The `FlowstateManager` destructor calls `FlowstateExit` on the active state
- In `main`, the `FlowstateManager` lives inside a scope block `{ }` so it is destroyed (and `FlowstateExit` fires) before `ServiceLocator::Shutdown()` is called
- No game logic belongs in `GameEngen.cpp` — only platform/framework boilerplate (GLFW init, GLAD, ImGui, the loop shell)
- Concrete flowstates live in `Flowstates/`

## Audio Design
- **One-shot sounds** (WAV): fully decoded into an OpenAL buffer via `dr_wav`. Managed by `SoundManager::LoadSound()` / `Play()`. Uses a pooled source list capped at `maxSources` (default 32); when the cap is reached the oldest playing source is stolen.
- **Music** (MP3): streamed from disk via `dr_mp3` using a 3-buffer ring queue. Managed by `SoundManager::LoadMusic()` / `PlayMusic()` / `StopMusic()`. Requires `SoundManager::Update()` to be called once per frame.
