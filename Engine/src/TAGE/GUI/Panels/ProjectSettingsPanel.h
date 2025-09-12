#pragma once

namespace TAGE {
	class ProjectSettingsPanel
	{
	public:
		void OnImGuiRender();

		void Open() { _IsOpen = true; }
		bool IsOpen() const { return _IsOpen; }
	private:
		enum class Settings {
			Graphics, StartupScene
		} _CurrentSetting = Settings::Graphics;
	private:
		bool _IsOpen = false;
		void DrawSettings(Settings setting);

		void DrawGraphicsSettings();
		void DrawScreenSpaceSettings();
		void DrawStartupSceneSettings();
	};
}