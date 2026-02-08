// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "cosmVI_lib.h"
#include "Tickable.h"
#include "Voice.h"
#include "CosmVISubsystem.generated.h"

UENUM(BlueprintType)
enum class ECosmVIState : uint8
{
    Uninitialized UMETA(DisplayName = "Uninitialized"),
    Initializing  UMETA(DisplayName = "Initializing"),
    Idle          UMETA(DisplayName = "Idle (Ready)"),
    Running       UMETA(DisplayName = "Running"),
    Paused        UMETA(DisplayName = "Paused"),
    Stopping      UMETA(DisplayName = "Stopping"),
    Error         UMETA(DisplayName = "Error")
};

UENUM(BlueprintType)
enum class ECosmVILanguage : uint8
{
    Japanese UMETA(DisplayName = "Japanese (ja-small)"),
    English  UMETA(DisplayName = "English (en-small)"),
	Italian UMETA(DisplayName = "Italian (it-small)"),
	French  UMETA(DisplayName = "French (fr-small)"),
	Deutshch UMETA(DisplayName = "Deutshch (de-small)"),
	//Korean UMETA(DisplayName = "Korean (ko-small)"),
	Chinese UMETA(DisplayName = "Chinese (cn-small)"),
};

// --- Blueprintに通知するためのデリゲート定義 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCosmVIResultSignature, FString, Text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCosmVIErrorSignature, FString, ErrorMessage);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCosmVIStateChangedSignature, FString, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCosmVIStateChangedSignature, ECosmVIState, NewState);

/**
 * 音声認識ライブラリを管理するサブシステム
 */
UCLASS()
class COSMVI_API UCosmVISubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    // --- Subsystem ライフサイクル ---
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // --- FTickableGameObject インターフェース ---
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { return TStatId(); }
    virtual bool IsTickable() const override { return !IsTemplate(); } // CDOではTickさせない

    // --- Blueprint Callable API ---

    /**
     * システム状態が変化したときに通知します。
     * @return string 新しい状態 ("Idle", "Listening", "Processing" など)
	 */
    UPROPERTY(BlueprintAssignable, Category = "CosmVI")
    FCosmVIStateChangedSignature OnStateChanged;

    /**
     * 現在のライブラリの状態を取得します。
     * 変数のゲッターみたい。
     */
    UFUNCTION(BlueprintPure, Category = "CosmVI")
    ECosmVIState GetCurrentState();

    /**
     * 現在、音声認識が実行中かどうかを判定します。
     * (GetCurrentState() == Running のショートカット)
     */
    UFUNCTION(BlueprintPure, Category = "CosmVI")
    bool IsRecognitionRunning();

    /**
     * 音声認識の初期化（モデルロード）
     * @param Language 使用する言語モデル
     */
    UFUNCTION(BlueprintCallable, Category = "CosmVI")
    bool InitializeCosmVI(ECosmVILanguage Language = ECosmVILanguage::Japanese);

    /**
     * 言語（モデル）を動的に切り替えます。
     */
    UFUNCTION(BlueprintCallable, Category = "CosmVI")
    bool SwitchLanguage(ECosmVILanguage NewLanguage);

    /**
     * 音声認識を開始します。
     * @return 成功したらtrue、マイクがない場合などはfalse
     */
    UFUNCTION(BlueprintCallable, Category = "CosmVI")
    bool StartRecognition();

    /**
     * 音声認識を停止します。
     * @return 成功したらtrue
	 */
    UFUNCTION(BlueprintCallable, Category = "CosmVI")
    bool StopRecognition();

    // --- イベント通知用 (BlueprintでBindできる) ---
    /**
	 * 認識途中の結果を通知します。
	 * @return string 認識途中のテキスト
     */
    UPROPERTY(BlueprintAssignable, Category = "CosmVI")
    FCosmVIResultSignature OnPartialResult; // 認識途中

    /**
     * 認識確定の結果を通知します。
     * @return string 認識確定のテキスト
	 */
    UPROPERTY(BlueprintAssignable, Category = "CosmVI")
    FCosmVIResultSignature OnFinalResult;   // 認識確定

    /**
     * エラー発生時に通知します。
	 * @return string エラーメッセージ
     */
    UPROPERTY(BlueprintAssignable, Category = "CosmVI")
    FCosmVIErrorSignature OnErrorOccurred;

    /**
     * 認識する単語リスト（文法、JSON形式）を設定します。
     * @param JSONString 例: ["前進", "停止", "[unk]"]
     *                   空文字 "" を渡すと制限を解除します。
     */
    UFUNCTION(BlueprintCallable, Category = "CosmVI")
    bool SetGrammar(const FString& JSONString);

private:
    FString GetModelFolderName(ECosmVILanguage Language);

    cosmVI_Handle Handle = nullptr;

    TSharedPtr<IVoiceCapture> VoiceCapture;

    TArray<uint8> VoiceCaptureBuffer;
};