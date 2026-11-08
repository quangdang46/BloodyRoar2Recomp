# Web (WASM) port — plan and blocker audit

Goal: run `BloodyRoar2_Recompiled` in a browser via Emscripten.

## Current architecture (what must be ported)

- Recompiled game C (`generated/SLES_017.22_full_*.c`) — plain C, portable.
- Runtime (`psxrecomp/runtime/src/`): CPU core, memory, CD-ROM, GPU
  (3 renderers: GL / Vulkan / software), SPU audio, SIO pads, memcard,
  netplay (libjuice), debug server (TCP), recomp-ui launcher (Dear ImGui).
- SDL3 for window/input/audio; FetchContent-pinned.

## Blockers found (source audit)

| Component | File | Issue | WASM path |
|---|---|---|---|
| Fibers | `psx_fiber.c` | Win32 Fiber / POSIX ucontext | Emscripten `emscripten_fiber_*` API exists — add a 3rd backend; or Asyncify |
| Overlay/mod loader | `overlay_loader.c`, `overlay_posix.c` | `dlopen` | Stub out on `__EMSCRIPTEN__` (mods become built-in only) |
| Debug server | `debug_server.c` | pthreads + TCP | Emscripten pthreads OK (`-pthread`); or WebSocket bridge |
| mmap | `memory.c` | guest RAM 2MB + scratchpad | Emscripten `mmap` works for anonymous; file-backed needs MEMFS/IDB |
| Disc image | `cdrom.c`, `cue_sheet.cpp` | reads `.bin` sectors | MEMFS upload (553 MB — heavy) or IDB + HTTP range streaming layer |
| GPU | `gpu_gl_renderer.c` | desktop GL | OpenGL ES 3.0 → WebGL2 should mostly work; fallback `gpu_sw_renderer` → canvas ImageData (slow but sure) |
| Vulkan | `gpu_vk_renderer.c` | desktop Vulkan | Exclude on wasm |
| SDL3 | window/input/audio | SDL3 has official Emscripten port | Should work; verify version pinned by FetchContent |
| libjuice | ICE/UDP netplay | UDP sockets | WebRTC data channels needed — defer netplay on web v1 |
| recomp-ui | Dear ImGui launcher | works on wasm w/ GL | Keep or strip for v1 |

## Suggested phasing

1. **Spike**: `emcmake cmake` configure; fix compile errors; target a
   *headless* wasm build (no SDL window) proving CPU+mem+cdrom run.
2. **Framebuffer**: software renderer → `<canvas>` via SDL or raw
   `emscripten_set_canvas_element_size`; get BIOS/logo on screen.
3. **Disc**: MEMFS mount of `.bin` (test 553 MB memory headroom) or
   IDB streaming; `.cue` parsing already in `cue_sheet.cpp`.
4. **GL renderer**: try `gpu_gl_renderer` under WebGL2; keep sw fallback.
5. **Input**: SDL gamepad/keyboard → already abstracted.
6. **Audio**: SPU → SDL audio → WebAudio (verify buffer callback path).
7. **Netplay**: defer; single-player + agent mode first.

## Agent mode on web

