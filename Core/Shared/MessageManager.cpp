 #include "pch.h"
#include "Shared/MessageManager.h"

std::unordered_map<string, string> MessageManager::_enResources = {
	{ "Cheats", u8"Cheats" },
	{ "Debug", u8"Debug" },
	{ "EmulationSpeed", u8"Geschwindigkeit" },
	{ "ClockRate", u8"Taktrate" },
	{ "Error", u8"Fehler" },
	{ "GameInfo", u8"Spielinfo" },
	{ "GameLoaded", u8"Spiel geladen" },
	{ "Input", u8"Eingabe" },
	{ "Patch", u8"Patch" },
	{ "Movies", u8"Filme" },
	{ "NetPlay", u8"Netplay" },
	{ "Overclock", u8"Taktgeschwindigkeit gesteigert" },
	{ "Region", u8"Region" },
	{ "SaveStates", u8"Speicherdateien" },
	{ "ScreenshotSaved", u8"Screenshot erstellt" },
	{ "SoundRecorder", u8"Audio" },
	{ "Test", u8"Test" },
	{ "VideoRecorder", u8"Film" },

	{ "ApplyingPatch", u8"Patch wird angewendet: %1" },
	{ "CheatApplied", u8"1 Cheat angewendet" },
	{ "CheatsApplied", u8"%1 Cheats wurden angewendet" },
	{ "CheatsDisabled", u8"Alle Cheats deaktiviert" },
	{ "CoinInsertedSlot", u8"Eingeworfen! (Slot %1)" },
	{ "ConnectedToServer", u8"Verbindung zum Server hergestellt" },
	{ "ConnectionLost", u8"Verbindung zum Server verloren" },
	{ "CouldNotConnect", u8"Es konnte keine Verbindung zum Server hergestellt werden" },
	{ "CouldNotInitializeAudioSystem", u8"Audio-System konnte nicht initialisiert werden" },
	{ "CouldNotFindRom", u8"Es konnte keine kompatible ROM-Datei gefunden werden (%1)" },
	{ "CouldNotWriteToFile", u8"Die Datei %1 konnte nicht beschrieben werden" },
	{ "CouldNotLoadFile", u8"Die Datei %1 konnte nicht geladen werden" },
	{ "EmulationMaximumSpeed", u8"Maximale Geschwindigkeit" },
	{ "EmulationSpeedPercent", u8"%1%" },
	{ "FdsDiskInserted", u8"Diskette %1 Seite %2 eingelegt" },
	{ "Frame", u8"Frame" },
	{ "GameCrash", u8"Spiel ist gecrashed (%1)" },
	{ "KeyboardModeDisabled", u8"Tastaturmodus deaktiviert" },
	{ "KeyboardModeEnabled", u8"Tastaturmodus aktiviert und Mesen-Shortcuts deaktiviert" },
	{ "Lag", u8"Lag" },
	{ "Mapper", u8"Mapper: %1, Sub-Mapper: %2" },
	{ "MovieEnded", u8"Film beendet" },
	{ "MovieStopped", u8"Film gestoppt" },
	{ "MovieInvalid", u8"Film-Datei nicht korrekt" },
	{ "MovieMissingRom", u8"Zum Abspielen des Films wird ROM (%1) gebraucht" },
	{ "MovieNewerVersion", u8"Filme, die von einer neueren Mesen-Version erstellt wurden, sind nicht kompatibel. Bitte die aktuellste Version herunterladen." },
	{ "MovieIncompatibleVersion", u8"Film ist mit dieser Mesen-Version nicht kompatibel" },
	{ "MoviePlaying", u8"Abgespielter Film: %1" },
	{ "MovieRecordingTo", u8"Mesen-Aufnahme gestartet (%1)" },
	{ "MovieSaved", u8"Mesen-Aufnahme gespeichert (%1)" },
	{ "NetplayVersionMismatch", u8"Netplay-Client verwendet nicht die gleiche Version von Mesen - Verbindung wurde getrennt" },
	{ "NetplayNotAllowed", u8"Diese Aktion ist nicht erlaubt, solange eine Verbindung zu einem Server besteht" },
	{ "OverclockEnabled", u8"Hochtaktung..." },
	{ "OverclockDisabled", u8"Hochtaktung deaktiviert" },
	{ "PrgSizeWarning", u8"PRG ist kleiner als 32kb" },
	{ "SaveStateEmpty", u8"Slot ist leer" },
	{ "SaveStateIncompatibleVersion", u8"Spielstand ist mit dieser Mesen-Version nicht kompatibel" },
	{ "SaveStateInvalidFile", u8"Spielstand-Datei nicht korrekt" },
	{ "SaveStateWrongSystem", u8"Fehler: Spielstand kann nicht geladen werden (falscher Konsolentyp)" },
	{ "SaveStateLoaded", u8"Spiel geladen (Slot %1)" },
	{ "SaveStateLoadedFile", u8"Spiel geladen (Slot %1)" },
	{ "SaveStateSavedFile", u8"Spiel gespeichert (Slot %1)" },
	{ "SaveStateMissingRom", u8"Zum Laden des Spielstands ist ROM %1 erforderlich" },
	{ "SaveStateNewerVersion", u8"Ein Spielstand, der von einer neueren Mesen-Version erstellt wurde, ist nicht kompatibel. Bitte die aktuellste Version herunterladen." },
	{ "SaveStateSaved", u8"Spiel gespeichert (Slot %1)" },
	{ "SaveStateSlotSelected", u8"Slot %1 ausgewählt" },
	{ "ScanlineTimingWarning", u8"PPU-Timing wurde umgestellt" },
	{ "ServerStarted", u8"Server gestartet (Port: %1)" },
	{ "ServerStopped", u8"Server abgeschaltet" },
	{ "SoundRecorderStarted", u8"WAV-Aufnahme gestartet (%1)" },
	{ "SoundRecorderStopped", u8"WAV-Aufnahme gespeichert (%1)" },
	{ "TestFileSavedTo", u8"Test-Datei gespeichert (%1)" },
	{ "UnsupportedMapper", u8"Mapper nicht kompatibel (%1), Spiel kann nicht geladen werden" },
	{ "VideoRecorderStarted", u8"Video-Aufnahme gestartet (%1)" },
	{ "VideoRecorderStopped", u8"Video-Aufnahme gespeichert (%1)" },
};

