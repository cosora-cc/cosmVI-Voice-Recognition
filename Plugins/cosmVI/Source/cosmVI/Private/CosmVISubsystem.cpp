// Fill out your copyright notice in the Description page of Project Settings.


#include "CosmVISubsystem.h"
#include "Misc/Paths.h"
#include "VoiceModule.h"
#include "Interfaces/IPluginManager.h"

// --- ヘルパー関数 ---
FString UCosmVISubsystem::GetModelFolderName(ECosmVILanguage Language)
{
    switch (Language)
    {
    case ECosmVILanguage::Japanese:
        return TEXT("vosk-model-small-ja-0.22");
    case ECosmVILanguage::English:
        return TEXT("vosk-model-small-en-us-0.15");
	case ECosmVILanguage::Italian:
		return TEXT("vosk-model-small-it-0.22");
	case ECosmVILanguage::French:
		return TEXT("vosk-model-small-fr-0.22");
	case ECosmVILanguage::Deutshch:
		return TEXT("vosk-model-small-de-0.15");
	//case ECosmVILanguage::Korean:
	//	return TEXT("vosk-model-small-ko-0.22");
	case ECosmVILanguage::Chinese:
		return TEXT("vosk-model-small-cn-0.22");
    default:
        return TEXT("vosk-model-small-ja-0.22");
    }
}

void UCosmVISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // ハンドル作成
    if (cosmVI_Create(&Handle) != COSMVI_SUCCESS)
    {
        UE_LOG(LogTemp, Error, TEXT("CosmVI: Failed to create instance."));
    }
}

void UCosmVISubsystem::Deinitialize()
{
    StopRecognition(); // 念のため停止

    // マイク解放
    if (VoiceCapture.IsValid())
    {
        VoiceCapture->Shutdown();
        VoiceCapture.Reset();
    }

    // ライブラリ解放
    if (Handle)
    {
        cosmVI_Destroy(&Handle);
        Handle = nullptr;
    }

    Super::Deinitialize();
}

bool UCosmVISubsystem::InitializeCosmVI(ECosmVILanguage Language)
{
    FString ModelFolderName = GetModelFolderName(Language);
    // ハンドルがなければ作成
    if (!Handle)
    {
        if (cosmVI_Create(&Handle) != COSMVI_SUCCESS) return false;
    }

    // --- モデルパス解決 (引数を使用) ---
    FString ModelPath;
    FString PluginBaseDir = IPluginManager::Get().FindPlugin("cosmVI")->GetBaseDir();
    FString PathInPlugin = FPaths::Combine(PluginBaseDir, TEXT("Resources/model"), ModelFolderName);
    FString ExeBaseDir = FPlatformProcess::BaseDir();
    FString PathInBinary = FPaths::Combine(ExeBaseDir, TEXT("Resources/model"), ModelFolderName);

    // 存在確認
    if (FPaths::DirectoryExists(PathInPlugin))
    {
        ModelPath = FPaths::ConvertRelativePathToFull(PathInPlugin);
    }
    else if (FPaths::DirectoryExists(PathInBinary))
    {
        ModelPath = FPaths::ConvertRelativePathToFull(PathInBinary);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CosmVI: Model folder '%s' NOT FOUND."), *ModelFolderName);
        return false;
    }

    // --- VoiceCaptureの作成---
    if (!VoiceCapture.IsValid()) {
        VoiceCapture = FVoiceModule::Get().CreateVoiceCapture("");
    }
    int32 SampleRate = 16000;

    // --- ライブラリ初期化 ---
    if (cosmVI_Initialize(Handle, TCHAR_TO_UTF8(*ModelPath), (float)SampleRate) == COSMVI_SUCCESS)
    {
        const UEnum* EnumPtr = StaticEnum<ECosmVILanguage>();
        UE_LOG(LogTemp, Log, TEXT("CosmVI: Initialized with language: %s"), *EnumPtr->GetNameStringByValue((int64)Language));

        // 言語を変えたら、文法設定もリセットしておくのが安全
        cosmVI_SetGrammar(Handle, "");

        // ノイズゲート無効化設定も忘れずに
        if (GEngine) {
            GEngine->Exec(NULL, TEXT("Voice.MicNoiseGateThreshold 0.0"));
            GEngine->Exec(NULL, TEXT("Voice.SilenceDetectionThreshold 0.0"));
        }

        return true;
    }

    return false;
}

