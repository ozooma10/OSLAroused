#pragma once

// The libxse fork of CommonLibSSE dropped RE::DebugNotification (CommonLibSSE-NG
// declared it alongside DebugMessageBox in RE/M/Misc.h). This re-adds it with
// NG's implementation: resolve the game's HUD-notification function by address
// and forward to it. RELOCATION_ID maps to the AE id under this fork.
namespace RE
{
	inline void DebugNotification(const char* a_notification, const char* a_soundToPlay = nullptr, bool a_cancelIfAlreadyQueued = true)
	{
		using func_t = void (*)(const char*, const char*, bool);
		REL::Relocation<func_t> func{ RELOCATION_ID(52050, 52933) };
		func(a_notification, a_soundToPlay, a_cancelIfAlreadyQueued);
	}
}