std::list<string> MessageManager::_log;
SimpleLock MessageManager::_logLock;
SimpleLock MessageManager::_messageLock;
bool MessageManager::_osdEnabled = true;
IMessageManager* MessageManager::_messageManager = nullptr;

void MessageManager::RegisterMessageManager(IMessageManager* messageManager)
{
	auto lock = _messageLock.AcquireSafe();
	if(MessageManager::_messageManager == nullptr) {
		MessageManager::_messageManager = messageManager;
	}
}

void MessageManager::UnregisterMessageManager(IMessageManager* messageManager)
{
	auto lock = _messageLock.AcquireSafe();
	if(MessageManager::_messageManager == messageManager) {
		MessageManager::_messageManager = nullptr;
	}
}

void MessageManager::SetOsdState(bool enabled)
{
	_osdEnabled = enabled;
}

string MessageManager::Localize(string key)
{
	std::unordered_map<string, string> *resources = &_enResources;
	/*switch(EmulationSettings::GetDisplayLanguage()) {
		case Language::English: resources = &_enResources; break;
		case Language::French: resources = &_frResources; break;
		case Language::Japanese: resources = &_jaResources; break;
		case Language::Russian: resources = &_ruResources; break;
		case Language::Spanish: resources = &_esResources; break;
		case Language::Ukrainian: resources = &_ukResources; break;
		case Language::Portuguese: resources = &_ptResources; break;
		case Language::Catalan: resources = &_caResources; break;
		case Language::Chinese: resources = &_zhResources; break;
	}*/
	if(resources) {
		if(resources->find(key) != resources->end()) {
			return (*resources)[key];
		}/* else if(EmulationSettings::GetDisplayLanguage() != Language::English) {
			//Fallback on English if resource key is missing another language
			resources = &_enResources;
			if(resources->find(key) != resources->end()) {
				return (*resources)[key];
			}
		}*/
	}

	return key;
}

void MessageManager::DisplayMessage(string title, string message, string param1, string param2)
{
	if(MessageManager::_messageManager) {
		auto lock = _messageLock.AcquireSafe();
		if(!MessageManager::_messageManager) {
			return;
		}

		title = Localize(title);
		message = Localize(message);

		size_t startPos = message.find(u8"%1");
		if(startPos != std::string::npos) {
			message.replace(startPos, 2, param1);
		}

		startPos = message.find(u8"%2");
		if(startPos != std::string::npos) {
			message.replace(startPos, 2, param2);
		}

		if(_osdEnabled) {
			MessageManager::_messageManager->DisplayMessage(title, message);
		} else {
			MessageManager::Log("[" + title + "] " + message);
		}
	}
}

void MessageManager::Log(string message)
{
	auto lock = _logLock.AcquireSafe();
	if(message.empty()) {
		message = "------------------------------------------------------";
	}
	if(_log.size() >= 1000) {
		_log.pop_front();
	}
	_log.push_back(message);
}

void MessageManager::ClearLog()
{
	auto lock = _logLock.AcquireSafe();
	_log.clear();
}

string MessageManager::GetLog()
{
	auto lock = _logLock.AcquireSafe();
	stringstream ss;
	for(string &msg : _log) {
		ss << msg << "\n";
	}
	return ss.str();
}
