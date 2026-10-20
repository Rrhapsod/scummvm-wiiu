# Native Wii U port — chat entry point

For Wii U/Aroma work, first read
`backends/platform/sdl/wiiu/SESSION_CONTEXT.md` completely.
In the local nested workspace, also read `../../AGENTS.md` when present;
`../../docs/WIIU_PORT_PLAN.md` holds the detailed historical evidence.
For a standalone clone without those outer files, use the context document,
README.md and backend README.WIIU, and identify missing local evidence.

Use `$scummvm-wiiu-context` if available; these file routes also work without
the skill. Inspect Git status/branch/HEAD before edits. Treat the current user
request as the task, not the historical plan's "first task" or "next step."
Do not assume a new chat knows previous messages.

Preserve the native SDL2/WUT architecture; no RetroArch dependency. Keep public
Wii U documentation in English and first person for Rrhapsod's own authorship
and tests. Preserve the ScummVM application name/icon and existing user data.
Separate host/build results from hardware validation, and local packaging from
publication. No automatic push, tag, upload or release replacement.

After authorized implementation or handoff work, refresh SESSION_CONTEXT.md
with verified facts and remaining work. Keep detailed history outside that
short entry point. Do not rewrite existing release assets just to refresh it.
These Wii U-specific directions do not select the task for unrelated ports.
