// Copyright Epic Games, Inc. All Rights Reserved.

#include "cosmVI.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

#include "cosmVI_lib.h" 

#define LOCTEXT_NAMESPACE "FcosmVIModule"

// ログカテゴリの定義（ログを見やすくするため）
DEFINE_LOG_CATEGORY_STATIC(LogCosmVI, Log, All);

void FcosmVIModule::StartupModule()
{
    FString BaseDir = IPluginManager::Get().FindPlugin("cosmVI")->GetBaseDir();

    // パス
    FString LibraryPath;
    FString VoskPath;

#if PLATFORM_WINDOWS
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/cosmVI_lib/lib/Win64/cosmVI_lib.dll"));

    VoskPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/cosmVI_lib/lib/Win64/libvosk.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/cosmVI_lib/lib/Mac/libcosmVI_lib.dylib"));
#endif

    // 1. libvosk.dllのロード
    if (FPaths::FileExists(VoskPath))
    {
        voskHandle = FPlatformProcess::GetDllHandle(*VoskPath);
        if (voskHandle)
        {
            UE_LOG(LogCosmVI, Log, TEXT("Dependencies loaded: libvosk.dll"));
        }
        else
        {
            UE_LOG(LogCosmVI, Error, TEXT("Failed to load dependency: libvosk.dll at %s"), *VoskPath);
        }
    }
    else
    {
        UE_LOG(LogCosmVI, Warning, TEXT("Dependency not found: libvosk.dll at %s"), *VoskPath);
    }

    // 2. cosmVI_lib.dllのロード
    if (FPaths::FileExists(LibraryPath))
    {
        cosmVIHandle = FPlatformProcess::GetDllHandle(*LibraryPath);

        if (cosmVIHandle)
        {
            // 成功ログ
            UE_LOG(LogCosmVI, Log, TEXT("cosmVI_lib.dll loaded successfully from %s"), *LibraryPath);

            // FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("Success", "cosmVI Loaded!")); // あるとまぁまぁ邪魔。私は嫌いかも。止まるのが嫌だ
        }
        else
        {
            // 失敗?!?!?!?!
            UE_LOG(LogCosmVI, Error, TEXT("Failed to load cosmVI_lib.dll! Handle is null. Check dependencies."));
            FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("Error", "Failed to load cosmVI_lib.dll. Check Output Log for details."));
        }
    }
    else
    {
        // ファイルが見つからないとかそんな事は今時起こるのかな？
        UE_LOG(LogCosmVI, Error, TEXT("cosmVI_lib.dll NOT FOUND at %s"), *LibraryPath);
        FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("Error", "cosmVI_lib.dll file not found. Check path."));
    }
}

void FcosmVIModule::ShutdownModule()
{
    // ハンドルの解放
    if (cosmVIHandle)
    {
        FPlatformProcess::FreeDllHandle(cosmVIHandle);
        cosmVIHandle = nullptr;
        UE_LOG(LogCosmVI, Log, TEXT("cosmVI_lib.dll unloaded."));
    }

    if (voskHandle)
    {
        FPlatformProcess::FreeDllHandle(voskHandle);
        voskHandle = nullptr;
        UE_LOG(LogCosmVI, Log, TEXT("libvosk.dll unloaded."));
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FcosmVIModule, cosmVI)