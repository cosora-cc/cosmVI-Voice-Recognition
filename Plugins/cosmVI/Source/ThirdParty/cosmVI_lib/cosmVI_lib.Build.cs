// WINDOWS 対応しかしていない !!!!!

using System.IO;
using UnrealBuildTool;

public class cosmVI_lib : ModuleRules
{
    public cosmVI_lib(ReadOnlyTargetRules Target) : base(Target)
    {
        // これは外部モジュール（コンパイル不要）であることを宣言
        Type = ModuleType.External;

        // このファイルの場所を基準にする
        string BaseDir = Path.GetFullPath(ModuleDirectory);

        // 1. ヘッダーファイルの場所を公開
        // SetEnvはこれで防げるらしい。（不思議）
        PublicIncludePaths.Add(Path.Combine(BaseDir, "include"));
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // 2. .libファイルのリンク設定
            PublicAdditionalLibraries.Add(Path.Combine(BaseDir, "lib", "Win64", "cosmVI_lib.lib"));

            // 3. .dllファイルのコピー設定 (Binariesフォルダへ)
            string DllName = "cosmVI_lib.dll";
            string DllPath = Path.Combine(BaseDir, "lib", "Win64", DllName);
            RuntimeDependencies.Add("$(PluginDir)/Binaries/Win64/" + DllName, DllPath);

            // VoskのDLLも忘れずに
            string VoskDllName = "libvosk.dll";
            string VoskDllPath = Path.Combine(BaseDir, "lib", "Win64", VoskDllName);
            RuntimeDependencies.Add("$(PluginDir)/Binaries/Win64/" + VoskDllName, VoskDllPath);
        }
        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            // 2. .libファイルのリンク設定
            PublicAdditionalLibraries.Add(Path.Combine(BaseDir, "lib", "Linux", "libcosmVI_lib.so"));

            // 3. .dllファイルのコピー設定 (Binariesフォルダへ)
            string DllName = "libcosmVI_lib.so";
            string DllPath = Path.Combine(BaseDir, "lib", "Linux", DllName);
            RuntimeDependencies.Add("$(PluginDir)/Binaries/Linux/" + DllName, DllPath);

            // VoskのDLLも忘れずに
            string VoskDllName = "libvosk.so";
            string VoskDllPath = Path.Combine(BaseDir, "lib", "Linux", VoskDllName);
            RuntimeDependencies.Add("$(PluginDir)/Binaries/Linux/" + VoskDllName, VoskDllPath);
        }
    }
}