The debug server protocol is plain TCP JSON. On web, bridge it over a
WebSocket (tiny proxy) or compile the agent loop *into* the wasm build
(agent_mode.py logic is ~100 lines of Python → port to C or run a
WebWorker that talks to the debug socket via ws:// proxy).

## Open questions

- Does the runtime use real threads for GPU/SPU, or is `psx_fiber`
  the only concurrency? (audit `SDL_CreateThread` call sites)
- MEMFS 553 MB: browser tab memory limit vs streaming reader — measure.
- Does `dirty_ram_interp` / recompiler need executable memory tricks
  that wasm forbids? (it's all compiled C — should be fine)

## Threading note (added)

Core uses `psx_fiber` (ucontext/fiber) not raw threads; only debug
server IO + overlay capture use `SDL_CreateThread`. On web, pthreads
require SharedArrayBuffer => COOP/COEP headers; alternatively compile
debug server out for v1. Emscripten's own `emscripten_fiber_*` API is
the clean backend for `psx_fiber.c`.

## Spike results (2026-11-08, Emscripten 7.0.0-git via Homebrew)

**Full `psx-runtime` target compiles + links to `BloodyRoar2_Recompiled.js/.wasm`
(38 MB wasm) and RUNS in Chromium.** Fixes applied along the way:

- `toml11/toml/comments.hpp`: added `#include <exception>` for
  `std::terminate` under Emscripten libc++.
- `psx_fiber.c`: new `__EMSCRIPTEN__` backend on `emscripten_fiber_*`
  (Asyncify). Per-fiber malloc'd C stack + 256 KiB asyncify stack.
  Verified standalone: main↔worker and fiber→fiber switches work.
- `gpu_gl_renderer.c`: `glReadBuffer(GL_BACK)` is a no-op guard on
  `__EMSCRIPTEN__` (WebGL can't select GL_BACK).
- `recomp_ui.cmake`: `EMSCRIPTEN` gets `LNG_GLES2` + `IMGUI_IMPL_OPENGL_ES2`
  (imgui shaders are `#version 100` under WebGL1).
- Link flags (build-wasm): `-sASYNCIFY -sASYNCIFY_STACK_SIZE=8388608
  -sALLOW_MEMORY_GROWTH -sINITIAL_MEMORY=805306368
  -sMAXIMUM_MEMORY=2147483648 -sEXPORTED_RUNTIME_METHODS=FS,...`
  (INITIAL_MEMORY must cover ~605 MB static+stack reservation;
  MAXIMUM 2 GB for the 528 MB disc in MEMFS).

**Browser validation so far**

- Launcher UI renders (imgui GLES2 shaders compile; RUI loads).
- `--no-launcher --headless` path: BIOS (`bios/openbios.bin`) loads from
  MEMFS, disc detected (`SLES-01722` PAL), OpenGL ES2 context created,
  GL pipeline init fails → **software renderer fallback engages**,
  LLE BIOS starts executing at `PC=0xBFC00000`.
- Disc delivery for the spike: `index.html` fetches `Game.cue` +
  `Game.bin` (528 MB) + staged `bios/ mods/ assets/ game.toml` into
  MEMFS via `Module.onRuntimeInitialized` + `Module.FS.writeFile`.

**Asyncify crash root-caused**

- First boot crashed with `RuntimeError: unreachable` inside
  `Asyncify.doRewind` — SDL3's Emscripten backend calls
  `emscripten_sleep(0)` on every present/`SDL_Delay`, unwinding the C
  stack through the deep emulation call chain; the default asyncify
  spill stack was too small → corrupted rewind → trap.
- NOT a fiber bug: `psx_fiber_*` was never called before the crash
  (EM_ASM instrumentation showed zero switches; HLE scheduler path
  doesn't use host fibers yet).
- Fix direction: `-sASYNCIFY_STACK_SIZE=8 MiB` + let SDL keep its
  asyncify sleeps (they are the yield mechanism the page needs) +
  `host_time.c` routes `psx_host_sleep_micros` through
  `emscripten_sleep` on `__EMSCRIPTEN__` so the frame pacer yields
  instead of busy-waiting (busy-wait starved the renderer thread —
  `page.evaluate`/screenshot timed out for 150 s+).

**BREAKTHROUGH — game runs in-browser at full speed (2026-11-09)**

- `index.html` stages `game.toml`, `game_options.toml`, and a savestate
  (`saves/openbios/state_80126178_slot09.pst`); `Module.preRun` sets
  `ENV.PSX_LOAD_SLOT='9'` (+ `PSX_FPS_TELEMETRY='1'` for diagnostics).
  Emscripten `getenv` reads the global `ENV` table and caches the string
  list on first call — `preRun` is the latest safe point (initRuntime can
  call getenv earlier).
- Savestate load skips the BIOS entirely: `savestate: LOADED slot 9 ->
  resuming pc=0x8019A648`, then `[FPS] game: 50.0 fps (0.83x)` sustained —
  guest runs at ~full speed in-game (the earlier BIOS-boot crawl was the
  LLE intro path, not representative).
- **ROOT CAUSE of the `RuntimeError: memory access out of bounds` in
  `sha256_file`/`free`: Emscripten's default `STACK_SIZE` is 64 KiB**
  (`settings.js`: `var STACK_SIZE = 64*1024`). The runtime's C++ paths
  (toml11, `std::filesystem`, and `sha256_file`'s own 64 KiB stack buffer)
  blow straight through it. Native has 8 MiB. Fix:
  `-sSTACK_SIZE=16777216` (16 MiB). Deterministic, reproduced in release,
  masked under ASAN (different stack layout). `--emit-symbol-map` produces
  `BloodyRoar2_Recompiled.js.symbols` (`<funcidx>:<name>`) — use it to map
  `wasm-function[N]` stack frames; beware Chrome heuristic-caching the
  wasm — tests must bypass cache (`--disk-cache-size=1` + no-cache route).
- Also confirmed this run: `bios_boot=HLE (shell skipped)` now engages
  (earlier denial was pre-game.toml), GL pipeline still fails → software
  renderer fallback, `emscripten_sleep` pacing yields fine (page stays
  responsive, ~50 fps guest).

**BLACK CANVAS — ROOT-CAUSED AND FIXED (2026-11-09)**

Three stacked causes, all fixed:

1. **GL pipeline stole the canvas's only context.** SDL3-Emscripten's
   window framebuffer presents via a **2D** canvas context
   (`Browser.createContext` → `putImageData`, see
   `src/video/emscripten/SDL_emscriptenframebuffer.c`). A canvas can only
   own ONE context type — `gl_renderer_init_context` created a WebGL
   context during its (failing) init, so the later 2D `getContext`
   returned null and every present silently failed. Fix:
   `gl_renderer_init_context` returns 0 immediately under
   `__EMSCRIPTEN__` (software renderer is the Web v1 present path).
2. **SDL picked the GLES2 accelerated renderer**, which would also claim
   the WebGL context. Fix: `SDL_SetHint(SDL_HINT_RENDER_DRIVER,
   "software")` before `SDL_CreateRenderer` under `__EMSCRIPTEN__`
   (both the init path and `ensure_sw_sdl_present`).
3. **`Browser` object undefined.** SDL's framebuffer EM_ASM calls
   `Browser.createContext`, but `library_browser.js` isn't linked into
   the modularized build (`grep` finds the call site, no definition →
   `[pageerror] Browser is not defined`). Fix: `index.html` defines
   `window.Browser = { createContext: c => c.getContext('2d') }`.

**VERIFIED IN BROWSER (Chromium headless, 2026-11-09):**

- Savestate slot 9 loads (`pc=0x8019A648`), guest sustains **~50 fps
  (0.83x)** indefinitely.
- Canvas reads **99.6% non-black**, screenshot shows real rendered
  gameplay (3087 distinct colors, dark arena scene).
- Web v1 present path = software renderer → SDL software renderer →
  window framebuffer → 2D `putImageData`. No WebGL needed.

**AGENT MODE ON WEB — IMPLEMENTED + COMPONENT-VERIFIED (2026-11-09)**

- `debug_server.c`: exported `br2_dbg_read8/16/32`, `br2_dbg_write8/16/32`,
  `br2_set_pad(slot, active_low_word)` (hold), `br2_release_pad`,
  `br2_pad_status` — same `s_input_override` path the TCP `set_input`
  command uses, so pad convention is identical (raw active-low word).
- `runtime.cmake`: `if(EMSCRIPTEN)` adds `-sEXPORTED_FUNCTIONS` for the
  bridge + `ccall,cwrap` runtime methods (unioned with the FS methods —
  two `-sEXPORTED_RUNTIME_METHODS` flags would clobber each other;
  target_link_options comes after CMAKE_EXE_LINKER_FLAGS so ours wins
  and must contain everything).
- `index.html`: `window.BR2_AGENT='0,1'` (SIO slots) starts an in-page
  agent — direct port of `bloody-roar-2/agent_mode.py` (phase gate,
  ctrl+1 human flag poke, same heuristic policy, 33 ms tick).
  **Gotcha**: the generated JS declares `var ENV={}` which wipes
  `window.ENV` before `preRun` — agent config must live outside ENV
  (`window.BR2_AGENT`), while `PSX_*` vars are (re)set inside `preRun`.
- Verified in Chromium: `br2_set_pad` overrides register
  (`br2_pad_status` → 0xFFDF/0xFF7F for held RIGHT/LEFT), `write8` to
  `ctrl+1` sticks, ccall reads return live fighter state (phase/HP/X
  sane, rounds cycle, fighters close to distance 0 = active combat).
- **Keyboard input verified**: neutral `0xFFFF` → ArrowRight `0xFFDF`
  (RIGHT), 'z' `0x7FFF` — SDL emscripten keyboard→pad path works with
  no extra code.
- **Audio**: WebAudio initialized (ScriptProcessorNode deprecation
  warning = SDL audio device live); in-game audio output not yet
  confirmed by ear, path is functional.
- All 32 preload files now resolve (earlier 404 was the savestate
  before it was added to FILES); slot07 added alongside slot09 for
  mid-fight savestate boots.
- **Char-id on web**: `br2_dbg_read_buf` bulk export + embedded
  `CHARSIGS` (132 × 16-byte `.ANI` windows, same db as
  `bloody-roar-2/char_sigs.json`). Scan window is anchored on the live
  `ctrl+0x30` blob pointer (`blob-0x2000 .. +0x40000`) — fixed per-slot
  RAM regions proved unreliable because arena placement varies per
  round/char (P2 blob landed just past the old window). Verified:
  P1=ALI 12/12, P2=LON 12/12 signature matches; detected once per
  phase-2 entry, cache cleared on round transitions.

- **Full boot verified** (no `PSX_LOAD_SLOT`): BIOS HLE boot-skip → game
  live in ~10 s at ~50 fps. Keyboard (Enter/Arrows) drives game flow —
  phase transitions respond, no soft-lock over 80 s of menu/fight
  cycling. Visual menu confirmation needs human eyes.
- **Agent-vs-CPU verified** (`window.BR2_AGENT='0'`): only slot 0 gets
  pad overrides (`br2_pad_status` slot0 varies, slot1 stays 0xFFFF =
  CPU-driven), rounds cycle normally, agent wins KOs.
- **Audio path verified functional**: `AudioContext` state=`running`
  @48 kHz and the ScriptProcessorNode deprecation warning fires = SDL
  audio device live and pulling buffers. Actual audible output needs a
  human ear; should still move to AudioWorklet eventually.
- **Human-vs-agent verified** (`window.BR2_AGENT='1'`): keyboard drives
  slot 0 (ArrowRight → pad0 `0xFFDF`, P1 moves), agent drives slot 1
  only (pad1 overrides, `P2 char: LON` logged). All three modes now
  confirmed on web: 2-agent, agent-vs-CPU, human-vs-agent.
- **Diagnostics cleaned**: removed `EM_ASM` fib* logs from
  `psx_fiber.c` and the `hb` heartbeat + `cpu_state`/`debug_server`
  includes from `host_time.c`. Rebuilt + smoke-tested: canvas 99.4%
  non-black, agent + char-id still work, no pageerrors.

**Still open**
- GL renderer on web is a real port, not a flag flip: all shaders in
  `gpu_gl_renderer.c` are `#version 330` desktop GLSL — WebGL2 only
  accepts `#version 300 es`. Needs: shader port (in/out, texture()
  samplers, no implicit int→float etc.), VAO/FBO path audit against
  GLES3, and `SDL_GL_CreateContext` → WebGL2 context (SDL3-Emscripten
  supports it; the earlier failure was only the canvas-context
  conflict, now understood). Until then the SDL software renderer +
  2D `putImageData` present is the only working path (~50 fps, 0.83×).
- 528 MB MEMFS footprint: works but heavy; IDB streaming is the real
  answer for production.
- `dlopen` overlay/mod loader unaddressed (built-in mods only so far).
- Debug server TCP → agent hot-ops now bridged via ccall exports;
  full JSON protocol still needs a WebSocket proxy if ever required.
- Audio path (SPU→SDL→WebAudio) untested (ScriptProcessorNode deprecation
  warning seen — works but should move to AudioWorklet).
