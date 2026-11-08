# ASSET POLICY

This project follows the same model as other console-recompilation projects
(e.g. the N64Recomp / mstan recomp family): **the software is free, the game
data is not.**

## What this project distributes

The repositories, release archives, and web builds produced by this project
contain ONLY original code, tools, documentation, and configuration:

- The recompiler framework and runtime (`psxrecomp`)
- The launcher UI (`recomp-ui` integration)
- Game-specific mods, plugins, seeds metadata, and build scripts
- Reverse-engineering notes, symbol maps, and agent tooling

## What this project NEVER distributes

Users must supply these files themselves from a legally obtained copy of
*Bloody Roar II* (see `baserom.md` for expected hashes and paths):

- PlayStation disc images (`.bin`/`.cue`, `.iso`, `.chd`)
- The game executable (`SLES_017.22`, `SCUS_944.24`, `SLPS_01842.xx`)
- Extracted CD filesystem contents (`.ANI`, `.ACT`, `.DAT`, audio, video)
- BIOS images (`SCPH*.BIN`) — a compatible open BIOS (OpenBIOS) is bundled
  instead for convenience

None of these files are committed to git (enforced by `.gitignore`) and none
are included in release packages.

## Derived-data boundary

Some project artifacts are derived from game data and are treated as
follows:

| Artifact | Status |
|---|---|
| `seeds/ghidra_funcs*.txt` (function addresses/names) | Facts/addresses only; no expressive code. Reviewed and kept. |
| `catalog_identity*.json`, `disc_probe*.json` | Disc metadata probes (volume IDs, sizes, hashes). Kept. |
| `generated/` recompiled C output | Derivative of the game EXE; **never committed, never distributed** — produced locally per user. |
| Web build character signatures (`char_sigs.json` / embedded `CHARSIGS`) | Short byte fingerprints sampled from `.ANI` streams. Minimal excerpts used only for runtime identification; the full `.ANI` data still requires a user-supplied disc. If stricter compliance is required, these can be regenerated per-user from their own disc. |
| `damage_by_act.json`, `action_clusters.json` (in the RE workspace) | Statistical/structural metadata about move data, not expressive content. |

## Web build note

The WebAssembly build currently expects the user to supply the disc image
(loaded into MEMFS at runtime). A future streaming build must preserve this
model: serve only code and let the client supply its own legally obtained
game data. Do not bundle disc data into any hosted web release.

## Releases

Release packages (see `scripts/package_release*.sh/.ps1`) must be audited
to contain only the items in "What this project distributes" above. Game
executables, disc images, extracted assets, BIOS dumps, and `generated/`
output must never appear in a release archive.