bool UCosmVISubsystem::SwitchLanguage(ECosmVILanguage NewLanguage)
{
    StopRecognition();

    if (Handle)
    {
        cosmVI_Destroy(&Handle);
        Handle = nullptr;
    }

    return InitializeCosmVI(NewLanguage);
}

bool UCosmVISubsystem::StartRecognition()
{
    // 1. ハンドルとマイクの存在
    if (!Handle)
    {
        UE_LOG(LogTemp, Error, TEXT("CosmVI Error: Handle is NULL. Did Initialize succeed?"));
        return false;
    }
    if (!VoiceCapture.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("CosmVI Error: VoiceCapture is INVALID. Failed to create microphone instance."));
        return false;
    }

    // 2. コアライブラリの開始
    cosmVI_Status Status = cosmVI_Start(Handle);
    if (Status != COSMVI_SUCCESS)
    {
        // エラーコードを数値で表示 (0=Success, 5=AlreadyRunning, 6=NotRunning, etc.)
        UE_LOG(LogTemp, Error, TEXT("CosmVI Error: cosmVI_Start failed with Status Code: %d"), (int32)Status);

        // すでに動いているならTrue扱いで良いかな？
        if (Status == COSMVI_ERROR_ALREADY_RUNNING)
        {
            UE_LOG(LogTemp, Warning, TEXT("CosmVI: Library was already running. Continuing..."));
        }
        else
        {
            return false;
        }
    }

    // 3. マイクの開始
    if (!VoiceCapture->Start())
    {
        UE_LOG(LogTemp, Error, TEXT("CosmVI Error: VoiceCapture->Start() FAILED!"));
        UE_LOG(LogTemp, Error, TEXT("Please [Voice] bEnabled=true is missing in DefaultEngine.ini"));


        // マイクが開けなかったのでライブラリも止める
        cosmVI_Stop(Handle);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("CosmVI: StartRecognition SUCCESS. Listening..."));
    return true;
}

bool UCosmVISubsystem::StopRecognition()
{
    if (!Handle) return false;

    // マイク停止
    if (VoiceCapture.IsValid())
    {
        VoiceCapture->Stop();
    }

    // ライブラリ停止
    return cosmVI_Stop(Handle) == COSMVI_SUCCESS;
}

