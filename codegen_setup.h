#ifndef CODEGEN_SETUP_H
#define CODEGEN_SETUP_H

/* Title-side hooks for psxrecomp/host/psxrecomp_codegen_host.
 * Implemented per-region in codegen_setup.c (US/Europe) and
 * codegen_setup_japan.c (Japan/Asia); wired only under
 * -DPSX_SETUP_WIZARD=ON. */

#include "recomp_launcher.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Fill the launcher game-info struct from this title's codegen config. */
void psx_game_codegen_setup_apply(RecompLauncherCGameInfo* gi);
/* After codegen completes, relaunch into the freshly built product exe. */
void psx_game_codegen_relaunch_or_exit(const char* disc_path);
/* If the product build already exists, forward argv to it and exit. */
void psx_game_codegen_forward_if_built(int argc, char** argv);

#ifdef __cplusplus
}
#endif

#endif
