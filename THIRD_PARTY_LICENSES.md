# THIRD-PARTY LICENSES

This project incorporates or depends on the following third-party
components. Each remains under its own license and copyright; nothing here
relicenses them. Full license texts ship inside each component's source
tree or upstream repository.

## Framework submodules (git submodules of this repository)

| Component | Source | License | Copyright |
|---|---|---|---|
| psxrecomp (static recompilation framework + runtime) | github.com/novapowers0/psxrecomp | PolyForm Noncommercial License 1.0.0 | (c) 2026 Matthew Stan |
| recomp-ui (launcher UI framework) | github.com/mstan/recomp-ui | MIT | (c) 2026 Matthew Stanley |

Notes:

- `psxrecomp` is pinned at commit `219a362` (see `framework_pins.txt`).
  Its PolyForm Noncommercial terms apply to the framework itself; this
  game's MIT license does not extend to it. Noncommercial use of the
  combined work should be reviewed against both licenses.
- `recomp-ui` is pinned at `b688ca7`. It depends on Dear ImGui
  (github.com/ocornut/imgui, MIT, (c) Omar Cornut) for its UI layer.

## Libraries inside psxrecomp

| Component | License | Copyright |
|---|---|---|
| recomp-net (netplay/rollback transport) | MIT | recomp-net contributors |
| retcomm-rbengine | MIT | retcomm-rbengine contributors |
| libchrd / libchdr (CHD disc image support) | BSD-style (see `psxrecomp/runtime/licenses/libchdr-NOTICES.txt`) | (c) Romain Tisserand |
| libjuice (WebRTC data channels, via recomp-net) | see upstream | pentresh / contributors |

## Vendored libraries (psxrecomp/recompiler/lib)

| Component | License | Copyright |
|---|---|---|
| ELFIO | MIT | Sergey Satskiy |
| fmt | MIT | Victor Zverovich |
| rabbitizer (MIPS decoder) | MIT | datum contributors |
| toml11 | MIT | Toru Niinae |

## Fetched at build time (CMake FetchContent)

| Component | License |
|---|---|
| SDL3 | zlib |
| zlib | zlib |

System libraries may additionally be used when present (OpenGL, Vulkan
loader/headers, libpulse/ALSA on Linux, etc.) under their respective
licenses.

## Game content (NOT covered by any license in this repository)

Bloody Roar II executables, disc images, and all extracted game data are
third-party copyrighted material. They are never distributed by this
project — see `ASSET_POLICY.md`.
