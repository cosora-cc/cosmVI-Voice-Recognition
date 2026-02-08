// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class cosmVI : ModuleRules
{
	public cosmVI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"cosmVI_lib",
				"Projects",
				"Voice"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);


        // =========================================================
        // モデルファイルをビルド先にコピーする設定
        // =========================================================

        // 1. コピー元のモデルディレクトリ (Plugins/cosmVI/Resources/model)
        string ModelSourceDir = Path.Combine(PluginDirectory, "Resources", "model");

        if (Directory.Exists(ModelSourceDir))
        {
            // foreachを使わず、1行でフォルダ全体を再帰的に追加します
            // 第一引数：ターゲット（$(ProjectDir)や$(BinaryOutputDir)を指定するのが一般的ですが、プラグイン内なら以下でOK）
            // 第二引数：ソース（実際のパス）
            RuntimeDependencies.Add("$(BinaryOutputDir)/Resources/model/...", Path.Combine(ModelSourceDir, "..."));
        }

        // =========================================================================
        // コアライブラリをリンクするための設定
        // =========================================================================

        // 1. ThirdPartyライブラリへのパスを取得
        string ThirdPartyPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/cosmVI_lib"));

        // 2. ヘッダーファイルへのインクルードパスを追加
        //    これで、C++コードでインクルードできるようになる
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "include"));

        // 3. Windowsプラットフォームでのみ実行する設定
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // 4. インポートライブラリ(.lib)へのパスを追加
            //    これにより、リンカがDLLの関数を見つけられるようになる
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Win64", "cosmVI_lib.lib"));

            // 5. ランタイムDLLをパッケージに含める設定
            //    ゲーム実行時にDLLが見つかるように、Binaries/Win64 フォルダにコピーするようUBTに指示

            // cosmVI.dll
            string DllPath_cosmVI = Path.Combine(ThirdPartyPath, "lib", "Win64", "cosmVI_lib.dll");
            RuntimeDependencies.Add(DllPath_cosmVI);

            // libvosk.dll (依存DLLも忘れずに！)
            string DllPath_vosk = Path.Combine(ThirdPartyPath, "lib", "Win64", "libvosk.dll");
            RuntimeDependencies.Add(DllPath_vosk);
        }

        // ============================================================
        // OSによる自動ロードを止め、StartupModuleでの手動ロードまで待機させる設定
        // ============================================================
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDelayLoadDLLs.Add("cosmVI_lib.dll");
        }

        // 3. Linuxプラットフォームでのみ実行する設定
        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            // 4. インポートライブラリ(.lib)へのパスを追加
            //    これにより、リンカがDLLの関数を見つけられるようになる
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Linux", "libcosmVI_lib.so"));

            // 5. ランタイムsoをパッケージに含める設定
            //    ゲーム実行時にsoが見つかるように、Binaries/Linux フォルダにコピーするようUBTに指示

            // cosmVI.dll
            string SoPath_cosmVI = Path.Combine(ThirdPartyPath, "lib", "Linux", "libcosmVI_lib.so");
            RuntimeDependencies.Add(SoPath_cosmVI);

            // libvosk.so (依存SOも忘れずに！)
            string SoPath_vosk = Path.Combine(ThirdPartyPath, "lib", "Linux", "libvosk.so");
            RuntimeDependencies.Add(SoPath_vosk);
        }

        // ============================================================
        // OSによる自動ロードを止め、StartupModuleでの手動ロードまで待機させる設定
        // ============================================================
        //if (Target.Platform == UnrealTargetPlatform.Linux)
        //{
        //    PublicDelayLoadDLLs.Add("cosmVI_lib.dll");
        //}
    }
}