void UCosmVISubsystem::Tick(float DeltaTime)
{
    if (!Handle || !VoiceCapture.IsValid()) return;

    // 1. マイクから音声を吸い出してライブラリに供給
    if (VoiceCapture->IsCapturing())
    {
        uint32 BytesAvailable = 0;

        // 溜まったデータを全て処理する
        while (true)
        {
            EVoiceCaptureState::Type CaptureState = VoiceCapture->GetCaptureState(BytesAvailable);

            if (CaptureState != EVoiceCaptureState::Ok || BytesAvailable == 0)
            {
                break;
            }

            VoiceCaptureBuffer.Reset();
            VoiceCaptureBuffer.AddUninitialized(BytesAvailable);

            uint32 ReadBytes = 0;
            VoiceCapture->GetVoiceData(VoiceCaptureBuffer.GetData(), BytesAvailable, ReadBytes);

            cosmVI_FeedAudio(Handle, reinterpret_cast<int16_t*>(VoiceCaptureBuffer.GetData()), ReadBytes);
        }
    }


    // 2. ライブラリからのイベントをポーリングして通知
    cosmVI_Event Event;
    // キューが空になるまで取り出す
    while (cosmVI_PollEvent(Handle, &Event) == COSMVI_SUCCESS)
    {
        // イベントが「なし」の場合はループを抜ける（念の為）
        if (Event.type == COSMVI_EVENT_NONE) break;

        FString TextData = UTF8_TO_TCHAR(Event.data.result_text);

        switch (Event.type)
        {
        case COSMVI_EVENT_PARTIAL_RESULT:
        {
            OnPartialResult.Broadcast(TextData);
            break;
        }

        case COSMVI_EVENT_FINAL_RESULT:
        {
            OnFinalResult.Broadcast(TextData);
            //UE_LOG(LogTemp, Log, TEXT("CosmVI Final: %s"), *TextData); // デバックの時は、ここのコメントアウトを外すと、分かりやすい
            break;
        }

        case COSMVI_EVENT_ERROR:
        {
            FString ErrorMsg = UTF8_TO_TCHAR(Event.data.error_info.error_message);
            //UE_LOG(LogTemp, Error, TEXT("CosmVI Error: %s"), *ErrorMsg); // デバックの時は、ここのコメントアウトを外すと、分かりやすい
            OnErrorOccurred.Broadcast(ErrorMsg);
            break;
        }
        case COSMVI_EVENT_STATE_CHANGED:
        {
            ECosmVIState NewBPState = ECosmVIState::Error;

            // CのenumをUEのenumに変換
            switch (Event.data.new_state)
            {
            case COSMVI_STATE_UNINITIALIZED: NewBPState = ECosmVIState::Uninitialized; break;
            case COSMVI_STATE_INITIALIZING:  NewBPState = ECosmVIState::Initializing; break;
            case COSMVI_STATE_IDLE:          NewBPState = ECosmVIState::Idle; break;
            case COSMVI_STATE_RUNNING:       NewBPState = ECosmVIState::Running; break;
            case COSMVI_STATE_PAUSED:        NewBPState = ECosmVIState::Paused; break;
            case COSMVI_STATE_STOPPING:      NewBPState = ECosmVIState::Stopping; break;
            case COSMVI_STATE_ERROR:         NewBPState = ECosmVIState::Error; break;
            }

            // Blueprintに通知！
            if (OnStateChanged.IsBound())
            {
                OnStateChanged.Broadcast(NewBPState);
            }

            // ログにも出しておく
            UE_LOG(LogTemp, Log, TEXT("CosmVI State Changed to: %d"), (int32)NewBPState);
            break;
        }

        default:
            break;
        }
    }
}

bool UCosmVISubsystem::SetGrammar(const FString& JSONString)
{
    if (!Handle) return false;

    // 例外判定はライブラリ
    if (cosmVI_SetGrammar(Handle, TCHAR_TO_UTF8(*JSONString)) == COSMVI_SUCCESS)
    {
        UE_LOG(LogTemp, Log, TEXT("CosmVI: SetGrammar request sent: %s"), *JSONString);
        return true;
    }

    return false;
}

ECosmVIState UCosmVISubsystem::GetCurrentState()
{
    if (!Handle) return ECosmVIState::Uninitialized;

    cosmVI_State RawState;
    if (cosmVI_GetState(Handle, &RawState) != COSMVI_SUCCESS)
    {
        return ECosmVIState::Error;
    }

    // 変換
    switch (RawState)
    {
    case COSMVI_STATE_UNINITIALIZED: return ECosmVIState::Uninitialized;
    case COSMVI_STATE_INITIALIZING:  return ECosmVIState::Initializing;
    case COSMVI_STATE_IDLE:          return ECosmVIState::Idle;
    case COSMVI_STATE_RUNNING:       return ECosmVIState::Running;
    case COSMVI_STATE_PAUSED:        return ECosmVIState::Paused;
    case COSMVI_STATE_STOPPING:      return ECosmVIState::Stopping;
    case COSMVI_STATE_ERROR:         return ECosmVIState::Error;
    default:                         return ECosmVIState::Error;
    }
}

bool UCosmVISubsystem::IsRecognitionRunning()
{
    // 単純に現在の状態が Running かどうか
    return GetCurrentState() == ECosmVIState::Running;
}